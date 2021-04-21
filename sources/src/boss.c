#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL_image.h>
#include <assert.h>
#include <math.h>

#include <boss.h>
#include <player.h>
#include <sprite.h>
#include <constant.h>
#include <monster.h>
#include <bomb.h>
#include <map.h>
#include <dijkstra.h>
#include <game.h>

struct bomb_thrown{
    int* boss_throw_to;
    int* current_coord;
    int last_update;
};

struct boss {
    int lives;
    int phase;
    int x,y;
    enum direction direction;
    int actif;          //Si le boss est vivant
    int invulnerabilite; // 1 = invulnérable
    int tempo_invu;
    struct bomb_thrown* bomb_thrown;
    int can_launch_bomb;
    int launched_bomb; // Indique si des bombes sont en train de se déplacer
    int tempo_launch; // temps du dernier lancé
    int tempo_monstre;
    int tempo_move;
};

struct bomb_thrown* bomb_thrown_intit(struct boss* boss){
    struct bomb_thrown* bomb_thrown = malloc(sizeof(struct bomb_thrown)*50);
    bomb_thrown->boss_throw_to= malloc((NB_BOMB_BOSS_MAX*2)*sizeof(int));
    bomb_thrown->current_coord= malloc((NB_BOMB_BOSS_MAX*2)*sizeof(int));
    for (int i =0; i<NB_BOMB_BOSS_MAX;i++){
        bomb_thrown->boss_throw_to[2*i] = 0;
        bomb_thrown->boss_throw_to[2*i+1] = 0;
        bomb_thrown->current_coord[2*i] = 40*boss->x;
        bomb_thrown->current_coord[2*i+1] = 40*boss->y;
    }
    bomb_thrown->last_update=0;
    return bomb_thrown;
}

struct boss* boss_init(){
    struct boss* boss = malloc(sizeof(struct boss));
    boss->lives = BOSS_LIVES;
    boss->phase = 0;
    boss->x=9;
    boss->y=1;
    boss->direction = SOUTH;
    boss->actif = 1;
    boss->invulnerabilite = 0;
    boss->can_launch_bomb = 0;
    boss->bomb_thrown=bomb_thrown_intit(boss);
    boss->launched_bomb = 0;
    boss->tempo_launch = 0;
    boss->tempo_monstre =-20000;
    boss->tempo_move =0;
    return boss;
}

int get_nb_bomb_phase(struct boss* boss){
    switch(boss->phase){
        case 0:
            return NB_BOMB_BOSS;
            break;
        case 1:
            return NB_BOMB_BOSS_2;
            break;
        case 2:
            return NB_BOMB_BOSS_3;
            break;
        default:
            return 0;
            break;

    }
}
int boss_is_alive(struct boss* boss){
    return boss->actif;
}

void boss_set_actif(struct boss* boss, int val){
    boss->actif= val;
}

int boss_can_launch_bomb(struct boss* boss){
    return boss->can_launch_bomb;
}

void boss_set_launch_bomb(struct boss* boss, int val){
    boss->can_launch_bomb=val;
}

void boss_kill(struct boss* boss, struct map* map){
    boss->actif = 0;
    map_set_cell_type(map,10,0,CELL_EMPTY);
    map_set_cell_type(map,11,1,CELL_EMPTY);
    map_set_cell_type(map,boss->x,boss->y,CELL_EMPTY);
    //printf("Le boss meurt\n");
}


int boss_get_lives(struct boss* boss){
    return (boss->lives);
}

void boss_set_lives(struct boss* boss,int val){
    if (val >=0 && val <= BOSS_PHASE3){
        boss->lives = val;
        boss->phase = 2;
    }
    if (val >BOSS_PHASE3 && val <= BOSS_PHASE2){
        boss->lives = val;
        boss->phase = 1;
    }
    if (val >BOSS_PHASE2 && val <= BOSS_LIVES){
        boss->lives = val;
        boss->phase = 0;
    }
}

int boss_get_x(struct boss* boss){
    return boss->x;
}

void boss_set_x(struct boss* boss, int val){
    boss->x= val;
}

int boss_get_y(struct boss* boss){
    return boss->y;
}

void boss_set_y(struct boss* boss, int val){
    boss->y= val;
}

int boss_get_direction(struct boss* boss){
    return boss->direction;
}

void boss_set_direction(struct boss* boss, int val){
    boss->direction= val;
}

int boss_get_launched_bomb(struct boss* boss){
    return boss->launched_bomb;
}


void boss_set_launched_bomb(struct boss* boss, int val){
    boss->launched_bomb= val;
}

int boss_get_tempo_launch(struct boss* boss){
    return boss->tempo_launch;
}

void boss_set_tempo_launch(struct boss* boss, int val){
    boss->tempo_launch=val;
}

int boss_get_tempo_monstre(struct boss* boss){
    return boss->tempo_monstre;
}

void boss_set_tempo_monstre(struct boss* boss, int val){
    boss->tempo_monstre = val;
}

int boss_get_tempo_move(struct boss* boss){
    return boss->tempo_move;
}

void boss_set_tempo_move(struct boss* boss, int val){
    boss->tempo_move= val;
}

struct bomb_thrown* boss_get_bombs(struct boss*boss){
    return boss->bomb_thrown;
}

int boss_get_bomb_last_update(struct bomb_thrown* bomb_thrown){
    return bomb_thrown->last_update;
}

void boss_set_bomb_last_update(struct bomb_thrown* bomb_thrown, int current_time){
    bomb_thrown->last_update = current_time;
}

int boss_get_bomb_x(struct bomb_thrown* bomb_thrown, int i){
    return bomb_thrown->boss_throw_to[2*i];
}

void boss_set_bomb_x(struct bomb_thrown* bomb_thrown, int i, int val){
    bomb_thrown->boss_throw_to[2*i] = val;
}
void boss_set_bomb_y(struct bomb_thrown* bomb_thrown, int i, int val){
    bomb_thrown->boss_throw_to[2*i+1] = val;
}

int boss_get_bomb_y(struct bomb_thrown* bomb_thrown, int i){
    return bomb_thrown->boss_throw_to[2*i+1];
}

int boss_get_current_bomb_x(struct bomb_thrown* bomb_thrown, int i){
    return bomb_thrown->current_coord[2*i];
}

int boss_get_current_bomb_y(struct bomb_thrown* bomb_thrown, int i){
    return bomb_thrown->current_coord[2*i+1];
}

void boss_set_current_bomb_x(struct bomb_thrown* bomb_thrown, int i, int value){
    bomb_thrown->current_coord[2*i] = value;
}

void boss_set_current_bomb_y(struct bomb_thrown* bomb_thrown, int i, int value){
    bomb_thrown->current_coord[2*i + 1] = value;
}

void boss_add_lives(struct boss* boss,struct map* map, int i, int tickpaused){
    boss->lives += i;
    if (i < 0){
        boss->invulnerabilite = 1;
        boss->tempo_invu = SDL_GetTicks()-tickpaused;
    }

    if (boss->lives == BOSS_PHASE2)
        boss_change_phase(boss, 1);
    if (boss->lives == BOSS_PHASE3)
        boss_change_phase(boss, 2);
    if (boss->lives <= 0){
        boss_kill(boss, map);
    }
}


int boss_get_phase(struct boss* boss) {
    return boss->phase;
}

void bomb_travel(struct bomb** bombes,struct boss* boss,struct player* player,struct map* map,int nb_bombes, int tickpaused){
    int current_time = SDL_GetTicks() - tickpaused;
    int time_passed = current_time - boss_get_bomb_last_update(boss->bomb_thrown);
    //printf("time_passed= %i", time_passed);
    if (time_passed >250){
        for (int i = 0; i < nb_bombes; i++){
            int x_dest = 40*boss_get_bomb_x(boss->bomb_thrown,i);
            int y_dest = 40*boss_get_bomb_y(boss->bomb_thrown,i);
            int x_bomb =boss_get_current_bomb_x(boss->bomb_thrown,i);
            int y_bomb =boss_get_current_bomb_y(boss->bomb_thrown,i);
            float dist = sqrt(square(x_dest-x_bomb)+square(y_dest-y_bomb));
            if (abs(dist) < VITESSE_BOMB*300*40/1000 || current_time -boss->tempo_launch > 4000){
                if (dist !=0){
                    int x_def =boss_get_bomb_x(boss->bomb_thrown,i);
                    int y_def=boss_get_bomb_y(boss->bomb_thrown,i);
                    game_place_bomb(bombes, x_def, y_def, player, map, tickpaused,0);
                    //printf("bombe %i placé en %i,%i\n",i,x_def,y_def);
                    boss_set_current_bomb_x(boss->bomb_thrown, i,x_dest);
                    boss_set_current_bomb_y(boss->bomb_thrown,i,y_dest);
                    boss->launched_bomb--;
                }
            }
            else{
                //printf("time=%i\n",time_passed);
                float d_parcourut= time_passed * VITESSE_BOMB*40/1000;
                //printf("La bombe %i parcourt %f mètre sur %f restant\n",i,d_parcourut,dist);
                float x_parcourut = (x_dest-x_bomb) *d_parcourut/dist;
                float y_parcourut = (y_dest-y_bomb) *d_parcourut/dist;
                if (0<x_parcourut && x_parcourut<1)
                    x_parcourut = 40;
                if (-1<x_parcourut && x_parcourut<0)
                    x_parcourut = -40;
                if (0<y_parcourut && y_parcourut<1)
                    y_parcourut = 40;
                if (-1<y_parcourut && y_parcourut<0)
                    y_parcourut = -40;
                //printf("La bombe %i était en x=%i, y=%i sur x\n",i,x_bomb,y_bomb);
                //printf("La bombe parcourt %f, ou %i sur x et %f ou %i sur y\n",x_parcourut,(int)x_parcourut,y_parcourut,(int)y_parcourut);
                int x_new= x_bomb + (int)x_parcourut;
                int y_new= y_bomb + (int)y_parcourut ;
                boss_set_current_bomb_x(boss->bomb_thrown, i,x_new);
                boss_set_current_bomb_y(boss->bomb_thrown,i,y_new);
                //printf("La bombe %i voyage %i %i\n",i,x_new,y_new);
                boss_set_bomb_last_update(boss->bomb_thrown,current_time);
            }
        }
    }

}

void boss_throw_bomb(struct boss* boss,struct player* player, struct map* map,struct bomb_thrown* bomb_thrown,int nb_bombes, int tickpaused){
    int cell_disp[2*(2*HEIGHT_BOMB+1)*(2*WIDTH_BOMB+1)] = {0};
    int xp= player_get_x(player);
    int yp= player_get_y(player);
    int nb_cell_disp = 0;
    for (int i =xp-WIDTH_BOMB/2; i < xp+WIDTH_BOMB/2+1; i++ ){
        for (int j =yp-HEIGHT_BOMB; j < yp+ HEIGHT_BOMB+1; j++ ){
            if (map_is_inside(map, i, j)){
                if ((map_get_cell_type(map,i,j) == CELL_EMPTY) || (map_get_cell_type(map,i,j) ==CELL_EXPLOSION)){
                    //printf("Cell libre en x=%i, y=%i\n",i,j);
                    if (i!= boss->x || j!= boss->y){
                        cell_disp[2*nb_cell_disp]=i;
                        cell_disp[2*nb_cell_disp+1]=j;
                        nb_cell_disp++;
                    }
                }
            }
        }
    }
    int nb_used[NB_BOMB_BOSS]= {0};
    //printf("IL y a %i cell libre autour du joueur\n",nb_cell_disp);
    for (int i=0; i< nb_bombes && i< nb_cell_disp;i++){
        int test = 1;
        while (test){
            test = 0;
            int nb_rand = rand()% nb_cell_disp+ 1;     //On génere 3 cases aléatoirement parmis celles dispo
            for (int c = 0; c < nb_bombes;c++){
                if  (nb_rand == nb_used[c]){
                    test = 1;
                }
            }
            if (test ==0){
                nb_used[i]=nb_rand;
                bomb_thrown->boss_throw_to[2*i]= cell_disp[2*nb_rand-2];
                bomb_thrown->boss_throw_to[2*i+1]= cell_disp[2*nb_rand-1];
                //printf("Une bombe en %i %i\n",bomb_thrown->boss_throw_to[2*i],bomb_thrown->boss_throw_to[2*i+1]);
            }
        }
    }
    bomb_thrown->last_update=SDL_GetTicks()-tickpaused;
}

void boss_change_phase(struct boss* boss, int i){
    switch (i){
        case 1:
            boss->phase =1;
            break;
        case 2:
            boss->phase = 2;
            break;
    }
}

int boss_get_invul_tempo(struct boss* boss){
    return boss->tempo_invu;
}

void boss_set_invul_tempo(struct boss* boss, int val){
    boss->tempo_invu=val;
}

int boss_get_invu(struct boss* boss){
    return boss->invulnerabilite;
}

void boss_set_invu(struct boss* boss, int val){
    boss->invulnerabilite=val;
}

static int boss_move_aux(struct boss* boss, struct map* map, int x, int y) {
	int xporte = 2*x-boss->x;
	int yporte = 2*y-boss->y;	// Coordodoné où une porte pourrait se trouver
	if (!map_is_inside(map, x, y)) {
		return 0;
    }
	if (map_is_inside(map, xporte, yporte)) {
		if (map_get_cell_type(map, xporte, yporte)==CELL_DOOR) {
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
    case CELL_DOOR:
        return 0;
        break;
	default:
		break;
		}
	
	// Boss has moved
	return 1;
}

void boss_move(struct boss* boss, struct map* map,struct player* player, int tickpaused){
    int current_time = SDL_GetTicks() - tickpaused;
    if (current_time- boss->tempo_move > delay_monstre(player_get_level(player))){
        int* Path =malloc(sizeof(int)*STATIC_MAP_WIDTH*STATIC_MAP_HEIGHT);
        int xb = boss->x;
        int yb = boss->y;
        int l_path = dijkstra(Path, map, player, xb, yb);
        if (l_path>1){
            int x_dest = Path[l_path - 2]%STATIC_MAP_WIDTH;
            int y_dest = Path[l_path - 2]/STATIC_MAP_WIDTH;
            int test =0;
            if (x_dest-xb == 1 && y_dest == yb){
		    	boss->direction =EAST;
                test =1;
            }
		    else if (x_dest-xb == -1 && y_dest == yb){
		    	boss->direction =WEST;
                test =1;
            }
		    else if (y_dest-yb == 1 && x_dest == xb){
			    boss->direction =SOUTH;
                test =1;
            }
		    else if (y_dest-yb == -1 && x_dest == xb){
		    	boss->direction =NORTH;
                test=1;
            }
		    else {
		    	//printf("Erreur1 le boss ne se déplace pas sur une case adjacente: de x=%i y=%i à x=%i y=%i\n",xb,yb,x_dest,y_dest);
		    }
            if (boss_move_aux(boss,map,x_dest,y_dest) && test){
                map_set_cell_type(map,boss->x,boss->y,CELL_EMPTY);
                map_set_cell_type(map,x_dest,y_dest,CELL_BONUS | 7);
                boss->x = x_dest;
                boss->y= y_dest;
                boss->tempo_move = current_time;
            }
        }
        else{
            //printf("l_path vaut %i\n", l_path);
            int xp =player_get_x(player);
            int yp =player_get_y(player);
            if (boss_move_aux(boss,map,xp,yp)){
                if (xp-xb == 1 && yp==yb){
		    	    boss->direction =EAST;
                    map_set_cell_type(map,boss->x,boss->y,CELL_EMPTY);
                    boss->x++;
                    map_set_cell_type(map,boss->x,boss->y,CELL_BONUS | 7);
                    boss->tempo_move = current_time;
                }
		        else if (xp-xb == -1 && yp==yb){
		    	    boss->direction =WEST;
                    map_set_cell_type(map,boss->x,boss->y,CELL_EMPTY);
                    boss->x--;
                    map_set_cell_type(map,boss->x,boss->y,CELL_BONUS | 7);
                    boss->tempo_move = current_time;
                }
		        else if (yp-yb == 1 && xp == xb){
			        boss->direction =SOUTH;
                    map_set_cell_type(map,boss->x,boss->y,CELL_EMPTY);
                    boss->y++;
                    map_set_cell_type(map,boss->x,boss->y,CELL_BONUS | 7);
                    boss->tempo_move = current_time;
                }
		        else if (yp-yb == -1 && xp == xb){
		    	    boss->direction =NORTH;
                    map_set_cell_type(map,boss->x,boss->y,CELL_EMPTY);
                    boss->y--;
                    map_set_cell_type(map,boss->x,boss->y,CELL_BONUS | 7);
                    boss->tempo_move = current_time;
                }
                else {
		    	    //printf("Erreur2 le boss ne se déplace pas sur une case adjacente: de x=%i y=%i à x=%i y=%i\n",xb,yb,xp,yp);  
                }
            }
        }
    }
}

void boss_damage(struct player* player, int x, int y, int tickpaused){
    int xp = player_get_x(player);
    int yp = player_get_y(player);
    if (xp == x && yp == y){
        if (player_get_invulnerabilite(player)==0)
            player_add_lives(player, -1, tickpaused);
    }
}

void boss_update(struct monster** monsters,struct bomb** bombes,struct boss* boss,struct player* player, struct map* map,int level, int tickpaused){
    int x = boss->x;
    int y= boss->y;
    boss_damage(player,x,y,tickpaused);
    int current_time = SDL_GetTicks()-tickpaused;
    if (map_get_cell_type(map,x,y)==CELL_EXPLOSION){
        if (boss->invulnerabilite == 0){
            //printf("Le boss perd une vie\n");
            boss_add_lives(boss,map, -1, tickpaused);
        }
    }
    if (boss->invulnerabilite){
        if (current_time - boss->tempo_invu >1000){
            boss->invulnerabilite = 0;
            //printf("le boss n'est plus invulnérable");
        }
    }
    switch(boss_get_phase(boss)){
        case 0:
        if (boss->can_launch_bomb){
            //printf("bombe lancé!\n");
            boss_throw_bomb(boss, player, map, boss_get_bombs(boss),NB_BOMB_BOSS,tickpaused);
            for (int i =0; i < NB_BOMB_BOSS; i++){
                boss->can_launch_bomb = 0;
                boss->tempo_launch = SDL_GetTicks()-tickpaused;
                boss->launched_bomb = NB_BOMB_BOSS;
            }
        }
        else if (boss->launched_bomb > 0){
            //printf("travel\n");
            bomb_travel(bombes,boss,player,map,NB_BOMB_BOSS,tickpaused);
        }
        else{
            //printf("test\n");
            if (current_time - boss->tempo_launch >6000){
                boss->tempo_launch = current_time;
                boss->can_launch_bomb = 1;
                for (int i =0; i< NB_BOMB_BOSS; i++){
                    boss_set_current_bomb_x(boss->bomb_thrown,i,40*x);
                    boss_set_current_bomb_y(boss->bomb_thrown,i,40*y);
                }
            }
        }
        break;
        case 1:
        if (boss->can_launch_bomb){
            boss_throw_bomb(boss, player, map, boss_get_bombs(boss),NB_BOMB_BOSS_2,tickpaused);
            for (int i =0; i < NB_BOMB_BOSS_2; i++){
                boss->can_launch_bomb = 0;
                boss->tempo_launch = SDL_GetTicks()-tickpaused;
                boss->launched_bomb = NB_BOMB_BOSS_2;
            }
        }
        else if (boss->launched_bomb > 0){
            bomb_travel(bombes,boss,player,map,NB_BOMB_BOSS_2,tickpaused);
        }
        else{
            if (current_time - boss->tempo_launch >7000){
                boss->tempo_launch = current_time;
                boss->can_launch_bomb = 1;
                for (int i =0; i< NB_BOMB_BOSS_2; i++){
                    boss_set_current_bomb_x(boss->bomb_thrown,i,40*x);
                    boss_set_current_bomb_y(boss->bomb_thrown,i,40*y);
                }
            }
        }
        if (current_time - boss->tempo_monstre > 30000){
            boss->tempo_monstre = current_time;
            create_monster(monsters,map, 9, 1,level);
            create_monster(monsters,map, 10, 3,level);
            create_monster(monsters,map, 9, 3,level);
            create_monster(monsters,map, 8, 2,level);
        }
            break;
        case 2:
        boss_move(boss,map,player,tickpaused);
        if (boss->can_launch_bomb){
            boss_throw_bomb(boss, player, map, boss_get_bombs(boss),NB_BOMB_BOSS_3,tickpaused);
            for (int i =0; i < NB_BOMB_BOSS_3; i++){
                boss->can_launch_bomb = 0;
                boss->tempo_launch = SDL_GetTicks()-tickpaused;
                boss->launched_bomb = NB_BOMB_BOSS_3;
            }
        }
        else if (boss->launched_bomb > 0){
            bomb_travel(bombes,boss,player,map,NB_BOMB_BOSS_3,tickpaused);
        }
        else{
            if (current_time - boss->tempo_launch >7000){
                boss->tempo_launch = current_time;
                boss->can_launch_bomb = 1;
                for (int i =0; i< NB_BOMB_BOSS_3; i++){
                    boss_set_current_bomb_x(boss->bomb_thrown,i,40*x);
                    boss_set_current_bomb_y(boss->bomb_thrown,i,40*y);
                }
            }
        }
        if (current_time - boss->tempo_monstre > 30000){
            boss->tempo_monstre = current_time;
            create_monster(monsters,map, 3,2 ,level);
            create_monster(monsters,map, 11, 11,level);
            create_monster(monsters,map, 9, 3,level);
            create_monster(monsters,map, 0, 11,level);
        }
            break;
        default:
            //printf("ERREUR:Le boss est dans une phase bizzare\n");
            break;
    }

}



void boss_display(struct boss* boss) {
	assert(boss);
	window_display_image(sprite_get_boss(boss->direction),
			boss->x * SIZE_BLOC, boss->y * SIZE_BLOC);
}

void boss_display_bomb_thrown(struct boss* boss,struct map* map){
      for (int i = 0; i < get_nb_bomb_phase(boss);i++){
        int x_disp =boss_get_current_bomb_x(boss->bomb_thrown,i);
        int y_disp =boss_get_current_bomb_y(boss->bomb_thrown,i);
        //printf("On affiche la bombe %i en %i %i\n",i,x_disp,y_disp);
        display_bomb(map,x_disp,y_disp,1);
    }
}
