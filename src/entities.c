#include "peripherals.h"
#include "entities.h"
#include "danmaku.h"


void write_pixel(int x, int y, int col) 
{
	vid_set_pixel(y, x, col);
}

void print_entity(entity *r, int transp) 
{
	r->lookup[0] = global_col;
	for(int px=0; px<r->sx; ++px) for(int py=0; py<r->sy; ++py)
	{
		if(transp && r->sprite[py*r->sx+px]==0) continue;
		write_pixel(r->x+px-r->off_x, r->y+py-r->off_y, r->lookup[r->sprite[py*r->sx+px]]);
	}
}



__attribute__((optimize("-fno-tree-loop-optimize")))
void move_entity1(entity *r, int dx, int dy)
{	
	r->lookup[0] = global_col;
	
	for(int px=0; px<r->sx; ++px) for(int py=0; py<r->sy; ++py)
	{
		if(px-dx<0 || px-dx>=r->sx || py-dy<0 || py-dy>=r->sy)
		{
			write_pixel(r->x+px-r->off_x, r->y+py-r->off_y, r->lookup[0]);
		} 
		else if(r->sprite[(py-dy)*r->sx+px-dx]==0) 
		{
			write_pixel(r->x+px-r->off_x, r->y+py-r->off_y, r->lookup[0]);
		}
	}
}



void move_entity2(entity *r, int dx, int dy)
{	
	r->lookup[0] = global_col;
	
	
	r->x += dx;
	r->y += dy;
	
	print_entity(r, 1);
}

int entity_off_board(entity *r, int dx, int dy)
{
	if(r->x+dx<r->off_x || r->x+dx+r->off_x>BOARD_W) return 1;
	if(r->y+dy<r->off_y || r->y+dy+r->off_y>BOARD_H) return 1;
	return 0;
}


void clear_entity(entity *r) 
{
	for(int px=0; px<r->sx; ++px) for(int py=0; py<r->sy; ++py)
	{
		write_pixel(r->x+px-r->off_x, r->y+py-r->off_y, global_col);
	}
}



int update_bullet1(bullet_entity *b) 
{
	b->x += b->dx;
	b->y += b->dy;
	int ddx = (b->x>>RES)-b->e.x, ddy = (b->y>>RES)-b->e.y;
	if(entity_off_board(&b->e, ddx, ddy))
	{
		clear_entity(&b->e);
		return 0;
	}
	move_entity1(&b->e, ddx, ddy);
	return 1;
}

void update_bullet2(bullet_entity *b) 
{
	int ddx = (b->x>>RES)-b->e.x, ddy = (b->y>>RES)-b->e.y;
	move_entity2(&b->e, ddx, ddy);
}






