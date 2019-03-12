#include "DongleCommon.h"

void reboot()
{
	wdt_enable(WDTO_250MS);
    for (;;);
}