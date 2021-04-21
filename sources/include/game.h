/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#ifndef GAME_H_
#define GAME_H_

#include <player.h>
#include <map.h>
#include <bomb.h>
#include <monster.h>
// Abstract data type
struct game;


//Liste contenant les maps
struct maps;

// Create a new game
struct game* game_new();

// Free a game
void game_free(struct game* game);

// Return the player of the current game
struct player* game_get_player(struct game* game);

//Return the current level
int game_get_level(struct game* game);

//Set the level
void game_set_level(struct game* game, int i);
//Return the number of level
int game_get_levels(struct game* game);

// Return the current map
struct map* game_get_current_map(struct game* game);

//Return the asked map
struct map* game_get_map(struct game* game, int i);

// Display the game on the screen
void game_display(struct game* game);

// update
int game_update(struct game* game);

//Return the struct bomb
struct bomb* game_get_bomb(struct game* game);
struct bomb* game_get_bomb_tab(struct game* game, int i);

//Return the boss
struct boss* game_get_boss(struct game* game);

//Return the monster i
struct monster* game_get_monster(struct game* game, int i);

//Return the array of monster
struct monster** game_get_monsters(struct game* game);

void game_set_tick_pause(struct game* game,int i);
int game_get_tick_pause(struct game* game);

int game_get_start(struct game* game);

void game_set_start(struct game* game,int start);
//Fonction permettant de placer une bombe
void game_place_bomb(struct bomb** bombes,int x,int y, struct player* player, struct map* map, int tick_paused,int poseur);

int game_get_menu(struct game* game);

#endif /* GAME_H_ */
