/*******************************************************************************
 * Les déplacements du monstre sont fortement inspirés de ceux du player
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL/SDL_image.h>
#include <assert.h>
#include <math.h>

#include <player.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <constant.h>
#include <monster.h>
#include <dijkstra.h>

struct monster {
	int x, y;
	enum direction direction;
	int actif; //1= le monstre est actif
	int* Path; //Chemin que suivra le monstre
	int l_path;
	int nb_path; //Indice permmettant de parcourir le chemin 
	int move_tempo;
};

struct monster* monster_init(int x, int y, int a) {
	struct monster* monster = (struct monster*) malloc(sizeof(struct monster));
	if (!monster)
		error("Memory error");

	monster->direction = NORTH;
	monster_set_position(monster, x, y);
	if (a==1) {
		monster->actif = 1;
	}
	else if (a==0)
		monster->actif = 0;
	else
		error("état du monstre invalide");
	monster->nb_path =0;
	monster->Path=malloc(STATIC_MAP_WIDTH*STATIC_MAP_HEIGHT*sizeof(int));
	for (int k=0;k<STATIC_MAP_WIDTH*STATIC_MAP_HEIGHT;k++){
		monster->Path[k]=0;
	}
	monster->move_tempo = 0;
	monster->l_path = 0;
	return monster;
}


void monster_set_position(struct monster *monster, int x, int y) {
	assert(monster);
	monster->x = x;
	monster->y = y;
}

float dist_detection(int level, int levels){
	if (level==levels){
		return STATIC_MAP_WIDTH+ STATIC_MAP_HEIGHT;
	}
	return (DIST_DETECTION(level));
}
void monster_free(struct monster* monster) {
	assert(monster);
	free(monster);
}

int monster_get_actif(struct monster* monster) {
	assert(monster != NULL);
	return monster->actif;
}

void monster_set_actif(struct monster* monster, int i) {
	assert(monster != NULL);
	monster->actif = i;
}
int monster_get_x(struct monster* monster) {
	assert(monster != NULL);
	return monster->x;
}

int monster_get_y(struct monster* monster) {
	assert(monster != NULL);
	return monster->y;
}

int monster_will_move(int level) {
	int t_mvmt = delay_monstre(level)/30;
	if (rand() % t_mvmt == 1)
        return 1;

    return 0;
}



static int monster_move_aux(struct monster* monster, struct map* map, int x, int y) {
	if (!map_is_inside(map, x, y)) {
		return 0;
    }
	if (map_is_inside(map, x+1, y)) {
		if (map_get_cell_type(map, x+1, y)==CELL_DOOR) {
    		return 0;
		}
    }
	if (map_is_inside(map, x-1, y)) {
		if (map_get_cell_type(map, x-1, y)==CELL_DOOR) {
    		return 0;
		}
    }
	if (map_is_inside(map, x, y+1)) {
		if (map_get_cell_type(map, x, y+1)==CELL_DOOR) {
    		return 0;
		}
    }
	if (map_is_inside(map, x, y-1)) {
		if (map_get_cell_type(map, x, y-1)==CELL_DOOR) {
    		return 0;
		}
    }
	switch (map_get_cell_type(map, x, y)) {
	case CELL_SCENERY:
		return 0;
		break;

	case CELL_BOX:
		return 0;
		break;

	case CELL_BONUS:
		return 0;
		break;
	case CELL_KEY:
		return 0;
		break;
	case CELL_BOMB:
		return 0;
		break;

	case CELL_MONSTER:
        return 0;
		break;

	case CELL_EXPLOSION:
		return 0;
		break;
	default:
		break;
		}
	
	// Monster has moved
	return 1;
}


void monster_set_current_way_rand(struct monster* monster, struct map* map) {
	assert(monster);
	int x = monster->x;
	int y = monster->y;
    
	int nbdir = 0;
	int dir[4]={5,5,5,5};		//Je crée un tableau de taille 0-4, contenant les directions possible pour le monstre
		
		
	if (monster_move_aux(monster, map, x+1, y)) {
		dir[nbdir] = 2;
		nbdir++;
	}
		
	if (monster_move_aux(monster, map, x-1, y)) {
		dir[nbdir] = 3;
		nbdir++;
	}
		
	if (monster_move_aux(monster, map, x, y+1)) {
		dir[nbdir] = 1;
		nbdir++;
	}
		
	if (monster_move_aux(monster, map, x, y-1)) {
		dir[nbdir] = 0;
		nbdir++;
	}
		
	if (nbdir != 0) {
        switch(rand() % nbdir) {
            case 0:
				if (dir[0]==0) {
                	monster->direction = NORTH;
				}
				if (dir[0]==1) {
                	monster->direction = SOUTH;
				}
				if (dir[0]==2) {
                	monster->direction = EAST;
				}
				if (dir[0]==3) {
                	monster->direction = WEST;
				}
                break;
            case 1:
                if (dir[1]==0) {
            		monster->direction = NORTH;
				}
				if (dir[1]==1) {
            		monster->direction = SOUTH;
				}
				if (dir[1]==2) {
            		monster->direction = EAST;
				}
				if (dir[1]==3) {
                	monster->direction = WEST;
				}
                break;
            case 2:
            	if (dir[2]==0) {
            		monster->direction = NORTH;
				}
				if (dir[2]==1) {
            		monster->direction = SOUTH;
				}
				if (dir[2]==2) {
            		monster->direction = EAST;
				}
				if (dir[2]==3) {
            		monster->direction = WEST;
				}
            	break;
        	case 3:
            	if (dir[3]==0) {
               		monster->direction = NORTH;
				}
				if (dir[3]==1) {
                	monster->direction = SOUTH;
				}
				if (dir[3]==2) {
                	monster->direction = EAST;
				}
				if (dir[3]==3) {
            		monster->direction = WEST;
				}
            	break;
			

		}
    }
	else {
		switch(rand() % 4) {
            case 0:
				monster->direction = NORTH;
				break;
			case 1:
				monster->direction = SOUTH;
				break;
			case 2:
				monster->direction = EAST;
				break;
			case 3:
				monster->direction = WEST;
				break;

		}
	}
	    
    

}

	


int kill_monsters(struct monster* monster,struct map* map) {
	int x = monster->x;
	int y = monster->y;
	if (map_get_cell_type(map, x, y)==CELL_EXPLOSION) {
		monster->actif = 0;
		return 0;
	}
	return 1;
}

int create_monster(struct monster** monsters, struct map* map, int x, int y,int level){
	int nbmonster = get_nb_monster(map);
	int nbmonster_m = NB_MONSTER_MAX;
	if (nbmonster < nbmonster_m){
		set_nb_monster(map, nbmonster +1);
		for (int i = 0+level*NB_MONSTER_MAX; i < nbmonster_m*(level+1); i++){
			if (monsters[i]->actif == 0){
				monsters[i]->actif = 1;
				monsters[i]->x = x;
				monsters[i]->y = y;
				monsters[i]->l_path=0;
				monsters[i]->nb_path=0;
				monsters[i]->Path=NULL;
				monsters[i]->Path = malloc(STATIC_MAP_HEIGHT*STATIC_MAP_WIDTH*sizeof(int));
				map_set_cell_type(map,x,y,CELL_MONSTER);
				return 1;
			}
		}
	}
	else {
		printf("Il y a déja trop de monstre sur la map\n");
	}
	return 0;
}

void monster_update(struct monster* monster,struct player* player, struct map* map, int level,int levels, int tick_paused){
	if (monster->actif==1){
		monster_move(monster, player, map, level,levels,tick_paused);
		kill_monsters(monster, map);
	}
}

int monster_move(struct monster* monster,struct player* player, struct map* map, int level,int levels, int tick_paused) {
	int x = monster->x;
	int y = monster->y;
	int xp =player_get_x(player);
	int yp =player_get_y(player);
	int current_time = SDL_GetTicks() - tick_paused;
	int delay = delay_monstre(level);
	int move = 0;
	float dist = sqrt(square(xp-x)+square(yp-y));
	if (current_time - monster->move_tempo > delay){ // On ne calcule dijkstra que si le monstre va se déplacer
		if (dist < dist_detection(level,levels)){
			monster->l_path=dijkstra(monster->Path,map, player,monster->x,monster->y);
			monster->nb_path=0;
			/*
			printf("joueur en x=%i y=%i\n",xp,yp);
			printf("monstre en x=%i y=%i\n",x,y);
			printf("Longueur du chemin: %i\n",monster->l_path);
			for (int i=0; i<monster->l_path;i++){
				int val=monster->Path[i];
				int x_temp = val%STATIC_MAP_WIDTH;
				int y_temp = val/STATIC_MAP_WIDTH;
				printf(" %i %i\n",x_temp,y_temp);
			}*/
		}
	}
	if (monster->l_path - monster->nb_path == 1 && current_time - monster->move_tempo >delay_monstre(level)){
		int test =1;
		if (xp-x == 1 && yp==y)
			monster->direction =EAST;
		else if (xp-x == -1 && yp==y)
			monster->direction =WEST;
		else if (yp-y == 1 && xp ==x)
			monster->direction =SOUTH;
		else if (yp-y == -1 && xp ==x)
			monster->direction =NORTH;
		else{
			test =0;
		}
		if (test && monster_move_aux(monster, map, xp, yp)){
			monster->x = xp;
			monster->y=yp;
			monster->move_tempo=current_time;
			move = 1;
			if (kill_monsters(monster, map))
				map_set_cell_type(map,monster->x,monster->y,CELL_MONSTER);
			map_set_cell_type(map,x,y,CELL_EMPTY);	
			}
		monster->l_path = 0;
		monster->nb_path =0;
	}
	else if (monster->l_path - monster->nb_path < 1 ){ //Si on a pas de chemin jusqu'au joueur ou qu'on l'a parcourut et ne trouve plus le joueur
		monster->l_path =0;
		monster->nb_path =0;
		//printf("Je suis en alea\n");
		if (monster_will_move(level)) {
    		monster_set_current_way_rand(monster, map);
    		switch (monster->direction) {
			case NORTH:
				if (monster_move_aux(monster, map, x, y - 1)) {
					monster->y--;
					move = 1;
					if (kill_monsters(monster, map))
						map_set_cell_type(map,monster->x,monster->y,CELL_MONSTER);
					map_set_cell_type(map,x,y,CELL_EMPTY);	
				}
				break;

			case SOUTH:
				if (monster_move_aux(monster, map, x, y + 1)) {
					monster->y++;
					move = 1;
					if (kill_monsters(monster, map))
						map_set_cell_type(map,monster->x,monster->y,CELL_MONSTER);
					map_set_cell_type(map,x,y,CELL_EMPTY);
				}
				break;

			case WEST:
				if (monster_move_aux(monster, map, x - 1, y)) {
					monster->x--;
					move = 1;
					if (kill_monsters(monster, map))
						map_set_cell_type(map,monster->x,monster->y,CELL_MONSTER);
					map_set_cell_type(map,x,y,CELL_EMPTY);
				}
				break;

			case EAST:
				if (monster_move_aux(monster, map, x + 1, y)) {
					monster->x++;
					move = 1;
					if (kill_monsters(monster, map))
						map_set_cell_type(map,monster->x,monster->y,CELL_MONSTER);
					map_set_cell_type(map,x,y,CELL_EMPTY);
					
				}
				break;
			}
		}
	}
	else{
		//printf("delay: %i timer: %i\n", delay, current_time-monster->move_tempo);
		if (current_time - monster->move_tempo >delay){
			int cell_next=monster->Path[monster->l_path - monster->nb_path -2];
			int x_dest = cell_next%STATIC_MAP_WIDTH;
			int y_dest = cell_next/STATIC_MAP_WIDTH;
			//printf("Le monstre en %i %i bouge en %i %i, merci dijkstra\n",x,y,x_dest,y_dest);
			if (monster_move_aux(monster,map,x_dest,y_dest) && dist!=0){
				
				int test = 0;
				if (x_dest-x == 1 && y_dest ==y){
					monster->direction =EAST;
					monster->x++;
					test = 1;
				}
				else if (x_dest-x == -1 && y_dest ==y){
					monster->direction =WEST;
					monster->x--;
					test = 1;
				}
				else if (y_dest-y == 1 && x_dest ==x){
					monster->direction =SOUTH;
					monster->y++;
					test = 1;
				}
				else if (y_dest-y == -1 && x_dest ==x){
					monster->direction =NORTH;
					monster->y--;
					test = 1;
				}
				if (test){
					map_set_cell_type(map,x,y,CELL_EMPTY);
					move = 1;
					if (kill_monsters(monster,map))
						map_set_cell_type(map,monster->x,monster->y,CELL_MONSTER);
					monster->nb_path++;
					//printf("Le monstre a bougé!\n");
				}
			}
			monster->move_tempo = current_time;
		}
	}
	return move;
}

void monster_display(struct monster* monster) {
	assert(monster);
	window_display_image(sprite_get_monster(monster->direction),
			monster->x * SIZE_BLOC, monster->y * SIZE_BLOC);
}
