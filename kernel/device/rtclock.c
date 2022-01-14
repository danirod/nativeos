/**
 * \file
 * \brief Clock
 */

#include <kernel/cpu/idt.h>
#include <machine/cpu.h>
#include <sys/device.h>

#define REG_SECONDS 0
#define REG_MINUTES 2
#define REG_HOURS 4
#define REG_DAY 7
#define REG_MONTH 8
#define REG_YEAR 9
#define REG_CENTURY 0x32
#define REG_STATUS_B 0xB

/** Converts from RTC-BCD to BIN. */
#define BCD_TO_BIN(bcd) ((bcd / 16) * 10) + (bcd & 0xF)

static struct {
	unsigned char is_24, is_binary;
	unsigned short year;
	unsigned char seconds, minutes, hours;
	unsigned char month, day;
} rtclock;

static unsigned char
read_cmos(unsigned int reg)
{
	unsigned char value;

	/* We do not want interrupts while we are working here. */
	__asm__("cli");
	port_out_byte(0x70, reg);
	value = port_in_byte(0x71);
	__asm__("sti");
	return value;
}

static void
update_clock(void)
{
	unsigned int century;

	rtclock.seconds = read_cmos(REG_SECONDS);
	rtclock.minutes = read_cmos(REG_MINUTES);
	rtclock.hours = read_cmos(REG_HOURS);
	rtclock.year = read_cmos(REG_YEAR);
	rtclock.month = read_cmos(REG_MONTH);
	rtclock.day = read_cmos(REG_DAY);
	rtclock.day = read_cmos(REG_DAY);
	century = read_cmos(REG_CENTURY);
	if (!rtclock.is_binary) {
		rtclock.seconds = BCD_TO_BIN(rtclock.seconds);
		rtclock.minutes = BCD_TO_BIN(rtclock.minutes);
		rtclock.hours = BCD_TO_BIN(rtclock.hours);
		rtclock.year = BCD_TO_BIN(rtclock.year);
		rtclock.month = BCD_TO_BIN(rtclock.month);
		rtclock.day = BCD_TO_BIN(rtclock.day);
		century = BCD_TO_BIN(century);
	}
	rtclock.year = (century * 100) + rtclock.year;
}

static void
clock_interrupt(struct idt_data *data)
{
	update_clock();

	/* Read from register C or the clock will get mad at us. */
	port_out_byte(0x70, 0x0C);
	port_in_byte(0x71);
}

static int clock_init(void);
static int clock_open(unsigned int flags);
static int clock_close(void);
static unsigned int clock_read(unsigned char *buf, unsigned int len);

static driver_t clock_driver = {
    .drv_name = "clock",
    .drv_init = &clock_init,
    .drv_flags = DV_FCHARDEV,
};

DEVICE_DESCRIPTOR(clock, clock_driver);

static device_t clock_device = {
    .dev_family = &clock_driver,
    .dev_close = &clock_close,
    .dev_open = &clock_open,
    .dev_read_chr = &clock_read,
};

static void
install_clock_interrupts()
{
	char status;

	// Turn the interrupt bits on. I don't know. Life is difficult.
	__asm__("cli");
	port_out_byte(0x70, 0x8B);
	status = port_in_byte(0x71);
	status |= 0x40;
	port_out_byte(0x70, 0x8B);
	port_out_byte(0x71, status);
	__asm__("sti");
	idt_set_handler(0x28, clock_interrupt);
}

static int
clock_init(void)
{
	unsigned char van_damme = read_cmos(REG_STATUS_B);
	rtclock.is_24 = (van_damme & 0x2) != 0;
	rtclock.is_binary = (van_damme & 0x4) != 0;
	update_clock();
	install_clock_interrupts();
	device_install(&clock_device, "clock");
	return 0;
}

static int
clock_open(unsigned int flags)
{
	return 0;
}

static void
write_number(unsigned char *buf, unsigned int num, unsigned int len)
{
	char tmp[16];
	int i = 0;

	while (num > 0) {
		tmp[i++] = '0' + (num % 10);
		num /= 10;
	}
	while (i < len) {
		tmp[i++] = '0';
	}
	while (i >= 0) {
		*buf++ = tmp[--i];
	}
}

static unsigned int
clock_read(unsigned char *buf, unsigned int len)
{
	// YYYYMMDDHHMMSS
	if (len < 14) {
		return 0;
	}
	write_number(&buf[0], rtclock.year, 4);
	write_number(&buf[4], rtclock.month, 2);
	write_number(&buf[6], rtclock.day, 2);
	write_number(&buf[8], rtclock.hours, 2);
	write_number(&buf[10], rtclock.minutes, 2);
	write_number(&buf[12], rtclock.seconds, 2);
	return 14;
}

static int
clock_close(void)
{
	return 0;
}
