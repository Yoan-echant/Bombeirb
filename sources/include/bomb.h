#ifndef BOMB_H_
#define BOMB_H_

#include <player.h>
#include <map.h>
#include <monster.h>



struct bomb;

struct bomb* bomb_init();

struct bomb* bomb_place(struct bomb* bomb,int x,int y, struct player* player, struct map* map, int tick_paused, int poseur);


int bomb_get_status(struct bomb* bomb);

void bomb_update(struct bomb* bomb, struct map* map, struct player* player, int tick_paused, struct monster** monsters);
void bomb_explode(struct bomb* bomb,struct map* map, struct player* player);

void explode_reset(struct map* map, struct bomb* bomb, struct player* player, struct monster** monsters);

int get_nb_bombs(struct bomb* bomb);
int bomb_get_level(struct bomb* bomb);


int bomb_get_x(struct bomb* bomb);
int bomb_get_y(struct bomb* bomb);
int bomb_get_range(struct bomb* bomb);
void set_bomb_tab(struct bomb* bomb,int x,int y, int range, int status,int nb_bombes,int creation,struct map* map,int level);
int bomb_get_creation(struct bomb* bomb);

#endif /* BOMB_H_ */
