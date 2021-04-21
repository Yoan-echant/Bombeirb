/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL_image.h>
#include <assert.h>

#include <player.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <constant.h>
#include <monster.h>
#include <bomb.h>
#include <map.h>
#include <music.h>

struct player {
	int x, y;
	enum direction direction;
	int nb_bombs;
	int invulnerabilite;  //0 = vulnérable, 1 = invulnérable
	int lives;
	int invulnerabilite_tempo;
	int bomb_range;
	int player_key;
	int level;
	int win;
};

struct player* player_init(int bombs) {
	struct player* player = malloc(sizeof(*player));
	if (!player)
		error("Memory error");

	player->direction = NORTH;
	player->nb_bombs = bombs;
	player->bomb_range=1;
	player->invulnerabilite = 0;
	player->invulnerabilite_tempo=0;
	player->lives = 3;
	player->player_key=0;
	player->level=0;
	player->win=0;
	return player;
}

void player_update(struct player* player, struct map* map, int tick_paused) {
	assert(player);
	int x = player->x;
	int y = player->y;
	map_get_cell_type(map,x,y);
	switch (map_get_cell_type(map,x,y)) {
		case CELL_MONSTER:
			player_add_lives(player, -1, tick_paused);
			break;
		case CELL_EXPLOSION:
			player_add_lives(player, -1, tick_paused);
			break;
		default:
			break;
	}
}
void player_set_position(struct player *player, int x, int y, int level) {
	assert(player);
	player->x = x;
	player->y = y;
	player->level=level;
}

void player_set_bombs(struct player* player, int i){
    if (i>0 && i<10)
		player->nb_bombs = i;
}


void player_free(struct player* player) {
	assert(player);
	free(player);
}
void player_set_level(struct player* player,int level){
	player->level=level;
}

int player_get_level(struct player* player){
	return player->level;
}

int player_get_key(struct player* player){
	return player->player_key;
}

void player_set_key(struct player* player, int i){
	if (i>=0 && i<10)
		player->player_key=i;
}

int player_get_lives(struct player* player) {
	return player->lives;
}

void player_add_lives(struct player* player, int i, int tick_paused) {
		if ((i < 0) && (player_get_lives(player)+i>0)){
			player->lives+=i;
			player_set_invulnerabilite(player, 1, tick_paused);
		}
		else if ((player_get_lives(player) + i == 0) && (i < 0)) {
			player->lives+=i;
			music_get_wasted();
		}

	if (i > 0 && player->lives + i < 10){
		player->lives+=i;
	}
}

void player_set_lives(struct player* player, int i){
	player->lives = i;
}
int get_invul_tempo(struct player* player) {
	return player->invulnerabilite_tempo;
}

int player_get_x(struct player* player) {
	assert(player != NULL);
	return player->x;
}

int player_get_y(struct player* player) {
	assert(player != NULL);
	return player->y;
}
int player_has_win(struct player* player){
	return player->win;
}

void player_set_win(struct player* player, int win){
	player->win=win;
}

void player_set_current_way(struct player* player, enum direction way) {
	assert(player);
	player->direction = way;
}

int player_get_nb_bomb(struct player* player) {
	assert(player);
	return player->nb_bombs;
}

enum direction get_player_direction(struct player* player){
	return player->direction;
}

void player_inc_nb_bomb(struct player* player) {
	assert(player);
	player->nb_bombs += 1;
}

void player_dec_nb_bomb(struct player* player) {
	assert(player);
	player->nb_bombs -= 1;
}

int player_get_bomb_range(struct player* player){
	return player->bomb_range;
}

void player_set_bomb_range(struct player* player, int range){
	if (0 < range && range < 10){
		player->bomb_range=range;
	}
	else{
		printf("Cette valeur de bomb_range n'est pas valide");
	}
}

void player_inc_bomb_range(struct player* player){
	if (player->bomb_range < 9)
		player->bomb_range+=1;
}

void player_dec_bomb_range(struct player* player){
	if (player->bomb_range>1)
		player->bomb_range-=1;
}

int player_get_invulnerabilite(struct player* player) {
	return player->invulnerabilite;
}

void player_set_invulnerabilite(struct player* player, int i, int tick_paused){
	player->invulnerabilite = i;
		player->invulnerabilite_tempo=SDL_GetTicks()- tick_paused;
}

static int player_move_aux(struct player* player, struct map* map, int x, int y) {
	int xarrive = 2*x-player->x;
	int yarrive = 2*y-player->y;	// Coordonnées où une caisse serait poussée
	if (!map_is_inside(map, x, y))
		return 0;

	switch (map_get_cell_type(map, x, y)) {
	case CELL_SCENERY:
		if((get_bonus_type(map,x,y)&0x0f)==SCENERY_PRINCESS){
			player->win=1;
			music_get_win();
		}
		else{
			//printf("scenery\n");
			return 0;
		}
		break;
	
	case CELL_BOMB:
		//printf("bombe\n");
		return 0;
		break;

	case CELL_BOX:
		if (map_is_inside(map,xarrive,yarrive)) {
			int box_type=get_bonus_type(map,x,y);
			if (map_get_cell_type(map, xarrive, yarrive) == CELL_EMPTY) {
				map_set_cell_type(map,x,y, CELL_EMPTY);
				map_set_cell_type(map,xarrive,yarrive,CELL_BOX | box_type);
				return 1;
			}
		}
		//printf("box\n");
		return 0;
		break;

	case CELL_BONUS:
		switch(get_bonus_type(map,x,y)&0x0f ){
			case BONUS_BOMB_RANGE_DEC:
				if (player_get_bomb_range(player)>1)
					player_dec_bomb_range(player);
				map_set_cell_type(map,x,y,CELL_EMPTY);
				break;
			case BONUS_BOMB_RANGE_INC:
				player_inc_bomb_range(player);
				map_set_cell_type(map,x,y,CELL_EMPTY);
				break;
			case BONUS_BOMB_NB_DEC:
				if (player_get_nb_bomb(player)>1)
					player_dec_nb_bomb(player);
				map_set_cell_type(map,x,y,CELL_EMPTY);
				return 1;
				break;
			case BONUS_BOMB_NB_INC:
				if (player_get_nb_bomb(player)<9)
					player_inc_nb_bomb(player);
				map_set_cell_type(map,x,y,CELL_EMPTY);
				return 1;
				break;
			case BONUS_LIFE:
				player_add_lives(player,1,0);
				map_set_cell_type(map,x,y,CELL_EMPTY);
				return 1;
				break;
			default:
				return 1;
				break;
		}
		break;

	case CELL_MONSTER:
		return 1;
		break;
	case CELL_DOOR:
		if ((get_bonus_type(map,x,y) & 0x01)==1){
			player_set_level(player,(get_bonus_type(map,x,y) & 0x0e)/2);
		}
		else if (player_get_key(player)){
			player_set_level(player,(get_bonus_type(map,x,y) & 0x0e)/2);
			map_set_cell_type(map,x,y,CELL_DOOR | (get_bonus_type(map,x,y) & 0x0e) | 1);
			player_set_key(player, player_get_key(player)-1);
		}
		break;
	case CELL_KEY :
		player_set_key(player, player_get_key(player)+1);
		map_set_cell_type(map,x,y,CELL_EMPTY);
		return 1;
		break;
	default:
		return 1;
		break;
	}

	// Player has moved
	return 1;
}

int player_move(struct player* player, struct map* map) {
	int x = player->x;
	int y = player->y;
	int move = 0;

	switch (player->direction) {
	case NORTH:
		if (player_move_aux(player, map, x, y - 1)) {
			player->y--;
			move = 1;
		}
		break;

	case SOUTH:
		if (player_move_aux(player, map, x, y + 1)) {
			player->y++;
			move = 1;
		}
		break;

	case WEST:
		if (player_move_aux(player, map, x - 1, y)) {
			player->x--;
			move = 1;
		}
		break;

	case EAST:
		if (player_move_aux(player, map, x + 1, y)) {
			player->x++;
			move = 1;
		}
		break;
	}

	if (move && map_get_cell_type(map,x,y)==CELL_BONUS) {
		map_set_cell_type(map, x, y, CELL_EMPTY);
	}
	return move;
}

void player_display(struct player* player) {
	assert(player);
	window_display_image(sprite_get_player(player->direction),
			player->x * SIZE_BLOC, player->y * SIZE_BLOC);
}