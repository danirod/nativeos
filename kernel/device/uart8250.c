/**
 * \file
 * \brief UART 8250 driver
 *
 * UART 8250 is the basic serial port for the PC platform. While the COM port
 * is deprecated and most computers don't have a COM port these days, it is
 * still a extremely popular debug tool, because most virtual machines will
 * provide a way to emulate COM ports, allowing to be used as a log or as a
 * quick shell.
 *
 * The PC architecture supports up to four serial ports. At the moment,
 * NativeOS will only support one serial port. Communication between the PC
 * and the UART is handled via IO ports. There are eight ports per UART.
 * For instance, for the first UART device these ports will be 0x3F8 to 0x3FF.
 *
 * Use cases for the UART port:
 *
 * \li The most important thing about the UART is the RX/TX (receive/transmit).
 *     OUT a byte to the 0x3F8 port, it will be send to the serial port.
 *     IN a byte from the 0x3F8 port, it will be received from the serial port.
 * \li Then there is the line status register. It is in PORT+5 and you read
 *     the bits, you can have information about the status of the port, such as
 *     whether there is data waiting to be read.
 * \li Settings are configured using the Line Control Register, which is in
 *     PORT+3. Update the value of this port to tweak the behaviour of the
 *     serial port. Should be used to tweak the preferences like baud rate or
 *     parity.
 *
 * Settings that can be configured realisticly:
 *
 * \li The baud rate. Turn the DLAB bit ON in the line control register, send
 *     the divisor for 115200 to use through PORT (LSB) and PORT+1 (MSB), turn
 *     off the DLAB bit.
 * \li The data bits. Use bits 1 and 0 of the line control register to set it
 *     to 5 (00), 6 (01), 7 (10) or 8 (11). Nowadays 8 is the standard, and
 *     also the only accepted configuration at the moment.
 * \li The stop bits, which are used for sync the bits of each byte. Set the
 *     bit 2 of the line control register to 0 for 1 bit, or 1 for 1.5 or 2
 *     bits (this use case depends on even bits per character). These days,
 *     standard is 1 bit.
 * \li The parity, used for basic checksums to detect transmission errors.
 *     Use bit 3 to enable parity (0 = OFF, 1 = ON), and if ON, use the bits
 *     4 and 5 to set the parity mode (00 = ODD, 01 = EVEN, 10 = MARK,
 *     11 = SPACE). These days, standard is NO PARITY.
 * \li Interrupts. With DLAB off, write a value to PORT+1 to configure the
 *     interrupt mode. Depending on the bits that are set to 1, you will
 *     receive more or less interruptions. Bit 0 = data available, bit 1 =
 *     transmitter is empty, bit 2 = error or break, bit 3 = status changed.
 *
 * Interrupts for the serial port will come from IRQ 12 for the first port,
 * and IRQ 11 for the second port. Assuming that protected mode is enabled,
 * which will be.
 */
#include <kernel/cpu/idt.h>
#include <kernel/cpu/io.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/device.h>
#include <sys/ringbuf.h>
#include <sys/vfs.h>

#define UART_PORT 0x3F8

#define BAUD_RATE_115200 0
#define BAUD_RATE_38400 1
#define BAUD_RATE_19200 2
#define BAUD_RATE_9600 3
#define BAUD_RATE_7200 4
#define BAUD_RATE_4800 5
#define BAUD_RATE_3600 6
#define BAUD_RATE_2400 7
#define BAUD_RATE_2000 8
#define BAUD_RATE_1800 9
#define BAUD_RATE_1200 10
#define BAUD_RATE_600 11
#define BAUD_RATE_300 12
#define BAUD_RATE_150 13
#define BAUD_RATE_110 14
#define BAUD_RATE_50 15

/**
 * \brief List of divisor values
 * These are the accepted divisor values that can be set through the DLAB port
 * when the baud rate is being configured. It is possible to pick one of these
 * through code by using the BAUD_RATE constant definitions.
 */
static unsigned short uart8250_baud_divisors[] = {
    0x001, // 115200
    0x003, // 38400
    0x006, // 19200
    0x00C, // 9600
    0x010, // 7200
    0x018, // 4800
    0x020, // 3600
    0x030, // 2400
    0x03A, // 2000
    0x040, // 1800
    0x060, // 1200
    0x0C0, // 600
    0x180, // 300
    0x300, // 150
    0x417, // 110
    0x900, // 50
    0x000, // NUL
};

static struct {
	uint32_t flags; /**< Holds the flags of the current open mode. */
	ringbuf_t *rx_buf; /**< Ringbuffer to store the bytes until read. */
} uart8250_context;

/**
 * \brief Reconfigure the UART device
 * Based on the current configuration of the context, set the device
 */
static void
uart8250_reconfigure(void)
{
	unsigned short divisor = uart8250_baud_divisors[BAUD_RATE_19200];
	IO_OutP(UART_PORT + 1, 0x00); // turn off interrupts

	// Set the baud rate.
	IO_OutP(UART_PORT + 3, 0x80); // DLAB = on
	IO_OutP(UART_PORT + 0, divisor & 0xFF);
	IO_OutP(UART_PORT + 1, divisor >> 8);

	// Configure interrupts and status line.
	// WHY DO I WASTE SO MANY HOURS DEBUGGING THIS PIECE OF ANCIENT SHIT
	IO_OutP(UART_PORT + 3, 0x03); // 8N1 Mode
	// IO_OutP(UART_PORT + 2, 0xC7); // FIFO and all this shit
	IO_OutP(UART_PORT + 4, 0x07); // Modem, RTS, DTR, Interrupts ON
	IO_OutP(UART_PORT + 1, 0x01); // Actually turn on ready interrupts
}

static void
acknowledge(void)
{
	IO_InP(UART_PORT + 2);
}

static int uart8250_init(void);
static void uart8250_tini(void);
static int uart8250_open(unsigned int flags);
static int uart8250_close(void);
static uint32_t uart8250_read(unsigned char *buf, uint32_t len);
static uint32_t uart8250_write(unsigned char *buf, uint32_t len);

static driver_t uart8250_driver = {
    .dv_name = "UART8250",
    .dv_flags = DV_FCHARDEV,
    .dv_init = &uart8250_init,
    .dv_tini = &uart8250_tini,
};

static chardev_t uart8250_chardev = {
    .cd_family = &uart8250_driver,
    .cd_open = &uart8250_open,
    .cd_read = &uart8250_read,
    .cd_write = &uart8250_write,
    .cd_close = &uart8250_close,
};

static void
uart8250_interrupt(struct idt_data *idt)
{
	uint8_t value;

	/* Assert there is data. */
	while (IO_InP(UART_PORT + 5) & 1) {
		value = IO_InP(UART_PORT);
		if (uart8250_context.flags & VO_FWRITE) {
			ringbuf_write(uart8250_context.rx_buf, value);
		}
	}
	acknowledge();
}

static int
uart8250_init(void)
{
	device_install(&uart8250_chardev, "uart");
	uart8250_reconfigure();
	idt_set_handler(0x24, &uart8250_interrupt);
	acknowledge();
	return 0;
}

static void
uart8250_tini(void)
{
	device_remove("uart");
}

static int
uart8250_open(unsigned int flags)
{
	/* Device is already opened. */
	if (uart8250_context.flags) {
		return -1;
	}

	/* Reserve the device. */
	uart8250_context.flags = flags;
	uart8250_context.rx_buf = ringbuf_alloc(4096);

	return 0;
}

static int
uart8250_close(void)
{
	/* Is this possible? Do I need a spinlock? I don't want to know. */
	if (!uart8250_context.flags) {
		return -1;
	} else {
		ringbuf_free(uart8250_context.rx_buf);
		uart8250_context.flags = 0;
		return 0;
	}
}

static uint32_t
uart8250_read(unsigned char *buf, uint32_t len)
{
	size_t read_bytes = 0;
	while (read_bytes <= len
	       && ringbuf_test_ready(uart8250_context.rx_buf)) {
		*buf++ = ringbuf_read(uart8250_context.rx_buf);
		read_bytes++;
	}
	return read_bytes;
}

static uint32_t
uart8250_write_binary(unsigned char *buf, uint32_t len)
{
	unsigned int write_bytes = 0;
	while (len--) {
		IO_OutP(UART_PORT, *buf++);
		write_bytes++;
	}
	return write_bytes;
}

static uint32_t
uart8250_write_non_binary(unsigned char *buf, uint32_t len)
{
	unsigned int write_bytes = 0;
	while (len--) {
		switch (*buf) {
		case '\r':
			IO_OutP(UART_PORT, '\r');
			IO_OutP(UART_PORT, '\n');
			write_bytes += 2;
			buf++;
			break;
		case 127:
			IO_OutP(UART_PORT, '\b');
			write_bytes++;
			buf++;
			break;
		default:
			IO_OutP(UART_PORT, *buf++);
			write_bytes++;
			break;
		}
	}
	return write_bytes;
}

static uint32_t
uart8250_write(unsigned char *buf, uint32_t len)
{
	if (!buf || !*buf) {
		return 0;
	}
	if (uart8250_context.flags & VO_FBINARY) {
		return uart8250_write_binary(buf, len);
	} else {
		return uart8250_write_non_binary(buf, len);
	}
}

DEVICE_DESCRIPTOR(uart8250, uart8250_driver);
