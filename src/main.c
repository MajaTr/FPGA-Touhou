#include "peripherals.h"
#include "danmaku.h"

int main(void)
{
	while(1)
	{
		main_loop();
		while(!(avalon_read(PIO_BUTTONS) & ((1<<16)-(1<<12))));
	}
	return 0;
}
