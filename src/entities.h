#ifndef entities_h
#define entities_h

void write_pixel(int x, int y, int col);


typedef struct entity
{
	int x, y;
	int off_x, off_y;
	int sx, sy;
	int *lookup;
	int *sprite;
} entity;


void print_entity(entity *r, int transp);


void move_entity1(entity *r, int dx, int dy);


void move_entity2(entity *r, int dx, int dy);

int entity_off_board(entity *r, int dx, int dy);


void clear_entity(entity *r);


typedef struct bullet_entity
{
	entity e;
	int x, y;
	int dx, dy;
	
} bullet_entity;


int update_bullet1(bullet_entity *b);

void update_bullet2(bullet_entity *b);


#endif
