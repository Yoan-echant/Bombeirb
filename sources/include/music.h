#ifndef MUSIC_H_
#define MUSIC_H_

#include <SDL/SDL.h>
#include <game.h>



void music_menu_free();
void music_level_free();
// SOUND OF EXPLOSION
void music_get_explosion();

//SOUND OF THE DEATH OF THE PLAYER
void music_get_wasted();

void music_get_win();

void music_play(struct game* game);


void music_unload();

#endif /* MUSIC_H_ */