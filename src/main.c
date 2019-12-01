#include "peripherals.h"
#include "danmaku.h"

int main(void)
{
	while(1)
	{
		original_loop();
		//youmu_loop();
		while(!(avalon_read(PIO_BUTTONS) & ((1<<16)-(1<<12))));
	}
	return 0;
}
