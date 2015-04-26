#include <string.h>
#include "bionic.h"
#include "memio.h"
#include "utils.h"
#include "scriptic.h"
#include "fernvale-kbd.h"
#include "fernvale-pmic.h"

static int do_init(void)
{
	/* Disable system watchdog */
	writel(0x2200, 0xa0030000);

	/* Enable USB Download mode (required for no-battery operation) */
	writew(0x8000, PMIC_CTRL10);

	/* Disable battery watchdog */
	writew(0x2, PMIC_CTRL9);

	scriptic_run("set_plls");
	scriptic_run("enable_psram");

	return 0;
}

static int loop(void)
{
    int led = 0;
    for (;;)
    {
        led = !led;
        *((volatile uint32_t *) BIG_LED_ADDR) = led ? BIG_LED_ON : BIG_LED_OFF;

        _msleep(300);
    }
    
    return 0;
}

int main(void)
{
	do_init();

	while (1)
		loop();

	return 0;
}
