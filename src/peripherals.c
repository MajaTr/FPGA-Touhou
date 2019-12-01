#include "peripherals.h"
#include "avalon_addr.h"


int avalon_read(unsigned int address)
{
	volatile int *pointer = (volatile int *) address;
	return pointer[0];
}

void avalon_write(unsigned int address, int data)
{
	volatile int *pointer = (volatile int *) address;
	pointer[0] = data;
}


void vid_set_pixel(int x, int y, int colour)
{
	// derive a pointer to the framebuffer described as 16 bit integers
	volatile short *framebuffer = (volatile short *) (FRAMEBUFFER_BASE);

	// make sure we don't go past the edge of the screen
	if ((x<0) || (x>DISPLAY_WIDTH-1))
		return;
	if ((y<0) || (y>DISPLAY_HEIGHT-1))
		return;

	framebuffer[x+y*DISPLAY_WIDTH] = colour;
}

int next_val(int prev, int val) 
{
	int mnval = 10000, mni = 0;
	for(int i=-2; i<=2; ++i) 
	{
		int d = 256*i+val - prev;
		if(d<0) d = -d;
		if(d<mnval)
		{
			mnval = d;
			mni = i;
		}
	}
	return val+mni*256;
}


void clear_screen(int col) 
{
	volatile short *framebuffer = (volatile short *) (FRAMEBUFFER_BASE);
	for(int i=0; i<DISPLAY_HEIGHT*DISPLAY_WIDTH; ++i) framebuffer[i] = col;
}

void hex_output(int value)
{
	int *hex_leds = (int *) PIO_HEX_BASE;  // define a pointer to the register
	*hex_leds = value;                   // write the value to that address
}



unsigned int get_ms()
{
	return div(get_time(), 50*1000);
}


unsigned int help_buffer[(DISPLAY_WIDTH*DISPLAY_HEIGHT)>>4] = {};


void vid_set_pixel_conditional(int x, int y, int colour, int cond, unsigned int new_help_val)
{

	// make sure we don't go past the edge of the screen
	if ((x<0) || (x>DISPLAY_WIDTH-1))
		return;
	if ((y<0) || (y>DISPLAY_HEIGHT-1))
		return;

	int i = x+y*DISPLAY_WIDTH;
	unsigned int tmp = help_buffer[i>>4];
	unsigned int val = (tmp>>((i&15)<<1))&3;
	
	if(val==1) return;
	if(val==2) cond = 1;

	tmp ^= ((new_help_val^val)<<((i&15)<<1));
	help_buffer[i>>4] = tmp;

	if(!cond) return;

	// derive a pointer to the framebuffer described as 16 bit integers
	volatile short *framebuffer = (volatile short *) (FRAMEBUFFER_BASE);
	
	framebuffer[i] = colour;
}

void clear_help_buffer()
{
	for(int i=0; i<((DISPLAY_WIDTH*DISPLAY_HEIGHT)>>4); ++i) {
		//hex_output(i+1);
		help_buffer[i] = 0;
	}
}


