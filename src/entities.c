#include "peripherals.h"
#include "entities.h"
#include "danmaku.h"


void write_pixel(int x, int y, int col) 
{
	vid_set_pixel(y, x, col);
}

void write_pixel_conditional(int x, int y, int col, int cond, unsigned int new_help_val) 
{
	vid_set_pixel_conditional(y, x, col, cond, new_help_val);
}

void print_entity(entity *r, int transp) 
{
	r->lookup[0] = global_col;
	for(int px=0; px<r->sx; ++px) for(int py=0; py<r->sy; ++py)
	{
		if(transp && r->sprite[py*r->sx+px]==0) continue;
		int col_id = r->sprite[py*r->sx+px];
		int new_help_val = col_id ? 1 : 2;
		write_pixel_conditional(r->x+px-r->off_x, r->y+py-r->off_y, r->lookup[col_id], 1, new_help_val);
	}
}



__attribute__((optimize("-fno-tree-loop-optimize")))
void move_entity(entity *r, int dx, int dy)
{	
	r->lookup[0] = global_col;
	
	for(int px=0; px<r->sx; ++px) for(int py=0; py<r->sy; ++py)
	{
		if(px-dx<0 || px-dx>=r->sx || py-dy<0 || py-dy>=r->sy)
		{
			write_pixel_conditional(r->x+px-r->off_x, r->y+py-r->off_y, r->lookup[0], 1, 2);
		} 
	}
	
	r->x += dx;
	r->y += dy;
	
	for(int px=0; px<r->sx; ++px) for(int py=0; py<r->sy; ++py)
	{
		int col_id = r->sprite[py*r->sx+px];
		int new_help_val = col_id ? 1 : 2;
		if(px+dx<0 || px+dx>=r->sx || py+dy<0 || py+dy>=r->sy)
		{
			write_pixel_conditional(r->x+px-r->off_x, r->y+py-r->off_y, r->lookup[col_id], 1, new_help_val);
		} 
		else  
		{
			int cond = (r->sprite[(py+dy)*r->sx+px+dx] != col_id);
			write_pixel_conditional(r->x+px-r->off_x, r->y+py-r->off_y, r->lookup[col_id], cond, new_help_val);
		}
	}

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
		write_pixel_conditional(r->x+px-r->off_x, r->y+py-r->off_y, global_col, 1, 2);
	}
}



int update_bullet(bullet_entity *b) 
{
	b->x += b->dx;
	b->y += b->dy;
	int ddx = (b->x>>RES)-b->e.x, ddy = (b->y>>RES)-b->e.y;
	if(entity_off_board(&b->e, ddx, ddy))
	{
		clear_entity(&b->e);
		return 0;
	}
	move_entity(&b->e, ddx, ddy);
	return 1;
}






