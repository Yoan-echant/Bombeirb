/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL_image.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <map.h>
#include <constant.h>
#include <misc.h>
#include <sprite.h>
#include <window.h>
#include <bomb.h>
#include <game.h>

struct map {
	int width;
	int height;
	unsigned char* grid;
	int nb_monster;
	int* monster_init_x;
	int* monster_init_y;
	int key;
};

int get_nb_monster(struct map* map) {
	int nb = map->nb_monster;
	return nb;
}


int get_monster_init_x(struct map* map, int i){
	int x = map->monster_init_x[i];
	return x;
}

int get_monster_init_y(struct map* map, int i){
	int y = map->monster_init_y[i];
	return y;
}

void set_nb_monster(struct map* map, int x){
	map->nb_monster = x;
}

#define CELL(i,j) ( (i) + (j) * map->width)

struct map* map_new(int width, int height)
{
	assert(width > 0 && height > 0);

	struct map* map = malloc(sizeof (*map)*10);
	if (map == NULL )
		error("map_new : malloc map failed");

	map->width = width;
	map->height = height;
	
	map->grid = malloc(height * width);
	
	
	if (map->grid == NULL) {
		error("map_new : malloc grid failed");
	}

	// Grid cleaning
	int i, j;
	for (i = 0; i < width; i++)
	  for (j = 0; j < height; j++)
	    map->grid[CELL(i,j)] = CELL_EMPTY;

	//Monster
	map->nb_monster = 0;
	
	return map;
}

int map_is_inside(struct map* map, int x, int y)
{
	assert(map);
	if ((0 <= x) && (map->width >= x+1)) {
		if ((0 <= y) && (map->height >= y+1)) {
			return 1;
		}
	}
	return 0;
}

void map_free(struct map *map)
{
	if (map == NULL )
		return;
	free(map->grid);
	free(map);
}

int map_get_width(struct map* map)
{
	assert(map != NULL);
	return map->width;
}

int map_get_height(struct map* map)
{
	assert(map);
	return map->height;
}

enum cell_type get_bonus_type(struct map* map, int x,int y){
	return map->grid[CELL(x,y)] & 0x0f;
}


enum cell_type map_get_cell_type(struct map* map, int x, int y)
{
	assert(map && map_is_inside(map, x, y));
	return map->grid[CELL(x,y)] & 0xf0;
}

void map_set_cell_type(struct map* map, int x, int y, enum cell_type type)
{
	assert(map && map_is_inside(map, x, y));
	map->grid[CELL(x,y)] = type;
}

void display_bonus(struct map* map, int x, int y, unsigned char type)
{
	// bonus is encoded with the 4 most significant bits
	switch (type & 0x0f) {
	case BONUS_BOMB_RANGE_INC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_INC), x, y);
		break;

	case BONUS_BOMB_RANGE_DEC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_DEC), x, y);
		break;

	case BONUS_BOMB_NB_DEC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_NB_DEC), x, y);
		break;

	case BONUS_BOMB_NB_INC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_NB_INC), x, y);
		break;
	case BONUS_LIFE :
		window_display_image(sprite_get_banner_life(), x, y);
		break;
	}
	
}

void display_scenery(struct map* map, int x, int  y, unsigned char type)
{
	switch (type & 0x0f) { // sub-types are encoded with the 4 less significant bits
	case SCENERY_STONE:
		window_display_image(sprite_get_stone(), x, y);
		break;

	case SCENERY_TREE:
		window_display_image(sprite_get_tree(), x, y);
		break;
	case SCENERY_PRINCESS:
		window_display_image(sprite_get_princess(),x,y);
		break;
	case SCENERY_BOMB_BOX:
		window_display_image(sprite_get_bombox(),x,y);
		break;
	}
}
void display_bomb(struct map* map,int x, int y,unsigned char type){
	switch (type & 0x0f){
		case 1:
			window_display_image(sprite_get_bomb(3),x,y);
			break;
		case 2:
			window_display_image(sprite_get_bomb(2),x,y);
			break;
		case 3:
			window_display_image(sprite_get_bomb(1),x,y);
			break;
		case 4:
			window_display_image(sprite_get_bomb(0),x,y);
			break;
	}
}

void map_display(struct map* map)
{
	assert(map != NULL);
	assert(map->height > 0 && map->width > 0);

	int x, y;
	for (int i = 0; i < map->width; i++) {
	  for (int j = 0; j < map->height; j++) {
	    x = i * SIZE_BLOC;
	    y = j * SIZE_BLOC;

	    unsigned char type = map->grid[CELL(i,j)];
	    switch (type & 0xf0) {
			case CELL_SCENERY:
		  	display_scenery(map, x, y, type);
		  	break;
	  	case CELL_BOX:
					window_display_image(sprite_get_box(), x, y);
	      	break;
	    case CELL_BONUS:
	      display_bonus(map, x, y, type);
	      break;
	    case CELL_KEY:
	      window_display_image(sprite_get_key(), x, y);
	      break;
				case CELL_BOMB:
					display_bomb(map,x,y,type);
					break;
	    	case CELL_DOOR:
					switch (get_bonus_type(map,i,j)&0x01){
					case 0 :
						window_display_image(sprite_get_door_closed(), x, y);
	      		break;
					case 1:
						window_display_image(sprite_get_door_opened(), x, y);
						break;
					}
	      	break;
				case CELL_EXPLOSION:
					switch (get_bonus_type(map,i,j)&0x0f){
					case 0:
						window_display_image(sprite_get_explosion(),x,y);
						break;
					case BONUS_BOMB_RANGE_INC:
						window_display_image(sprite_get_explosion(),x,y);
						window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_INC), x, y);
						break;

					case BONUS_BOMB_RANGE_DEC:
						window_display_image(sprite_get_explosion(),x,y);
						window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_DEC), x, y);
						break;

					case BONUS_BOMB_NB_DEC:
						window_display_image(sprite_get_explosion(),x,y);
						window_display_image(sprite_get_bonus(BONUS_BOMB_NB_DEC), x, y);
						break;

					case BONUS_BOMB_NB_INC:
						window_display_image(sprite_get_explosion(),x,y);
						window_display_image(sprite_get_bonus(BONUS_BOMB_NB_INC), x, y);
						break;
					case BONUS_LIFE :
						window_display_image(sprite_get_explosion(),x,y);
						window_display_image(sprite_get_banner_life(), x, y);
						break;
					case BONUS_MONSTER :
						window_display_image(sprite_get_explosion(),x,y);
						window_display_image(sprite_get_monster(2), x, y);
						break;
					}
			
				break;
	   	 }
	  }
	}
}


/*
struct map* map_get_static(void)
{
	struct map* map = map_new(STATIC_MAP_WIDTH, STATIC_MAP_HEIGHT);

	unsigned char themap[STATIC_MAP_WIDTH * STATIC_MAP_HEIGHT] = {
	  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
	  CELL_STONE, CELL_STONE, CELL_STONE, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_BOX_BOMBDEC, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
	  CELL_BOX_BOMBINC, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_BOX_RANGEINC, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_BOX_BOMBDEC, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_KEY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_STONE, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY , CELL_EMPTY, CELL_EMPTY, CELL_STONE,  CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_TREE, CELL_BOX_RANGEINC, CELL_TREE, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,  CELL_EMPTY, CELL_STONE,  CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_TREE, CELL_TREE, CELL_TREE, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,  CELL_STONE,  CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE,  CELL_EMPTY, CELL_EMPTY,
	  CELL_BOX_RANGEDEC, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE,  CELL_BOX_LIFE, CELL_EMPTY,
	  CELL_BOX_RANGEINC,  CELL_EMPTY, CELL_DOOR , CELL_EMPTY , CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
		};

	for (int i = 0; i < STATIC_MAP_WIDTH * STATIC_MAP_HEIGHT; i++){
		map->grid[i] = themap[i];
	}

	return map;
}
*/

struct map* map_create_level(int level,char *repository){
	FILE * file;
	char map_level[256]="map/";
	int i=0;
	while(repository[i]!='\0'){
		i++;
	}
	for (int j=4;j<4+i;j++){
		map_level[j]=repository[j-4];
	}
	map_level[4+i]='_';
	map_level[5+i]=level+'0';
	file=fopen(map_level,"r");
	int width;
	int height;
	fscanf(file,"%i:%i",&width,&height);
	struct map* map=map_new(width,height);
	int cell_value;
	map->nb_monster = 0;
	map->monster_init_x = malloc(NB_MONSTER_MAX*sizeof(int));
	map->monster_init_y = malloc(NB_MONSTER_MAX*sizeof(int));
	for (int i=0;i<height;i++){  //les y
		for (int j=0;j<width;j++){ // les x
			fscanf(file,"%i",&cell_value);
			map->grid[CELL(j,i)] = cell_value;
			if (cell_value == 96){
				map->monster_init_x[map->nb_monster] = j;
				map->monster_init_y[map->nb_monster] = i;
				map->nb_monster++;
			}
		}
	}

	fclose(file);
	return map;
}

