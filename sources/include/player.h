/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#ifndef PLAYER_H_
#define PLAYER_H_

#include <map.h>
#include <constant.h>

struct player;

// Creates a new player with a given number of available bombs
struct player* player_init(int bomb_number);
void   player_free(struct player* player);

// Set the position of the player
void player_set_position(struct player *player, int x, int y,int level);

//return the lives of the player
int player_get_lives(struct player* player);

//set the lives of the player to x + i

void player_add_lives(struct player* player, int i, int tick_paused);

//Set the lives of the player to i
void player_set_lives(struct player* player, int i);

//Return the time at which the invulnerabilite was set
int get_invul_tempo(struct player* player);

// Returns the current position of the player
int player_get_x(struct player* player);
int player_get_y(struct player* player);

//return the state of the player
int player_get_invulnerabilite(struct player* player);

//Change the state of the player
void player_set_invulnerabilite(struct player* player, int i, int tick_paused);

// Set the direction of the next move of the player
void player_set_current_way(struct player * player, enum direction direction);

// Set, Increase, Decrease the number of bomb that player can put
int  player_get_nb_bomb(struct player * player);
void player_inc_nb_bomb(struct player * player);
void player_dec_nb_bomb(struct player * player);
//Set, Increase, Decrease the number of range the bombs have
int player_get_bomb_range(struct player* player);
void player_inc_bomb_range(struct player* player);
void player_dec_bomb_range(struct player* player);
void player_set_bomb_range(struct player* player, int range);

// Move the player according to the current direction
int player_move(struct player* player, struct map* map);

// Display the player on the screen
void player_display(struct player* player);

enum direction get_player_direction(struct player* player);

//Update the statut of the player
void player_update(struct player* player, struct map* map, int tick_paused);

void player_set_level(struct player* player,int level);

int player_get_level(struct player* player);

//Return the number of keys of the player
int player_get_key(struct player* player);

//add a key to the player
void player_set_key(struct player* player, int i);

//Set to i the number of bomb of the player
void player_set_bombs(struct player* player, int i);

int player_has_win(struct player* player);
void player_set_win(struct player* player, int win);

#endif /* PLAYER_H_ */
