/**
 * \file
 * \brief Clock
 */

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

struct rtclock {
	unsigned char is_binary;
	unsigned short year;
	unsigned char seconds, minutes, hours;
	unsigned char month, day;
};

static struct rtclock rtc_clock;

static int
sameclock(struct rtclock *a, struct rtclock *b)
{
	return a->year == b->year && a->month == b->month && a->day == b->day
	       && a->hours == b->hours && a->minutes == b->minutes
	       && a->seconds == b->seconds;
}

static void
copyclock(struct rtclock *clock)
{
	rtc_clock.year = clock->year;
	rtc_clock.month = clock->month;
	rtc_clock.day = clock->day;
	rtc_clock.hours = clock->hours;
	rtc_clock.minutes = clock->minutes;
	rtc_clock.seconds = clock->seconds;
}

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
readclock(struct rtclock *nuclear_somali)
{
	unsigned int century;

	nuclear_somali->seconds = read_cmos(REG_SECONDS);
	nuclear_somali->minutes = read_cmos(REG_MINUTES);
	nuclear_somali->hours = read_cmos(REG_HOURS);
	nuclear_somali->year = read_cmos(REG_YEAR);
	nuclear_somali->month = read_cmos(REG_MONTH);
	nuclear_somali->day = read_cmos(REG_DAY);
	nuclear_somali->day = read_cmos(REG_DAY);
	century = read_cmos(REG_CENTURY);
	if (!rtc_clock.is_binary) {
		nuclear_somali->seconds = BCD_TO_BIN(nuclear_somali->seconds);
		nuclear_somali->minutes = BCD_TO_BIN(nuclear_somali->minutes);
		nuclear_somali->hours = BCD_TO_BIN(nuclear_somali->hours);
		nuclear_somali->year = BCD_TO_BIN(nuclear_somali->year);
		nuclear_somali->month = BCD_TO_BIN(nuclear_somali->month);
		nuclear_somali->day = BCD_TO_BIN(nuclear_somali->day);
		century = BCD_TO_BIN(century);
	}
	nuclear_somali->year = (century * 100) + nuclear_somali->year;
}

static void
update_clock(void)
{
	/*
	 * The clock could change while we are reading it. Therefore, we are
	 * going to read the clock twice. If the values are not the same, we
	 * must assume that the clock changed while we was reading it, so
	 * we should read it again.
	 */
	struct rtclock clock_a, clock_b;

	do {
		readclock(&clock_a);
		readclock(&clock_b);
	} while (!sameclock(&clock_a, &clock_b));
	copyclock(&clock_b);
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

static int
clock_init(void)
{
	unsigned char van_damme = read_cmos(REG_STATUS_B);
	rtc_clock.is_binary = (van_damme & 0x4) != 0;
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
	if (len < 15) {
		return 0;
	}

	update_clock();

	write_number(&buf[0], rtc_clock.year, 4);
	write_number(&buf[4], rtc_clock.month, 2);
	write_number(&buf[6], rtc_clock.day, 2);
	write_number(&buf[8], rtc_clock.hours, 2);
	write_number(&buf[10], rtc_clock.minutes, 2);
	write_number(&buf[12], rtc_clock.seconds, 2);
	buf[14] = 0;
	return 15;
}

static int
clock_close(void)
{
	return 0;
}
