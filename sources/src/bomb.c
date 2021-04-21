#include <SDL/SDL_image.h>
#include <assert.h>
#include <unistd.h>

#include <player.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <constant.h>
#include <map.h>
#include <time.h>
#include <music.h>
#include <monster.h>

struct bomb {
    int range;
    int x,y;
    int status;
    int creation;
    int nb_bombes;
    int level;
    int destroyed;
    int poseur; // 1 si le joueur a posé la bombe, 0 sinon
};

struct bomb* bomb_init() {
	struct bomb* bomb = malloc(sizeof(struct bomb));
	if (!bomb)
		error("Memory error");
    bomb->status=0;
    bomb->nb_bombes=0;
    bomb->level=0;
    bomb->x=0;
    bomb->y=0;
    bomb->range=0;

	return bomb;
}

struct bomb* bomb_place(struct bomb* bomb,int x,int y, struct player* player, struct map* map, int tick_paused, int poseur){
    assert(bomb);
    struct bomb* new_bomb=(struct bomb*)malloc(sizeof(struct bomb));
    new_bomb->nb_bombes=bomb->nb_bombes+1;
    new_bomb->x=x;
    new_bomb->y=y;
    new_bomb->status=5;
    new_bomb->destroyed=0;
    new_bomb->level=player_get_level(player);
    new_bomb->range=player_get_bomb_range(player);
    map_set_cell_type(map,new_bomb->x,new_bomb->y,CELL_BOMB | 1); 
    new_bomb->creation=SDL_GetTicks() - tick_paused;
    new_bomb->poseur = poseur;
    return new_bomb;
}

int bomb_get_level(struct bomb* bomb){
    return bomb->level;
}
void set_bomb_tab(struct bomb* bomb,int x,int y, int range, int status,int nb_bombes,int creation,struct map* map,int level){
    bomb->x=x;
    bomb->y=y;
    bomb->range=range;
    bomb->status=status;
    bomb->destroyed=0;
    bomb->nb_bombes=nb_bombes;
    bomb->level=level;
    bomb->creation=creation;
    map_set_cell_type(map,x,y,112|(6-status));
}

int get_nb_bombs(struct bomb* bomb){
    return bomb->nb_bombes;
}

int bomb_get_x(struct bomb* bomb){
    return bomb->x;
}

int bomb_get_y(struct bomb* bomb){
    return bomb->y;
}

int bomb_get_range(struct bomb* bomb){
    return bomb->range;
}

int bomb_get_creation(struct bomb* bomb){
    return bomb->creation;
}

void bomb_explode(struct bomb* bomb,struct map* map,struct player* player){
    map_set_cell_type(map,bomb->x,bomb->y,CELL_EXPLOSION);
    int i=1;
    while (i<=bomb->range && map_is_inside(map,bomb->x,bomb->y+i)==1 && map_get_cell_type(map,bomb->x,bomb->y+i)!=CELL_SCENERY && map_get_cell_type(map,bomb->x,bomb->y+i)!=CELL_DOOR && map_get_cell_type(map,bomb->x,bomb->y+i)!=CELL_KEY && map_get_cell_type(map,bomb->x,bomb->y+i)!=CELL_BOMB){
        if (map_get_cell_type(map,bomb->x,bomb->y+i)==CELL_BOX){
            map_set_cell_type(map,bomb->x,bomb->y+i,CELL_EXPLOSION | get_bonus_type(map,bomb->x,bomb->y+i));
            break;
        }
        else
            map_set_cell_type(map,bomb->x,bomb->y+i,CELL_EXPLOSION);
        i++;        
    }

    i=1;
    while (i<=bomb->range && map_is_inside(map,bomb->x,bomb->y-i)==1 && map_get_cell_type(map,bomb->x,bomb->y-i)!=CELL_SCENERY && map_get_cell_type(map,bomb->x,bomb->y-i)!=CELL_DOOR && map_get_cell_type(map,bomb->x,bomb->y-i)!=CELL_KEY && map_get_cell_type(map,bomb->x,bomb->y-i)!=CELL_BOMB){
        if (map_get_cell_type(map,bomb->x,bomb->y-i)==CELL_BOX){
            map_set_cell_type(map,bomb->x,bomb->y-i,CELL_EXPLOSION | get_bonus_type(map,bomb->x,bomb->y-i));
            break;
        }
        else
            map_set_cell_type(map,bomb->x,bomb->y-i,CELL_EXPLOSION);
        i++;
    }

    i=1;
    while (i<=bomb->range && map_is_inside(map,bomb->x+i,bomb->y)==1 && map_get_cell_type(map,bomb->x+i,bomb->y)!=CELL_SCENERY && map_get_cell_type(map,bomb->x+i,bomb->y)!=CELL_DOOR && map_get_cell_type(map,bomb->x+i,bomb->y)!=CELL_KEY && map_get_cell_type(map,bomb->x+i,bomb->y)!=CELL_BOMB){
        if (map_get_cell_type(map,bomb->x+i,bomb->y)==CELL_BOX){
            map_set_cell_type(map,bomb->x+i,bomb->y,CELL_EXPLOSION | get_bonus_type(map,bomb->x+i,bomb->y));
            break;
        }
        else
            map_set_cell_type(map,bomb->x+i,bomb->y,CELL_EXPLOSION);
        i++;
    }

    i=1;
    while (i<=bomb->range && map_is_inside(map,bomb->x-i,bomb->y)==1 && map_get_cell_type(map,bomb->x-i,bomb->y)!=CELL_SCENERY && map_get_cell_type(map,bomb->x-i,bomb->y)!=CELL_DOOR && map_get_cell_type(map,bomb->x-i,bomb->y)!=CELL_KEY && map_get_cell_type(map,bomb->x-i,bomb->y)!=CELL_BOMB){
        if (map_get_cell_type(map,bomb->x-i,bomb->y)==CELL_BOX){
            map_set_cell_type(map,bomb->x-i,bomb->y,CELL_EXPLOSION | get_bonus_type(map,bomb->x-i,bomb->y));
            break;
        }
        else
            map_set_cell_type(map,bomb->x-i,bomb->y,CELL_EXPLOSION);
        i++;
    }
}

void explode_reset(struct map* map,struct bomb* bomb,struct player* player,struct monster** monsters){
    int i=1;
    while (i<=bomb->range && map_is_inside(map,bomb->x,bomb->y+i)==1 && (map_get_cell_type(map,bomb->x,bomb->y+i)==CELL_EXPLOSION || map_get_cell_type(map,bomb->x,bomb->y+i)==CELL_EMPTY)){
        if(map_get_cell_type(map,bomb->x,bomb->y+i)==CELL_EXPLOSION && get_bonus_type(map,bomb->x,bomb->y+i)!=0){
            if(get_bonus_type(map,bomb->x,bomb->y+i)==5)
               create_monster(monsters, map, bomb->x, bomb->y+i,bomb->level);
            else
                map_set_cell_type(map,bomb->x,bomb->y+i,CELL_BONUS | get_bonus_type(map,bomb->x,bomb->y+i));
        }
        else
            map_set_cell_type(map,bomb->x,bomb->y+i,CELL_EMPTY);
        i++;
    }
    i=1;
    while (i<=bomb->range && map_is_inside(map,bomb->x,bomb->y-i)==1 && (map_get_cell_type(map,bomb->x,bomb->y-i)==CELL_EXPLOSION || map_get_cell_type(map,bomb->x,bomb->y-i)==CELL_EMPTY )){
        if(map_get_cell_type(map,bomb->x,bomb->y-i)==CELL_EXPLOSION && get_bonus_type(map,bomb->x,bomb->y-i)!=0){
            if(get_bonus_type(map,bomb->x,bomb->y-i)==5)
               create_monster(monsters, map, bomb->x, bomb->y-i,bomb->level);
            else
                map_set_cell_type(map,bomb->x,bomb->y-i,CELL_BONUS | get_bonus_type(map,bomb->x,bomb->y-i));
        }
        else
            map_set_cell_type(map,bomb->x,bomb->y-i,CELL_EMPTY);
        i++;
    }
    i=1;
    while (i<=bomb->range && map_is_inside(map,bomb->x+i,bomb->y)==1 && (map_get_cell_type(map,bomb->x+i,bomb->y)==CELL_EXPLOSION || map_get_cell_type(map,bomb->x+i,bomb->y)==CELL_EMPTY)){
        if(map_get_cell_type(map,bomb->x+i,bomb->y)==CELL_EXPLOSION && get_bonus_type(map,bomb->x+i,bomb->y)!=0){
            if(get_bonus_type(map,bomb->x+i,bomb->y)==5)
               create_monster(monsters, map, bomb->x+i, bomb->y,bomb->level);
            else
                map_set_cell_type(map,bomb->x+i,bomb->y,CELL_BONUS | get_bonus_type(map,bomb->x+i,bomb->y));
        }
        else
            map_set_cell_type(map,bomb->x+i,bomb->y,CELL_EMPTY);
        i++;
    }
    i=1;
    while (i<=bomb->range && map_is_inside(map,bomb->x-i,bomb->y)==1 && (map_get_cell_type(map,bomb->x-i,bomb->y)==CELL_EXPLOSION || map_get_cell_type(map,bomb->x-i,bomb->y)==CELL_EMPTY )){
        if(map_get_cell_type(map,bomb->x-i,bomb->y)==CELL_EXPLOSION && get_bonus_type(map,bomb->x-i,bomb->y)!=0){
            if(get_bonus_type(map,bomb->x-i,bomb->y)==5)
               create_monster(monsters, map, bomb->x-i, bomb->y,bomb->level);
            else
                map_set_cell_type(map,bomb->x-i,bomb->y,CELL_BONUS | get_bonus_type(map,bomb->x-i,bomb->y));
        }
        else
            map_set_cell_type(map,bomb->x-i,bomb->y,CELL_EMPTY);
        i++;
    }
    if(map_is_inside(map,bomb->x,bomb->y)==1 && map_get_cell_type(map,bomb->x,bomb->y)==CELL_EXPLOSION){
        map_set_cell_type(map,bomb->x,bomb->y,CELL_EMPTY);
    }
}

int bomb_get_status(struct bomb* bomb){
    return bomb->status;
}

void bomb_update(struct bomb* bomb,struct map* map,struct player* player, int tick_paused, struct monster** monsters){
    int current_time=SDL_GetTicks() - tick_paused;
    int diff= current_time - bomb->creation;
    
    if(diff<1000){
        bomb->status=5;
        map_set_cell_type(map,bomb->x,bomb->y,CELL_BOMB | 1);
    }
    else if (diff>1000 && diff<2000){
        bomb->status=4;
        map_set_cell_type(map,bomb->x,bomb->y,CELL_BOMB | 2);
    }
    else if (diff>2000 && diff<3000){
        bomb->status=3;
        map_set_cell_type(map,bomb->x,bomb->y,CELL_BOMB | 3);
    }
    else if (diff>3000 && diff<4000){
        bomb->status=2;  
        map_set_cell_type(map,bomb->x,bomb->y,CELL_BOMB | 4);  
    } 
    else if (diff>4000 && diff<5000){
        bomb->status=1; 
        if (bomb->destroyed==0){
            bomb_explode(bomb,map,player);    
            bomb->destroyed=1;
        }   
    }
    else if(diff >5000){
        bomb->status=0;
        if (bomb->poseur && player_get_nb_bomb(player)<9)
            player_inc_nb_bomb(player);
        explode_reset(map,bomb,player,monsters);
        return;
    }   
}







