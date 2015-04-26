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

	scriptic_run("set_kbd");

	return 0;
}

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*x))
#endif

const uint32_t key_addr[] = {
	KBD_MEM2,
	KBD_MEM2,
	KBD_MEM1,
	KBD_MEM1,
	KBD_MEM3,
	KBD_MEM3,
	KBD_MEM2,
	KBD_MEM1,
	KBD_MEM3,
	KBD_MEM2,
	KBD_MEM1,
	KBD_MEM3,
	KBD_MEM2,
	KBD_MEM1,
	KBD_MEM3,
	KBD_MEM2,
	KBD_MEM2,
	KBD_MEM2,
};

const uint32_t key_mask[] = {
	0x0000fffb,
	0x0000fff7,
	0x0000fdff,
	0x0000fbff,
	0x0000ffef,
	0x0000ffdf,
	0x0000ffef,
	0x0000f7ff,
	0x0000ffbf,
	0x0000ffdf,
	0x0000efff,
	0x0000ff7f,
	0x0000ffbf,
	0x0000dfff,
	0x0000feff,
	0x0000dfff,
	0x0000bfff,
	0x00007fff,
};

static int read_keypad()
{
    int key;
    for (key = 0; key < (ARRAY_SIZE(key_addr) - 1); key++) {
        int v = readl(key_addr[key]);
        int newstate = (v == key_mask[key]);

        if (newstate) {
            return key;
        }
    }

    return -1;
}


void reboot()
{
    writel(0x1209, 0xa003001c);
}

int main()
{
	do_init();

        int last_key = -1;
        int new_key = -1;
        int led = 0;

        for (;;)
        {

            new_key = read_keypad();
            if (new_key == -1)
                last_key = -1;

            else if (new_key == ARRAY_SIZE(key_addr) - 2)
            {
                reboot();
            } else {
                if (new_key != last_key)
                {
                    last_key = new_key;

                    led = !led;
                    *((volatile uint32_t *) BIG_LED_ADDR) = led ? BIG_LED_ON : BIG_LED_OFF;
                    /* _msleep(300); */
                }
            }
        }

	return 0;
}
