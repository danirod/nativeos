/**
 * \file
 * \brief Virtual Console
 *
 * The Virtual Console is a character device that abstracts the framebuffer
 * as a character device, so that the higher level code doesn't have to do
 * manually dangerous things - such as manipulating the framebuffer directly,
 * in case the protocol ever changes from VGA/CGA to something like VESA.
 */

#include <config.h>
#include <device/vgafb.h>
#include <device/vtcon/scancodes.h>
#include <sys/device.h>
#include <sys/stdkern.h>
#include <sys/vfs.h>

static int vtcon_init(void);

#define VGA_COLS 80
#define VGA_ROWS 25
#define VGA_SIZE (VGA_COLS * VGA_ROWS)
#define VGA_ENTRY(char, fg, bg) (char | fg << 8 | bg << 12)

#define KBD_MOD_SHIFT 0x01
#define KBD_MOD_CTRL 0x02
#define KBD_MOD_ALT 0x04
#define KBD_MOD_META 0x08
#define KBD_CAP_UP 0x10
#define KBD_LOCK_CAPS 0x100
#define KBD_MODS (KBD_MOD_SHIFT | KBD_MOD_CTRL | KBD_MOD_ALT)

struct vtcontext {
	unsigned int cx, cy;
	unsigned char fg, bg;
	unsigned int kbd_status;
	unsigned short buffer[VGA_COLS * VGA_ROWS];
};

static vfs_node_t *con_fb, *con_kbd;
static struct vtcontext contexts[8];
static unsigned int current_context = 0;
static struct vtcontext *context = &contexts[0];
static vfs_node_t *clock = 0;

static void
drawstatus()
{
	char text[80], date[15];
	unsigned char fg, bg;
	unsigned short buffer[80];
	unsigned int i;

	/* Compose status bar text. */
	memset(text, 0, 80);
	text[1] = '1' + current_context;
	strncpy(&text[4], VERSION_NAME, 70);
	fs_read(clock, 0, &date, 15);

	/* Render status bar. */
	for (i = 0; i < VGA_COLS; i++) {
		fg = i < 3 ? 0x1 : 0x7;
		bg = fg == 0x7 ? 0x1 : 0x7;
		buffer[i] = VGA_ENTRY(text[i], fg, bg);
	}

	/* TODO: Add a sprintf function to never do this again. */
	buffer[VGA_COLS - 21] = VGA_ENTRY(' ', 0x1, 0x7);
	buffer[VGA_COLS - 20] = VGA_ENTRY(date[0], 0x1, 0x7);
	buffer[VGA_COLS - 19] = VGA_ENTRY(date[1], 0x1, 0x7);
	buffer[VGA_COLS - 18] = VGA_ENTRY(date[2], 0x1, 0x7);
	buffer[VGA_COLS - 17] = VGA_ENTRY(date[3], 0x1, 0x7);
	buffer[VGA_COLS - 16] = VGA_ENTRY('/', 0x1, 0x7);
	buffer[VGA_COLS - 15] = VGA_ENTRY(date[4], 0x1, 0x7);
	buffer[VGA_COLS - 14] = VGA_ENTRY(date[5], 0x1, 0x7);
	buffer[VGA_COLS - 13] = VGA_ENTRY('/', 0x1, 0x7);
	buffer[VGA_COLS - 12] = VGA_ENTRY(date[6], 0x1, 0x7);
	buffer[VGA_COLS - 11] = VGA_ENTRY(date[7], 0x1, 0x7);
	buffer[VGA_COLS - 10] = VGA_ENTRY(' ', 0x1, 0x7);
	buffer[VGA_COLS - 9] = VGA_ENTRY(date[8], 0x1, 0x7);
	buffer[VGA_COLS - 8] = VGA_ENTRY(date[9], 0x1, 0x7);
	buffer[VGA_COLS - 7] = VGA_ENTRY(':', 0x1, 0x7);
	buffer[VGA_COLS - 6] = VGA_ENTRY(date[10], 0x1, 0x7);
	buffer[VGA_COLS - 5] = VGA_ENTRY(date[11], 0x1, 0x7);
	buffer[VGA_COLS - 4] = VGA_ENTRY(':', 0x1, 0x7);
	buffer[VGA_COLS - 3] = VGA_ENTRY(date[12], 0x1, 0x7);
	buffer[VGA_COLS - 2] = VGA_ENTRY(date[13], 0x1, 0x7);
	buffer[VGA_COLS - 1] = VGA_ENTRY(' ', 0x1, 0x7);

	fs_write(con_fb, 2 * VGA_COLS * (VGA_ROWS - 1), buffer, sizeof(buffer));
}

static void
resetcontext(unsigned int i)
{
	unsigned int pos;

	contexts[i].cx = 0;
	contexts[i].cy = 0;
	contexts[i].fg = 7;
	contexts[i].bg = 0;
	for (pos = 0; pos < VGA_ROWS * VGA_COLS; pos++) {
		contexts[i].buffer[pos] = VGA_ENTRY(' ', 0x7, 0x0);
	}
}

static void
resetcontexts()
{
	unsigned int i;
	for (i = 0; i < 8; i++) {
		resetcontext(i);
	}
}

static inline void
syncfbcursor()
{
	unsigned short abspos = context->cy * VGA_COLS + context->cx;
	fs_ioctl(con_fb, VGAFB_IOCTL_MOVECUR, &abspos);
}

static void
switchcontext(unsigned int to)
{
	if (to < 8) {
		fs_read(con_fb, 0, context->buffer, sizeof(context->buffer));
		current_context = to;
		context = &contexts[current_context];
		fs_write(con_fb, 0, context->buffer, sizeof(context->buffer));
		drawstatus();
		syncfbcursor();
	}
}

static void
clearscreen()
{
	int x, y;
	unsigned int pos;
	unsigned short entry;

	for (y = 0; y < VGA_ROWS; y++) {
		for (x = 0; x < VGA_COLS; x++) {
			pos = y * VGA_COLS + x;
			entry = VGA_ENTRY(' ', context->fg, context->bg);
			fs_write(con_fb, 2 * pos, &entry, 2);
		}
	}

	context->cx = 0;
	context->cy = 0;
	drawstatus();
	syncfbcursor();
}

static inline void
copyrow(unsigned int dst, unsigned int src)
{
	unsigned char buf[VGA_COLS * 2];
	unsigned int dstofft, srcofft, read;
	if (dst < VGA_ROWS && src < VGA_ROWS) {
		srcofft = VGA_COLS * src * 2;
		dstofft = VGA_COLS * dst * 2;
		read = fs_read(con_fb, srcofft, buf, VGA_COLS * 2);
		fs_write(con_fb, dstofft, buf, read);
	}
}

static inline void
clearrow(unsigned int row)
{
	unsigned int i;
	unsigned int rowofft = VGA_COLS * row * 2;
	unsigned short value = context->fg << 8;
	for (i = 0; i < VGA_COLS; i++)
		fs_write(con_fb, rowofft + 2 * i, &value, 2);
}

static inline void
moveline()
{
	unsigned int row;
	if (++context->cy == VGA_ROWS - 1) {
		for (row = 1; row < VGA_ROWS - 1; row++)
			copyrow(row - 1, row);
		clearrow(VGA_ROWS - 2);
		context->cy = VGA_ROWS - 2;
	}
}

static inline void
movecursor()
{
	if (++context->cx == VGA_COLS) {
		context->cx = 0;
		moveline();
	}
}

static void
putchar(unsigned int ch)
{
	int pos;
	short entry;
	switch (ch) {
	case '\b':
		if (context->cx > 0) {
			context->cx--;
		}
		break;
	case '\t':
		do {
			context->cx++;
		} while (context->cx % 7);
		if (context->cx >= VGA_COLS) {
			context->cx = 0;
			moveline();
		}
		break;
	case '\n':
		moveline();
	case '\r':
		context->cx = 0;
		break;
	default:
		pos = context->cy * VGA_COLS + context->cx;
		entry = VGA_ENTRY(ch, context->fg, context->bg);
		fs_write(con_fb, 2 * pos, &entry, 2);
		movecursor();
		break;
	}
	drawstatus();
	syncfbcursor();
}

static void
putstr(char *str)
{
	if (str) {
		while (*str) {
			putchar(*str++);
		}
	}
}

static int
get_single_scancode(unsigned char key)
{
	scancode_t *scancode;
	char idx, *scancode_bytes;

	if (key > 0x58) {
		return VK_NUL;
	}
	scancode = &us_scancodes_1[key];
	scancode_bytes = (char *) scancode;
	idx = context->kbd_status & KBD_MODS;
	return scancode_bytes[idx];
}

static inline void
releasebit(unsigned short bit, unsigned int rel)
{
	if (rel) {
		context->kbd_status &= ~bit;
	} else {
		context->kbd_status |= bit;
	}
}

static inline void
togglekbdbit(unsigned short bit)
{
	context->kbd_status ^= bit;
}

static int
decode_scancode(kbdev_t *kbdev, unsigned char *buf, unsigned int len)
{
	unsigned int i;
	unsigned char keycode, rel, status, vk;

	if (len == 1) {
		keycode = buf[0] & 0x7F;
		rel = buf[0] & 0x80;
		vk = get_single_scancode(keycode);

		switch (vk) {
		case VK_LCTL:
			releasebit(KBD_MOD_CTRL, rel);
			break;
		case VK_LSHF:
		case VK_RSHF:
			releasebit(KBD_MOD_SHIFT, rel);
			break;
		case VK_LALT:
			releasebit(KBD_MOD_ALT, rel);
			break;
		case VK_CAPL:
			if (!rel)
				togglekbdbit(KBD_LOCK_CAPS);
			break;
		}

		kbdev->flags = context->kbd_status;
		if (rel) {
			kbdev->flags |= KBD_CAP_UP;
		}
		kbdev->vk = vk;
		return 0;
	} else if (len == 2) {
		/* One of those extended keys. */
		if (buf[0] != 0xE0) {
			/* Unless I'm using layer 2, I have no idea. */
			return -1;
		}
		keycode = buf[1] & 0x7F;
		rel = buf[1] & 0x80;

		switch (keycode) {
		case 0x1C:
			vk = VK_LF;
			break;
		case 0x1D:
			vk = VK_RCTL;
			releasebit(KBD_MOD_CTRL, rel);
			break;
		case 0x35:
			vk = '/';
			break;
		case 0x38:
			vk = VK_RALT;
			releasebit(KBD_MOD_ALT, rel);
			break;
		case 0x47:
			vk = VK_HOME;
			break;
		case 0x48:
			vk = VK_CUP;
			break;
		case 0x49:
			vk = VK_PGUP;
			break;
		case 0x4B:
			vk = VK_CLFT;
			break;
		case 0x4D:
			vk = VK_CRGT;
			break;
		case 0x4F:
			vk = VK_END;
			break;
		case 0x50:
			vk = VK_CDWN;
			break;
		case 0x51:
			vk = VK_PGDN;
			break;
		case 0x52:
			vk = VK_INSERT;
			break;
		case 0x53:
			vk = VK_DEL;
			break;
		case 0x5B:
		case 0x59:
			releasebit(KBD_MOD_META, rel);
			break;
		}
		kbdev->flags = context->kbd_status;
		if (rel) {
			kbdev->flags |= KBD_CAP_UP;
		}
		kbdev->vk = vk;
		return 0;
	}

	return -1;
}

static void
echo_scancode(kbdev_t *kbdev)
{
	if (kbdev->flags & KBD_CAP_UP) {
		return;
	}
	switch (kbdev->vk) {
	case VK_NUL:
		putstr("^@");
		break;
	case VK_SOH:
		putstr("^A");
		break;
	case VK_STX:
		putstr("^B");
		break;
	case VK_ETX:
		putstr("^C");
		break;
	case VK_EOT:
		putstr("^D");
		break;
	case VK_ENQ:
		putstr("^E");
		break;
	case VK_ACK:
		putstr("^F");
		break;
	case VK_BEL:
		putstr("^G");
		break;
	case VK_BS:
		putchar('\b');
		break;
	case VK_HT:
		putchar('\t');
		break;
	case VK_LF:
		putchar('\n');
		break;
	case VK_VT:
		putstr("^K");
		break;
	case VK_FF:
		clearscreen();
		break;
	case VK_CR:
		putchar('\r');
		break;
	case VK_SO:
		putstr("^N");
		break;
	case VK_SI:
		putstr("^O");
		break;
	case VK_DLE:
		putstr("^P");
		break;
	case VK_DC1:
		putstr("^Q");
		break;
	case VK_DC2:
		putstr("^R");
		break;
	case VK_DC3:
		putstr("^S");
		break;
	case VK_DC4:
		putstr("^T");
		break;
	case VK_NAK:
		putstr("^U");
		break;
	case VK_SYN:
		putstr("^V");
		break;
	case VK_ETB:
		putstr("^W");
		break;
	case VK_CAN:
		putstr("^X");
		break;
	case VK_EM:
		putstr("^Y");
		break;
	case VK_SUB:
		putstr("^Z");
		break;
	case VK_ESC:
		putstr("^[");
		break;
	case VK_FS:
		putstr("^\\");
		break;
	case VK_GS:
		putstr("^]");
		break;
	case VK_RS:
		putstr("^^");
		break;
	case VK_US:
		putstr("^_");
		break;
	case VK_CUP:
		putstr("^[[A");
		break;
	case VK_CDWN:
		putstr("^[[B");
		break;
	case VK_CRGT:
		putstr("^[[C");
		break;
	case VK_CLFT:
		putstr("^[[D");
		break;
	case VK_HOME:
		putstr("^[[H");
		break;
	case VK_END:
		putstr("^[[F");
		break;
	case VK_INSERT:
		putstr("^[[2~");
		break;
	case VK_DEL:
		putstr("^[[3~");
		break;
	case VK_PGUP:
		putstr("^[[5~");
		break;
	case VK_PGDN:
		putstr("^[[6~");
		break;
	case VK_F1:
		switchcontext(0);
		break;
	case VK_F2:
		switchcontext(1);
		break;
	case VK_F3:
		switchcontext(2);
		break;
	case VK_F4:
		switchcontext(3);
		break;
	case VK_F5:
		switchcontext(4);
		break;
	case VK_F6:
		switchcontext(5);
		break;
	case VK_F7:
		switchcontext(6);
		break;
	case VK_F8:
		switchcontext(7);
		break;
	case VK_F9:
		putstr("^[[20~");
		break;
	case VK_F10:
		putstr("^[[21~");
		break;
	case VK_F11:
		putstr("^[[23~");
		break;
	case VK_F12:
		putstr("^[[24~");
		break;
	default:
		if (kbdev->vk >= 27 && kbdev->vk < 127) {
			putchar(kbdev->vk);
		}
		break;
	}
}

static unsigned int
vtcon_read(unsigned char *buf, unsigned int len)
{
	unsigned int kbd_len, read_bytes = 0;
	unsigned char kbd_buf[16];
	kbdev_t kbdev;

	/* TODO: I'm going to hell for doing this in this function. */
	drawstatus();

	while (len >= sizeof(kbdev_t)) {
		kbd_len = fs_read(con_kbd, 0, kbd_buf, 16);
		if (kbd_len == 0) {
			break;
		}
		if (decode_scancode(&kbdev, kbd_buf, kbd_len) == 0) {
			echo_scancode(&kbdev);
			memcpy(buf, &kbdev, sizeof(kbdev_t));
			len -= sizeof(kbdev_t);
			read_bytes += sizeof(kbdev_t);
		}
	}
	return read_bytes;
}

static unsigned int
vtcon_write(unsigned char *buf, unsigned int len)
{
	unsigned int rem = len;
	while (rem--) {
		putchar(*buf);
		buf++;
	}
	return len;
}

static int
try_open_fb()
{
	con_fb = fs_resolve("DEV:/fb");
	if (con_fb && fs_open(con_fb, VO_FWRITE) == 0) {
		return 0;
	}
	return -1;
}

static int
try_open_kbd()
{
	con_kbd = fs_resolve("DEV:/kbd");
	if (con_kbd && fs_open(con_kbd, VO_FREAD) == 0) {
		return 0;
	}
	return -1;
}

static int
try_open_clock()
{
	clock = fs_resolve("DEV:/clock");
	if (clock && fs_open(clock, VO_FREAD) == 0) {
		return 0;
	}
	return -1;
}

static int
try_close_kbd()
{
	if (fs_close(con_fb) != 0) {
		return -1;
	}
	return 0;
}

static int
try_close_clock()
{
	if (fs_close(clock) != 0) {
		return -1;
	}
	return 0;
}

static int
try_close_fb()
{
	if (fs_close(con_fb) != 0) {
		return -1;
	}
	return 0;
}

static int
vtcon_open(unsigned int flags)
{
	if (flags & VO_FREAD)
		/* This is a write only device. */
		return -1;
	if (con_fb)
		/* Device is already opened. */
		return -1;
	if (try_open_fb() < 0) {
		return -1;
	}
	if (try_open_kbd() < 0) {
		try_close_fb();
		return -1;
	}
	if (try_open_clock() < 0) {
		try_close_kbd();
		try_close_fb();
		return -1;
	}
	drawstatus();
	syncfbcursor();
	return 0;
}

static int
vtcon_ioctl(int op, void *argp)
{
	if (op == 0) {
		clearscreen();
		return 0;
	}
	return -1;
}

static int
vtcon_close()
{
	try_close_clock();
	try_close_kbd();
	try_close_fb();
	resetcontexts();
	return 0;
}

static driver_t vtcon_driver = {
    .drv_name = "vgacon",
    .drv_flags = DV_FCHARDEV,
    .drv_init = &vtcon_init,
};

static device_t vtcon_device = {
    .dev_family = &vtcon_driver,
    .dev_open = &vtcon_open,
    .dev_read_chr = &vtcon_read,
    .dev_write_chr = &vtcon_write,
    .dev_ioctl = &vtcon_ioctl,
    .dev_close = &vtcon_close,
};

static int
vtcon_init(void)
{
	resetcontexts();
	device_install(&vtcon_device, "vtcon");
	return 0;
}

DEVICE_DESCRIPTOR(vtcon, vtcon_driver);
