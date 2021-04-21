/*******************************************************************************
 * Les déplacements du monstre sont fortement inspirés de ceux du player
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#ifndef MONSTER_H_
#define MONSTER_H_

#include <map.h>
#include <constant.h>

struct monster;

// Creates a new monster
struct monster* monster_init(int x, int y, int a);

//Set the state of the monster
void monster_set_actif(struct monster* monster, int i);

//get the state of the monster
int monster_get_actif(struct monster* monster);

// Set the position of the monster
void monster_set_position(struct monster *monster, int x, int y);

// Returns the current position of the monster
int monster_get_x(struct monster* monster);
int monster_get_y(struct monster* monster);

// Set the direction of the next move of the monster
void monster_set_current_way(struct monster* monster, struct map* map);

// Move the monster according to the current direction
int monster_move(struct monster* monster, struct player* player,struct map* map, int level, int levels, int tick_paused);

// Display the monster on the screen
void monster_display(struct monster* monster);

//Kill monster
int kill_monsters(struct monster* monster,struct map* map);

//Add a monster
int create_monster(struct monster** monsters, struct map* map, int x, int y,int level);

//Update a monster
void monster_update(struct monster* monster,struct player* player,struct map* map, int level,int levels, int tick_paused);


#endif /* MONSTER_H_ */
