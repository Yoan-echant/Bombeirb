#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL_image.h>
#include <assert.h>
#include <time.h>

#include <save.h>
#include <game.h>
#include <player.h>
#include <bomb.h>
#include <map.h>
#include <monster.h>
#include <constant.h>
#include <boss.h>



void save_game(struct game* game) {
    FILE* save = fopen("save/save.txt","w");
    int level =game_get_level(game);
    int keys =player_get_key(game_get_player(game));
    int nb_bombs=player_get_nb_bomb(game_get_player(game));
    int lives=player_get_lives(game_get_player(game));
    int player_x = player_get_x(game_get_player(game));
    int player_y = player_get_y(game_get_player(game));
    int bomb_range=player_get_bomb_range(game_get_player(game));
    int player_win=player_has_win(game_get_player(game));
    int game_started=game_get_start(game);
    fprintf(save,"%i %i %i %i\n%i %i\n %i\n %i\n %i\n",level,keys,nb_bombs,lives,player_x,player_y,bomb_range,player_win,game_started);
    FILE* file=fopen("data/data.txt","r");
	char folder[256];
    int player_xx,levelss,current_level;
	int player_yy;
    fscanf(file,"%i\n%i:%i,%i\n%s",&levelss,&current_level,&player_xx,&player_yy,folder);
        
    for (int i = 0; i< game_get_levels(game); i++){
        char map_level[256]="map/";
        int k=0;
        while(folder[k]!='\0'){
            k++;
        }
        for (int j=4;j<4+k;j++){
            map_level[j]=folder[j-4];
        }
        map_level[4+k]='_';
        map_level[5+k]=i+'0';
        FILE *original=fopen(map_level,"r");
        int width;
	    int height;
	    fscanf(original,"%i:%i",&width,&height);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int current_cell = map_get_cell_type(game_get_map(game,i),x,y)+get_bonus_type(game_get_map(game,i),x,y);
                if (current_cell > 95 && current_cell <110){
                    //printf("96->111 en %i %i \n",x,y);
                    current_cell = 111; // on change les case ou il y a un monstre afin de detecter les cas ou un monstre est sur un spawn
                }
                int cell;
                fscanf(original, "%i", &cell);
                if (cell != current_cell) {
                    //printf("cell originale:%i, nouvelle cell:%i\n",cell, current_cell);
                    fprintf(save,"%i %i %i\n",x,y,current_cell);
                }
                
            }
        }

        fclose(original);
        
        fprintf(save,"%i %i %i\n",width + 10,-1,-1);
        
    }
    for (int k=0;k<=15;k++){
        int bomb_x=bomb_get_x(game_get_bomb_tab(game,k));
        int bomb_y=bomb_get_y(game_get_bomb_tab(game,k));
        int bomb_status=bomb_get_status(game_get_bomb_tab(game,k));
        int bomb_range2=bomb_get_range(game_get_bomb_tab(game,k));
        int creation=bomb_get_creation(game_get_bomb_tab(game,k));
        int bomb_level=bomb_get_level(game_get_bomb_tab(game,k));
        fprintf(save,"%i %i %i %i %i %i\n",bomb_x,bomb_y,bomb_status,bomb_range2,creation,bomb_level);
    }
    int lives_boss =boss_get_lives(game_get_boss(game));
    int xb = boss_get_x(game_get_boss(game));
    int yb = boss_get_y(game_get_boss(game));
    int direction = boss_get_direction(game_get_boss(game));
    int actif = boss_is_alive(game_get_boss(game));
    int invu = boss_get_invu(game_get_boss(game));
    int tempo_invu = (SDL_GetTicks() -game_get_tick_pause(game) - boss_get_invul_tempo(game_get_boss(game)))%1000;
    int can_launch_bomb = boss_can_launch_bomb(game_get_boss(game));
    int nb_launched_bomb= boss_get_launched_bomb(game_get_boss(game));
    int tempo_launch= (SDL_GetTicks() -game_get_tick_pause(game) -boss_get_tempo_launch(game_get_boss(game)))%6000;
    int tempo_monstre= (boss_get_tempo_monstre(game_get_boss(game))+SDL_GetTicks() +game_get_tick_pause(game))%30000;
    int tempo_dep_bombes = SDL_GetTicks() -game_get_tick_pause(game) - boss_get_bomb_last_update(boss_get_bombs(game_get_boss(game)));
    fprintf(save,"%i %i %i %i %i %i %i %i %i %i %i %i %i\n", lives_boss, xb, yb,direction,actif,invu,tempo_invu,can_launch_bomb,nb_launched_bomb,tempo_invu,tempo_launch,tempo_dep_bombes, tempo_monstre);
    //printf("tempo launch: %i\n",tempo_dep_bombes);
    for (int k =0; k < NB_BOMB_BOSS_MAX; k++){
        int x_dest=boss_get_bomb_x(boss_get_bombs(game_get_boss(game)),k);
        int y_dest=boss_get_bomb_y(boss_get_bombs(game_get_boss(game)),k);
        int x_cur =boss_get_current_bomb_x(boss_get_bombs(game_get_boss(game)),k);
        int y_cur =boss_get_current_bomb_y(boss_get_bombs(game_get_boss(game)),k);
        //printf("la bombe %i va en :%i %i et est en %i %i\n",k,x_dest,y_dest,x_cur,y_cur);
        fprintf(save,"%i %i %i %i\n",x_dest,y_dest,x_cur,y_cur);
    }
    fclose(file);
    fclose(save);
}

void load_game(struct game* game){
    int level;
    int keys;
    int nb_bombs;
    int lives;
    int player_x;
    int player_y;
    int levels = game_get_levels(game);
    int bomb_range;
    int player_win;
    int game_started;
    game_set_tick_pause(game,0);
    FILE* save = fopen("save/save.txt","r");
    fscanf(save,"%i %i %i %i\n%i %i\n%i\n%i\n%i",&level,&keys,&nb_bombs,&lives,&player_x,&player_y,&bomb_range,&player_win,&game_started);
    player_set_level(game_get_player(game),level);
    game_set_level(game, level);
    /*printf("bombe= %i, lives= %i, keys = %i\n",nb_bombs,lives,keys);
    printf("coord_player: x=%i y=%i\n",player_x,player_y);*/
    player_set_key(game_get_player(game),keys);
    player_set_bombs(game_get_player(game), nb_bombs);
    player_set_lives(game_get_player(game), lives);
    player_set_position(game_get_player(game),player_x,player_y,level);
    player_set_bomb_range(game_get_player(game),bomb_range);
    player_set_win(game_get_player(game),player_win);
    game_set_start(game,game_started);
	FILE* file=fopen("data/data.txt","r");
	int player_xx,levelss,current_level;
	int player_yy;
	char folder[256];
	fscanf(file,"%i\n%i:%i,%i\n%s",&levelss,&current_level,&player_xx,&player_yy,folder);
    int WIDTH[10] = {0,0,0,0,0,0,0,0,0,0};
    int HEIGHT[10] = {0,0,0,0,0,0,0,0,0,0};
    for (int i = 0; i< levels; i++){
        char map_level[256]="map/";
        int k=0;
        while(folder[k]!='\0'){
            k++;
        }
        for (int j=4;j<4+k;j++){
            map_level[j]=folder[j-4];
        }
        map_level[4+k]='_';
        map_level[5+k]=i+'0';
        FILE *original=fopen(map_level,"r");
        int width;
        int height;
        fscanf(original,"%i:%i",&width,&height);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int cell;
                fscanf(original, "%i", &cell);
                map_set_cell_type(game_get_map(game,i),x,y,cell);   
            }
        }
        WIDTH[i] = width;
        HEIGHT[i] = height;
    }
    
    int num_map = 0;
    while (num_map < game_get_levels(game)){
        int x = 0;
        int y;
        int cell;
        //printf("On a une hauteur de:%i\n",WIDTH[num_map]);
        while (x < WIDTH[num_map]){
            fscanf(save,"%i %i %i",&x,&y,&cell);
            if (x<WIDTH[num_map]){
                if (cell < 118)
                    map_set_cell_type(game_get_map(game,num_map),x,y,cell);
                else
                    map_set_cell_type(game_get_map(game,num_map),x,y,0);
            }
        }
        for (int i = 0; i < NB_MONSTER_MAX;i++){
            monster_set_actif(game_get_monster(game,num_map*NB_MONSTER_MAX+i),0);
        }
        int nb_monster=0;
        for (int i =0; i< WIDTH[num_map];i++){
            for (int j=0; j<HEIGHT[num_map];j++){
                if (map_get_cell_type(game_get_map(game,num_map),i,j) == 96){
                    if (get_bonus_type(game_get_map(game,num_map),i,j) < 6)
                        map_set_cell_type(game_get_map(game,num_map),i,j,0);
                
                    else {
                        monster_set_position(game_get_monster(game,num_map*NB_MONSTER_MAX+nb_monster),i,j);
                        monster_set_actif(game_get_monster(game,num_map*NB_MONSTER_MAX+nb_monster),1);
                        nb_monster++;
                    }
                }
            }
        }
        set_nb_monster(game_get_map(game,num_map),nb_monster);       
        num_map++;
        
    }
    for (int j=0;j<=15;j++){
        int bomb_x,bomb_y,bomb_status,bomb_range2,bomb_creation,bomb_level;
        fscanf(save,"%i %i %i %i %i %i",&bomb_x,&bomb_y,&bomb_status,&bomb_range2,&bomb_creation,&bomb_level);
        if (bomb_status==1 || bomb_status==2 || bomb_status==3 || bomb_status==4 || bomb_status==5)
            set_bomb_tab(game_get_bomb_tab(game,j),bomb_x,bomb_y,bomb_range2,bomb_status,j,SDL_GetTicks()-(5-bomb_status)*1000,game_get_map(game,bomb_level),bomb_level);
    }
    int lives_boss = 0;
    int xb =0;
    int yb=0;
    int direction=0;
    int actif=0;
    int invu=0;
    int tempo_invu = 0;
    int can_launch_bomb = 0;
    int nb_launched_bomb=0;
    int tempo_launch=0;
    int tempo_monstre=0;
    int tempo_dep_bombes = 0;
    fscanf(save,"%i %i %i %i %i %i %i %i %i %i %i %i %i\n", &lives_boss, &xb, &yb,&direction,&actif,&invu,&tempo_invu,&can_launch_bomb,&nb_launched_bomb,&tempo_invu,&tempo_launch,&tempo_dep_bombes, &tempo_monstre);
    //printf("tempo_dep_bombes = %i\n",tempo_dep_bombes);
    /*tempo_dep_bombes = SDL_GetTicks() - game_get_tick_pause(game) - tempo_dep_bombes;
    tempo_launch =SDL_GetTicks() - game_get_tick_pause(game)-tempo_launch;
    tempo_invu = SDL_GetTicks() - game_get_tick_pause(game)-tempo_invu;
    tempo_monstre =SDL_GetTicks() - game_get_tick_pause(game)-tempo_monstre;
    printf("nb de bombe lancé: %i",nb_launched_bomb);
    */
    //printf("time_passed = %i\n",SDL_GetTicks()-game_get_tick_pause(game));
    boss_set_lives(game_get_boss(game), lives_boss);
    boss_set_x(game_get_boss(game),xb);
    boss_set_y(game_get_boss(game),yb);
    boss_set_direction(game_get_boss(game),direction);
    boss_set_actif(game_get_boss(game),actif);
    boss_set_invu(game_get_boss(game),invu);
    boss_set_invul_tempo(game_get_boss(game),tempo_invu);
    boss_set_launch_bomb(game_get_boss(game),can_launch_bomb);
    boss_set_launched_bomb(game_get_boss(game),nb_launched_bomb);
    boss_set_tempo_launch(game_get_boss(game),tempo_launch);
    boss_set_tempo_monstre(game_get_boss(game),tempo_monstre);
    boss_set_bomb_last_update(boss_get_bombs(game_get_boss(game)),tempo_dep_bombes);
    for (int k =0; k < NB_BOMB_BOSS_MAX; k++){
        int x_dest =0;
        int y_dest=0;
        int x_cur =0;
        int y_cur =0;
        fscanf(save,"%i %i %i %i\n",&x_dest,&y_dest,&x_cur,&y_cur);
        boss_set_bomb_x(boss_get_bombs(game_get_boss(game)),k, x_dest);
        boss_set_bomb_y(boss_get_bombs(game_get_boss(game)),k,y_dest);
        boss_set_current_bomb_x(boss_get_bombs(game_get_boss(game)),k,x_cur);
        boss_set_current_bomb_y(boss_get_bombs(game_get_boss(game)),k,y_cur);
        //printf("La bombe %i a destination de %i %i est en %i %i\n",k,x_dest,y_dest,x_cur,y_cur);
        //printf("time_passed= %i\n", SDL_GetTicks()-game_get_tick_pause(game)-tempo_dep_bombes);
    }
    fclose(save);
    fclose(file);
}