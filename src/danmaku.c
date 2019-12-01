#include "danmaku.h"
#include "peripherals.h"
#include "asmfunctions.h"
#include "entities.h"
#include "data.h"
#include <stddef.h>

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


void original_loop()
{
	//hex_output(1);
	clear_screen(global_col);
	//hex_output(2);
	clear_help_buffer();
	hex_output(3);
	entity reimu;
	reimu.x = BOARD_W/2; reimu.y = 100;
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

	hex_output(4);


	print_entity(&reimu, 1);
	print_entity(&marisa, 1);

	wait_for_tick();

	int cnt = 0;
	int t = 0;
	while(1)
	{
		unsigned int t1 = get_ms();
		clear_help_buffer();

		harmonic_advance(&hx);
		harmonic_advance(&hy);
		spawnx = initx + (hx.x>>RES);
		spawny = inity + (hy.x>>RES);
		if(cnt<n_bul)
		{
			bullet[cnt] = create_star(spawnx, spawny, rem(div(bullet_col++, 10), 3));
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

		move_entity(&marisa, mdx, mdy);
		move_entity(&reimu, rdx, rdy);
		for(int i=0; i<cnt; ++i) if(!update_bullet(bullet+i)) {
			bullet[i] = create_star(spawnx, spawny, rem(div(bullet_col++, 10), 3));
			
			bullet[i].dx = speed_get_dx(&s); bullet[i].dy = speed_get_dy(&s);
			//bullet[i].x += 7*bullet[i].dx; bullet[i].y += 7*bullet[i].dy;
			speed_pool_advance(&s);
			print_entity(&bullet[i].e, 1);
		}
		
		
		for(int i=0; i<cnt; ++i) if(collision(reimu.x, reimu.y, &bullet[i].e, 12))
		{
			return;
		}

		unsigned int t2 = get_ms();
		hex_output(t2-t1);
		wait_for_tick();
	}
	
}

typedef struct rand_pool {
	int maxval;
	int cfa, cfb;
	int curval;
} rand_pool;

int get_rand(rand_pool *r)
{
	r->curval = rem(r->cfa*r->curval+r->cfb, r->maxval);
	return r->curval;
}



typedef struct youmu_bullet_arm
{
	int sx, sy, ex, ey;
	int offx, offy;
	int speed, maxdif;
} youmu_bullet_arm;


int int_sqrt(int a)
{
	int x = 1;
	for(int i=0; i<10; ++i) x = (x+div(a, x))/2;
	return x;
}


int safe_div(int a, int b)
{
	return a>=0 ? div(a, b) : -div(-a, b);
}


youmu_bullet_arm create_bullet_arm(int sx, int sy, int len, int offlen, int dirx, int diry, int speed, int maxdif, int c)
{
	int rtx = int_sqrt(3*(1<<(2*RES-2)));
	int rty = 1<<(RES-1);
	while(c--)
	{
		int ndirx = (dirx*rtx-diry*rty)>>RES;
		int ndiry = (dirx*rty+diry*rtx)>>RES;
		dirx = ndirx; diry = ndiry;
	}
	youmu_bullet_arm arm;
	arm.sx = sx;
	arm.sy = sy;
	arm.ex = sx+((dirx*len)>>RES);
	arm.ey = sy+((diry*len)>>RES);
	arm.offx = (diry*offlen)>>RES;
	arm.offy = -((dirx*offlen)>>RES);
	arm.speed = speed;
	arm.maxdif = maxdif;
	return arm;
}


bullet_entity create_youmu_bullet(youmu_bullet_arm *arm, int t, int side, rand_pool *r, int col)
{
	int rt = t+div(get_rand(r), div(r->maxval, arm->maxdif))-(arm->maxdif/2);
	if(rt<(1<<(RES-3))) rt = (1<<(RES-3));
	if(rt>(1<<RES)-(1<<(RES-3))) rt = (1<<RES)-(1<<(RES-3));
	//hex_output(2);
	int destx = arm->sx+(((arm->ex-arm->sx)*rt)>>RES);
	int desty = arm->sy+(((arm->ey-arm->sy)*rt)>>RES);
	int spawnx = arm->sx+(((arm->ex-arm->sx)*t)>>RES)+side*arm->offx;
	int spawny = arm->sy+(((arm->ey-arm->sy)*t)>>RES)+side*arm->offy;
	int dx = destx-spawnx;
	int dy = desty-spawny;
	//hex_output(3);
	int norm = int_sqrt((dx>>RES)*(dx>>RES)+(dy>>RES)*(dy>>RES))<<RES;
	if(norm<64)
	{
		dx = 0;
		dy = -arm->speed;
	}
	else
	{
		dx = safe_div((dx*arm->speed), norm);
		dy = safe_div((dy*arm->speed), norm);
	}
	return create_small(spawnx, spawny, dx, dy, col);
}


void *memcpy(void *str1, const void *str2, size_t n)
{
	for(int i=0; i<n; ++i) *((unsigned char*)str1+i) = *((unsigned char*)str2+i);
	return str1;
}






void youmu_loop()
{
	clear_screen(global_col);
	//hex_output(2);
	clear_help_buffer();
	
	rand_pool r;
	r.maxval = 7321;
	r.cfa = 1133; r.cfb = 3430;
	r.curval = 0;

	youmu_bullet_arm arms[5];

	 

	int bullet_cnt = 0, maxn = 500;
	int bull_per_arm = 80;

	bullet_entity bullets[maxn];


	entity reimu;
	reimu.x = BOARD_W/2; reimu.y = 70;
	reimu.sprite = reimu_sprite;
	reimu.off_x = 6; reimu.off_y = 12;
	reimu.sx = 12; reimu.sy = 24;
	reimu.lookup = reimu_lookup;

	

	while(1)
	{
		clear_screen(global_col);
		clear_help_buffer();
		print_entity(&reimu, 1);
		for(int i=0; i<5; ++i)
		{
			arms[i] = create_bullet_arm(
				(BOARD_W/2)<<RES, 300<<RES,
				160<<RES, 1<<RES,
				-(1<<RES), 0,
				20*(1<<(RES-3)), 3*(1<<(RES-3)), i+1);
		}
		int frame_count = 0;
		bullet_cnt = 0;
		int bull_per_frame = 5;
		while(bull_per_frame*frame_count<bull_per_arm)
		{
			start_time_delta();
			clear_help_buffer();
		

			if(bull_per_frame*bullet_cnt<bull_per_arm)
			{
				for(int i=0; i<5*bull_per_frame; ++i)
				{
					int side = div(get_rand(&r), div(r.maxval, 4));
					if(side==0) side = -10;
					else if(side==1)  side = -2;
					else if(side==2) side = 2;
					else if(side==3) side = 10;
					if(div(get_rand(&r), div(r.maxval, 15))==0) side = 0;
					bullets[bullet_cnt*5*bull_per_frame+i] = create_youmu_bullet(arms+rem(i, 5), div((bull_per_frame*bullet_cnt)<<(RES), bull_per_arm), side, &r, rem(i, 5));
				}
				//hex_output(7);
				//print_entity(&bullets[bullet_cnt].e, 1);
				bullet_cnt++;
			}
			//hex_output(9);
			for(int i=0; i<5*bullet_cnt*bull_per_frame; ++i)
			{
				update_bullet(bullets+i);
				
			}


			int rdx=0, rdy=0;
			if(stick_up()) rdx=0, rdy=2;
			if(stick_down()) rdx=0, rdy=-2;
			if(stick_left()) rdx=-2, rdy=0;
			if(stick_right()) rdx=2, rdy=0;	
			move_entity(&reimu, rdx, rdy);
			
			
			for(int i=0; i<5*bull_per_frame*bullet_cnt; ++i) if(collision(reimu.x, reimu.y, &bullets[i].e, 6))
			{
				hex_output(0x2136);
				return;
			}
			++frame_count;
			//hex_output(frame_count);
			int t= time_delta();
			hex_output(div(t, 50*1000));
			while(time_delta()<CYCLES_PER_TICK);
		}

		while(frame_count<256)
		{
			start_time_delta();
			clear_help_buffer();

			int rdx=0, rdy=0;
			if(stick_up()) rdx=0, rdy=2;
			if(stick_down()) rdx=0, rdy=-2;
			if(stick_left()) rdx=-2, rdy=0;
			if(stick_right()) rdx=2, rdy=0;	
		
			if(frame_count>=60 && frame_count <=80)
			{
				rdx >>= 1;
				rdy >>= 1;
			}
			move_entity(&reimu, rdx, rdy);
			
			if(frame_count==60)
			{
				for(int i=0; i<5*bull_per_frame*bullet_cnt; ++i)
				{
					bullets[i].dx >>= 1;
					bullets[i].dy >>= 1;
					bullets[i].e.lookup = small_lookup[5];
				}
			}
			if(frame_count==80)
			{
				for(int i=0; i<5*bull_per_frame*bullet_cnt; ++i)
				{
					bullets[i].dx <<= 1;
					bullets[i].dy <<= 1;
					bullets[i].e.lookup = small_lookup[rem(i, 5)];
				}
			}
			for(int i=0; i<5*bull_per_frame*bullet_cnt; ++i)
			{
				update_bullet(bullets+i);
			}

			
			for(int i=0; i<5*bull_per_frame*bullet_cnt; ++i) if(collision(reimu.x, reimu.y, &bullets[i].e, 6))
			{
				hex_output(0x2137);
				return;
			}
			++frame_count;
			//hex_output(frame_count);
			int t= time_delta();
			hex_output(div(t, 50*1000));
			while(time_delta()<CYCLES_PER_TICK);
		}
		
		
	}
	
}









