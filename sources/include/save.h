/*******************************************************************************
 * This file is part of Bombeirb.
 ******************************************************************************/
#ifndef SAVE_H_
#define SAVE_H_

 #include <game.h>
 #include <bomb.h>

//Save the game into save.txt
void save_game(struct game* game);

//load the game from save.txt
void load_game(struct game* game);

#endif /* SAVE_H_ */