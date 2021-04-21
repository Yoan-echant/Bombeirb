#ifndef BOSS_H_
#define BOSS_H_

#include <map.h>
#include <player.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <constant.h>
#include <monster.h>
#include <bomb.h>

//On définit la structure du boss
struct boss;

//Structure contenant les coord ou le boss lance les bombes
struct bomb_thrown;

//Initialisation du boss
struct boss* boss_init();

//Tell if the boss is alive or not
int boss_is_alive(struct boss* boss);

//Set the boss alive or dead
void boss_set_actif(struct boss* boss, int val);

// Tell if the boss can launch a bomb
int boss_can_launch_bomb(struct boss* boss);
void boss_set_launch_bomb(struct boss* boss, int val);

// Return the boss timers
int boss_get_tempo_launch(struct boss* boss);
int boss_get_tempo_monstre(struct boss* boss);
int boss_get_tempo_move(struct boss* boss);
int boss_get_bomb_last_update(struct bomb_thrown* bomb_thrown);

void boss_set_tempo_launch(struct boss* boss, int val);
void boss_set_tempo_monstre(struct boss* boss, int val);
void boss_set_tempo_move(struct boss* boss, int val);
void boss_set_bomb_last_update(struct bomb_thrown* bomb_thrown, int val);
//Kill the boss
void boss_kill(struct boss* boss, struct map* map);

//Tell wether the boss is launching bomb
int boss_get_launched_bomb(struct boss* boss);
void boss_set_launched_bomb(struct boss* boss, int val);

//Return the number of lives the boss have 
int boss_get_lives(struct boss* boss);

//Augmente le nombre de vie du boss de i et le rend invincible si i<0. teste si il faut changer de phase
void boss_add_lives(struct boss* boss,struct map* map, int i, int tickpaused);

//Set the number of live to val and update the phase accordingly
void boss_set_lives(struct boss* boss,int val);

//Return the current phase of the boss
int boss_get_phase(struct boss* boss);

//Get the time the boss invu was started
int boss_get_invul_tempo(struct boss* boss);
void boss_set_invul_tempo(struct boss* boss, int val);

int boss_get_invu(struct boss* boss);
void boss_set_invu(struct boss* boss, int val);

//Change the boss phase to i and execute the animation
void boss_change_phase(struct boss* boss, int i);

//Return the sruct with the bomb lauched by the boss
struct bomb_thrown* boss_get_bombs(struct boss*boss);

//Set the coord where bomb will be launched
void boss_throw_bomb(struct boss* boss,struct player* player, struct map* map,struct bomb_thrown* bomb_thrown,int nb_bombes, int tickpaused);

//return the x of the iéme bombe and set corrd
int boss_get_bomb_x(struct bomb_thrown* bomb_thrown, int i);
void boss_set_bomb_x(struct bomb_thrown* bomb_thrown, int i, int val);
//return the y of the iéme bombe and set coord
int boss_get_bomb_y(struct bomb_thrown* bomb_thrown, int i);
void boss_set_bomb_y(struct bomb_thrown* bomb_thrown, int i, int val);

//Get the airborn coord of the thrown bomb 
int boss_get_current_bomb_x(struct bomb_thrown* bomb_thrown, int i);
int boss_get_current_bomb_y(struct bomb_thrown* bomb_thrown, int i);

//Set the airborn coord of the thrown bomb 
void boss_set_current_bomb_x(struct bomb_thrown* bomb_thrown, int i, int value);
void boss_set_current_bomb_y(struct bomb_thrown* bomb_thrown, int i, int value);

//Get the coord of the boss
int boss_get_x(struct boss* boss);
int boss_get_y(struct boss* boss);

//Set the coord of the boss
void boss_set_x(struct boss* boss, int val);
void boss_set_y(struct boss* boss, int val);

//get the orientation of the boss
int boss_get_direction(struct boss* boss);
void boss_set_direction(struct boss* boss, int val);

//Update the boss
void boss_update(struct monster** monsters,struct bomb** bombes,struct boss* boss, struct player* player,struct map* map,int level, int tickpaused);

//Display the boss
void boss_display(struct boss* boss);

//Display the bombs thrown
void boss_display_bomb_thrown(struct boss* boss, struct map* map);

#endif /* BOSS_H_ */