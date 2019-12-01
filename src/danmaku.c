#include "danmaku.h"
#include "peripherals.h"
#include "asmfunctions.h"
#include "entities.h"


int stick_up()
{
	return avalon_read(PIO_BUTTONS) & (1<<5);
}
int stick_down()
{
	return avalon_read(PIO_BUTTONS) & (1<<6);
}
int stick_left()
{
	return avalon_read(PIO_BUTTONS) & (1<<7);
}
int stick_right()
{
	return avalon_read(PIO_BUTTONS) & (1<<4);
}




#define CYCLES_PER_TICK 1200000
void wait_for_tick()
{
	unsigned int cur = div(get_time(), CYCLES_PER_TICK);
	while(div(get_time(), CYCLES_PER_TICK)==cur);
}



int reimu_sprite[24*12] = {
0, 0, 0, 0, 3, 4, 4, 3, 0, 0, 0, 0,
0, 0, 0, 0, 3, 4, 4, 3, 0, 0, 0, 0,
0, 0, 0, 0, 3, 4, 4, 3, 0, 0, 0, 0,
0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
0, 1, 3, 1, 3, 3, 1, 3, 1, 1, 0, 0,
1, 3, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0,
0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0,
0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0,
3, 3, 3, 1, 3, 3, 3, 3, 1, 3, 3, 3,
3, 3, 3, 1, 3, 3, 2, 3, 1, 3, 3, 3,
0, 3, 3, 1, 3, 2, 2, 2, 1, 3, 3, 0,
0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0,
0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0,
0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0,
0, 0, 1, 1, 2, 2, 2, 2, 1, 1, 0, 0,
0, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 0,
0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
0, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 0,
0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0
};
int reimu_lookup[5] = {0, PIXEL_RED, PIXEL_BLACK, PIXEL24(0xdd, 0xdd, 0xdd), PIXEL24(0x88, 0x88, 0x88)};


int marisa_sprite[32*27] = {
0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 2, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 2, 2, 1, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 2, 2, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 2, 2, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 2, 2, 0, 2, 2, 2, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
0, 0, 2, 2, 0, 2, 2, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
0, 0, 2, 2, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
0, 0, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
0, 0, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 4, 4, 4, 2, 0, 0, 0,
0, 0, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 0, 2, 4, 2, 0, 0,
0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 2, 2, 4, 4, 0, 0,
0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 2, 2, 0, 0, 4, 4, 4,
0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 0, 0, 0, 0, 0, 0,
0, 2, 4, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0,
0, 2, 4, 2, 0, 1, 2, 2, 2, 2, 6, 6, 6, 6, 1, 1, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 4, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 1, 1, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 4, 0, 0, 0, 0, 0, 3, 0, 0, 2, 2, 3, 1, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 4, 4, 0, 0, 0, 0, 0, 3, 3, 0, 2, 2, 3, 1, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
4, 4, 4, 4, 0, 0, 0, 0, 3, 3, 0, 1, 1, 3, 3, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
4, 4, 4, 4, 0, 0, 0, 0, 3, 3, 5, 5, 5, 5, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
4, 4, 4, 4, 0, 0, 0, 2, 3, 5, 5, 5, 5, 5, 5, 3, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0,
4, 4, 4, 4, 0, 0, 0, 2, 3, 5, 1, 5, 5, 1, 5, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
4, 4, 4, 4, 0, 0, 2, 2, 3, 5, 1, 5, 5, 1, 5, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 4, 4, 4, 0, 1, 1, 2, 3, 3, 5, 5, 5, 5, 3, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 4, 4, 0, 0, 0, 1, 1, 2, 3, 3, 3, 3, 3, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 4, 4, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
int marisa_lookup[7] = {0, PIXEL_BLACK, PIXEL_WHITE, PIXEL24(0xff, 0xff, 0), PIXEL24(0x80, 0x40, 0), PIXEL_WHITE, PIXEL_RED};


#define BUL_R 3

int bullet_sprite[7*7] = {
0, 1, 0, 0, 0, 1, 0,
0, 1, 1, 2, 1, 1, 0,
0, 1, 1, 1, 1, 1, 0,
0, 2, 1, 1, 1, 2, 0,
1, 1, 1, 1, 1, 1, 1,
0, 0, 2, 1, 2, 0, 0,
0, 0, 0, 1, 0, 0, 0,
};
int bullet_lookup[3][3] = {
{0, PIXEL24(0x0, 0x0, 0xd0), PIXEL24(0x0, 0x0, 0x80)},
{0, PIXEL24(0xd0, 0x0, 0x0), PIXEL24(0x80, 0x0, 0x0)},
{0, PIXEL24(0xd0, 0xd0, 0x0), PIXEL24(0x80, 0x80, 0x0)}
};




void *memset(void *str, int c, unsigned int n)
{
	for(int i=0; i<n; ++i) *((unsigned char*)str+i) = (unsigned char)c;
	return str;
}

bullet_entity create_bullet(int spawnx, int spawny, int col)
{
	bullet_entity bullet;
	bullet.e.x = spawnx; bullet.e.y = spawny;
	bullet.x = spawnx<<RES; bullet.y = spawny<<RES;
	bullet.e.sprite = bullet_sprite;
	bullet.e.off_x = BUL_R; bullet.e.off_y = BUL_R;
	bullet.e.sx = 2*BUL_R+1; bullet.e.sy = 2*BUL_R+1;
	bullet.e.lookup = bullet_lookup[col];
	bullet.dx = bullet.dy = 0;
	return bullet;
}



typedef struct speed_pool {
	int maxdx, maxdy, offdx, offdy;
	int dx, dy;
	int x_incr, y_incr, x_cf, y_cf;
} speed_pool;

void speed_pool_advance(speed_pool *s)
{
	s->dx = s->dx*s->x_cf+s->x_incr;
	s->dy = s->dy*s->y_cf+s->y_incr;
	s->dx = rem(s->dx, s->maxdx);
	s->dy = rem(s->dy, s->maxdy);
}


int speed_get_dx(speed_pool *s) 
{
	return s->dx+s->offdx;
}

int speed_get_dy(speed_pool *s) 
{
	return s->dy+s->offdy;
}



int collision(int x, int y, entity *e, int r)
{
	int dx = e->x-x, dy = e->y-y;
	if(dx<0) dx = -dx;
	if(dy<0) dy = -dy;
	return dx*dx+dy*dy<=r;
}


typedef struct harmonic_oscillator {
	int x;
	int dx;
	int p;
} harmonic_oscillator;

void harmonic_advance(harmonic_oscillator *h) {
	int neg = (h->x<0);
	if(!neg) h->dx -= div(h->x, h->p);
	else h->dx += div(-h->x, h->p);
	h->x += h->dx;
}


void main_loop()
{
	clear_screen(global_col);
	entity reimu;
	reimu.x = 100; reimu.y = 100;
	reimu.sprite = reimu_sprite;
	reimu.off_x = 6; reimu.off_y = 12;
	reimu.sx = 12; reimu.sy = 24;
	reimu.lookup = reimu_lookup;


	speed_pool s;
	s.maxdx = 4*(1<<RES)+1; s.offdx = -2*(1<<RES);
	s.maxdy = 1*(1<<RES)+1; s.offdy = -4*(1<<RES);
	s.x_incr = (1<<(RES-2))+3; s.y_incr = 3721;
	s.x_cf = 1; s.y_cf = 37;
	
	s.dx = s.dy = 0;
	
	int bullet_col = 0;

	int initx = BOARD_W/2, inity = BOARD_H-50;
	
	harmonic_oscillator hx, hy;
	hx.x = 0; hx.dx = (3<<RES); hx.p = 800;
	hy.x = 0; hy.dx = (1<<RES); hy.p = 120;

	int spawnx = initx, spawny = inity;


	entity marisa;
	marisa.x = spawnx; marisa.y = spawny;
	marisa.sprite = marisa_sprite;
	marisa.off_x = 13; marisa.off_y = 16;
	marisa.sx = 27; marisa.sy = 32;
	marisa.lookup = marisa_lookup;


	int n_bul = 90;
	bullet_entity bullet[n_bul];


	print_entity(&reimu, 1);
	print_entity(&marisa, 1);

	wait_for_tick();

	int cnt = 0;
	int t = 0;
	while(1)
	{
		unsigned int t1 = get_ms();

		harmonic_advance(&hx);
		harmonic_advance(&hy);
		spawnx = initx + (hx.x>>RES);
		spawny = inity + (hy.x>>RES);
		if(cnt<n_bul)
		{
			bullet[cnt] = create_bullet(spawnx, spawny, rem(div(bullet_col++, 10), 3));
			bullet[cnt].dx = speed_get_dx(&s); bullet[cnt].dy = speed_get_dy(&s);
			//bullet[cnt].x += 7*bullet[cnt].dx; bullet[cnt].y += 7*bullet[cnt].dy;
			speed_pool_advance(&s);
			print_entity(&bullet[cnt].e, 1);
			++cnt;
		}
		
		int mdx = spawnx-marisa.x, mdy = spawny-marisa.y;

		

		//clear_screen();
		int rdx=0, rdy=0;
		if(stick_up()) rdx=0, rdy=2;
		if(stick_down()) rdx=0, rdy=-2;
		if(stick_left()) rdx=-2, rdy=0;
		if(stick_right()) rdx=2, rdy=0;

		if(entity_off_board(&reimu, rdx, rdy)) 
		{
			rdx = rdy = 0;
		}

		move_entity1(&marisa, mdx, mdy);
		for(int i=0; i<cnt; ++i) if(!update_bullet1(bullet+i)) {
			bullet[i] = create_bullet(spawnx, spawny, rem(div(bullet_col++, 10), 3));
			
			bullet[i].dx = speed_get_dx(&s); bullet[i].dy = speed_get_dy(&s);
			//bullet[i].x += 7*bullet[i].dx; bullet[i].y += 7*bullet[i].dy;
			speed_pool_advance(&s);
			print_entity(&bullet[i].e, 1);
		}
		move_entity2(&marisa, mdx, mdy);
		for(int i=0; i<cnt; ++i) {
			update_bullet2(bullet+i);
		}
		move_entity1(&reimu, rdx, rdy);
		move_entity2(&reimu, rdx, rdy);
		
		
		for(int i=0; i<cnt; ++i) if(collision(reimu.x, reimu.y, &bullet[i].e, 12))
		{
			return;
		}

		unsigned int t2 = get_ms();
		hex_output(t2-t1);
		wait_for_tick();
	}
	
}

