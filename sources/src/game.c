/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <assert.h>
#include <time.h>

#include <SDL/SDL_mixer.h>
#include <game.h>
#include <misc.h>
#include <window.h>
#include <sprite.h>
#include <bomb.h>
#include <player.h>
#include <monster.h>
#include <map.h>
#include <save.h>
#include <boss.h>
#include <dijkstra.h>
#include <constant.h>
#include <music.h>

struct game {
	struct map** maps;       // the game's map
	short levels;        // nb maps of the game
	short level;
	struct player* player;
	struct monster** monsters;
	struct bomb** bombes;
	int pause; 				//0 = pause
	int tick_paused;
	struct boss* boss;
	int menu;
	int game_has_started;
	int paused;
	int music;
};

struct game*
game_new(void) {
	sprite_load(); // load sprites into process memory


	struct game* game = malloc(sizeof(struct game)*1000);

	
	
	FILE * file;
	file=fopen("data/data.txt","r");
	int player_x,levels,current_level;
	int player_y;
	char folder[256];
	fscanf(file,"%i\n%i:%i,%i\n%s",&levels,&current_level,&player_x,&player_y,folder);
	game->menu=0;
	game->paused=0;
	game->game_has_started=0;
	game->level = current_level;
	game->levels = levels;
	game->maps = malloc(sizeof(struct game) * game->levels);
	game->monsters = malloc(sizeof(struct game)*NB_MONSTER_MAX);
	game->pause = 1;
	game->music=0;
	game->tick_paused = 0;
	game->player = player_init(3);
	// Set default location of the player
	player_set_position(game->player, player_x, player_y,current_level);

	for (int i = 0; i < game->levels; i++){
		game->maps[i]=map_create_level(i,folder);
		for (int j = 0; j < get_nb_monster(game->maps[i]);j++){
			int xm = get_monster_init_x(game->maps[i],j);
			int ym = get_monster_init_y(game->maps[i],j);
			game->monsters[j + i* NB_MONSTER_MAX] = monster_init(xm,ym,1);
		}
		for (int j = get_nb_monster(game->maps[i]); j< NB_MONSTER_MAX;j++){
			game->monsters[j + i* NB_MONSTER_MAX] = monster_init(0,0,0);
		}
	}

	game->boss = boss_init();

	game->bombes=malloc(sizeof(struct game)*15);
	for (int i=0;i<=15;i++){
		game->bombes[i]=bomb_init();
	}
	return game;
}

void game_free(struct game* game) {
	assert(game);

	player_free(game->player);
	for (int i = 0; i < game->levels; i++)
		map_free(game->maps[i]);
}

int game_get_level(struct game* game){
	assert(game);
	return game->level;
}

int game_get_levels(struct game* game){
	assert(game);
	return game->levels;
}

void game_set_level(struct game* game, int i){
	game->level = i;
}

int game_get_start(struct game* game){
	return game->game_has_started;
}

void game_set_start(struct game* game,int start){
	game->game_has_started=start;
}

struct monster* game_get_monster(struct game* game, int i){
	return game->monsters[i];
}

struct monster** game_get_monsters(struct game* game){
	return game->monsters;
}

struct map* game_get_current_map(struct game* game) {
	assert(game);
	return game->maps[game->level];
}
struct map* game_get_map(struct game* game, int i){
	assert(game);
	return game->maps[i];
}

int game_get_menu(struct game* game){
	return game->menu;
}

struct bomb* game_get_bomb_tab(struct game* game, int i){
	assert(game);
	return game->bombes[i];
}

struct boss* game_get_boss(struct game* game){
	return game->boss;
}
struct player* game_get_player(struct game* game) {
	assert(game);
	return game->player;
}
void game_set_tick_pause(struct game* game,int i){
	game->tick_paused=i;
}
int game_get_tick_pause(struct game* game){
	return game->tick_paused;
 }
void game_place_bomb(struct bomb** bombes,int x,int y, struct player* player, struct map* map, int tick_paused, int poseur){
	for (int i=0;i<=15;i++){
		if (bombes[i]==0 || bomb_get_status(bombes[i]) ==0){
			music_get_explosion();
			bombes[i]=bomb_place(bombes[i], x, y,player,map, tick_paused, poseur);
			break;
		if (i == 15){
			printf("plus de mémoire pour la bombe\n");
			}
		}
	}
}

void game_banner_display(struct game* game) {
	assert(game);

	struct map* map = game_get_current_map(game);

	int y = (map_get_height(map)) * SIZE_BLOC;
	for (int i = 0; i < map_get_width(map); i++)
		window_display_image(sprite_get_banner_line(), i * SIZE_BLOC, y);

	int white_bloc = ((map_get_width(map) * SIZE_BLOC) - 6 * SIZE_BLOC) / 4;
	int x = white_bloc;
	y = (map_get_height(map) * SIZE_BLOC) + LINE_HEIGHT;
	window_display_image(sprite_get_banner_life(), 0, y);

	x = SIZE_BLOC ;
	window_display_image(sprite_get_number(player_get_lives(game->player)), x, y);

	x = 1 * white_bloc + 1 * SIZE_BLOC;
	window_display_image(sprite_get_banner_bomb(), x, y);

	x = 1 * white_bloc + 2 * SIZE_BLOC;
	window_display_image(
			sprite_get_number(player_get_nb_bomb(game_get_player(game))), x, y);

	x = 2 * white_bloc + 2 * SIZE_BLOC;
	window_display_image(sprite_get_banner_range(), x, y);

	x = 2 * white_bloc + 3 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_bomb_range(game->player)), x, y);

	x = 3 * white_bloc + 3 * SIZE_BLOC;
	window_display_image(sprite_get_door_opened(), x, y);

	x = 3 * white_bloc + 4 * SIZE_BLOC;
	window_display_image(sprite_get_number(game->level), x, y);


	x = 4 * white_bloc + 4 * SIZE_BLOC;
	window_display_image(sprite_get_key(), x, y);
	x = 4 * white_bloc + 5 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_key(game->player)), x, y);
}

void game_display(struct game* game) {
	assert(game);

	window_clear();
	game_banner_display(game);
	
	map_display(game_get_current_map(game));

	player_display(game->player);
	for (int i = 0; i < get_nb_monster(game_get_current_map(game)); i++){
		int num_monster= i + NB_MONSTER_MAX * game->level;
		if (monster_get_actif(game->monsters[num_monster])){
			monster_display(game->monsters[num_monster]);
		}
	}

	if (game->level == game->levels -1){
		if (boss_is_alive(game->boss)){
			boss_display(game->boss);
			if (boss_get_launched_bomb(game->boss))
				boss_display_bomb_thrown(game->boss, game->maps[game->level]);
		}
	}
	if (player_has_win(game->player))
		window_display_image(sprite_get_win(),0,0);
	if (player_get_lives(game->player)==0)
		window_display_image(sprite_get_lose(),0,0);
	window_refresh();
}


void game_change_level(struct game* game){

	if (player_get_level(game->player)!=game->level){
		int door_x[7]={3,3,11,10,11,2,0};
		int door_y[7]={11,9,1,0,10,10,1};
		music_level_free();
		if(player_get_level(game->player)<game->level){
			game->level=player_get_level(game->player);
			player_set_position(game->player,door_x[game->level],door_y[game->level],game->level);
		}
		else{
			game->level=player_get_level(game->player);
			player_set_position(game->player,1,0,game->level);
		}
		
	}
}

static short input_keyboard(struct game* game) {
	SDL_Event event;
	struct player* player = game_get_player(game);
	struct map* map = game_get_current_map(game);
	int menu_status=0;

	while (SDL_PollEvent(&event)) {	
						
								
		while (!game->menu){
													
					while(SDL_PollEvent(&event)) {
						switch (event.type) {						
						case SDL_QUIT:								
							return 1;									
						case SDL_KEYDOWN:							
							switch(event.key.keysym.sym){									
								case SDLK_RETURN:					
									game->menu = 1;
									if(game->game_has_started==1){
										switch (menu_status%4){
											case 0:
												game->tick_paused+=SDL_GetTicks()-game->paused;
												Mix_Resume(-1);
												break;
											case 1:
												save_game(game);
												game->tick_paused+=SDL_GetTicks()-game->paused;
												Mix_Resume(-1);
												break;
											case 2:
												window_display_image(sprite_get_help(),0,0);
												window_refresh();
												game->pause = 0;
												while (!game->pause){
													SDL_Event event;
													while(SDL_PollEvent(&event)) {
														switch (event.type) {
														case SDL_QUIT:
															return 1;
														case SDL_KEYDOWN:
															switch(event.key.keysym.sym){
																case SDLK_ESCAPE:
																	game->menu=0;
																	game->pause = 1;
																	break;
																default:
																	break;
															}
															break;
														default:
															break;
														}
													}
												}
												break;
											case 3:
												return 1;
												break;
										}
									}
									if (game->game_has_started==0){
										
										switch (menu_status%4){
											case 0:
												Mix_CloseAudio();
												music_menu_free();
												game->game_has_started=1;
												break;
											case 1:
												load_game(game);
												game->game_has_started=1;
												Mix_CloseAudio();
												music_menu_free();
												break;
											case 2:
												window_display_image(sprite_get_help(),0,0);
												window_refresh();
												game->pause = 0;
												while (!game->pause){
													SDL_Event event;
													while(SDL_PollEvent(&event)) {
														switch (event.type) {
														case SDL_QUIT:
															return 1;
														case SDL_KEYDOWN:
															switch(event.key.keysym.sym){
																case SDLK_ESCAPE:
																	game->menu=0;
																	game->pause = 1;
																	break;
																default:
																	break;
															}
															break;
														default:
															break;
														}
													}
												}
												
												break;
											case 3:
												return 1;
												break;
										}	
									}
													
									break;
								case SDLK_DOWN:
									if (menu_status!=3)
										menu_status++;
									break;
								case SDLK_UP:
									if (menu_status!=0)
										menu_status--;
									break;
								default:							
									break;							
							}										
							break;									
						default:									
							break;
						
						}
				}
			switch (menu_status%4){
				case 0:
					window_display_image(sprite_get_menu(game->game_has_started*4),0,0);
					break;
				case 1:
					window_display_image(sprite_get_menu(game->game_has_started*4+1),0,0);
					break;
				case 2:
					window_display_image(sprite_get_menu(game->game_has_started*4+2),0,0);
					break;
				case 3:
					window_display_image(sprite_get_menu(game->game_has_started*4+3),0,0);
					break;
			}
			window_refresh();
		}
		switch (event.type) {
			
		case SDL_QUIT:
			return 1;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				game->menu=0;
				game->paused=SDL_GetTicks();
				Mix_Pause(-1);
				break;
			case SDLK_UP:
				player_set_current_way(player, NORTH);
				player_move(player, map);
				break;
			case SDLK_DOWN:
				player_set_current_way(player, SOUTH);
				player_move(player, map);
				break;
			case SDLK_RIGHT:
				player_set_current_way(player, EAST);
				player_move(player, map);
				break;
			case SDLK_LEFT:
				player_set_current_way(player, WEST);
				player_move(player, map);
				break;
			case SDLK_SPACE:
				if (player_get_nb_bomb(game->player)>0){
					if(map_get_cell_type(map,player_get_x(player), player_get_y(player))!=CELL_DOOR && map_get_cell_type(map,player_get_x(player), player_get_y(player))!=CELL_BOMB && map_get_cell_type(map,player_get_x(player), player_get_y(player))!=CELL_MONSTER){
						game_place_bomb(game->bombes,player_get_x(player), player_get_y(player),player,map, game->tick_paused,1);
						player_dec_nb_bomb(player);
					}
				}
				break;
			/*case SDLK_m:
				create_monster(game->monsters, game->maps[game->level], 0, 0,game->level);
				break;
			case SDLK_s:
				save_game(game);
				break;
			case SDLK_l:
				load_game(game);
				break;
			case SDLK_d:;
				int* Path=malloc(STATIC_MAP_WIDTH*STATIC_MAP_HEIGHT*sizeof(int));
				int l_path=0;
				l_path =dijkstra(Path, game->maps[game->level],game->player,2,1);
				printf("Longueur du chemin: %i\n",l_path);
				for (int i=0; i<l_path;i++){
					int val=Path[i];
					int x_temp = val%STATIC_MAP_WIDTH;
					int y_temp = val/STATIC_MAP_WIDTH;
					printf(" %i %i\n",x_temp,y_temp);
				}
				break;
			*/
			case SDLK_p:
				game->pause = 0;
				Mix_Pause(-1);
				int tick_start =SDL_GetTicks();
				while (!game->pause){
					SDL_Event event;
					while(SDL_PollEvent(&event)) {
						switch (event.type) {
						case SDL_QUIT:
							return 1;
						case SDL_KEYDOWN:
							switch(event.key.keysym.sym){
								case SDLK_p:
									game->pause = 1;
									game->tick_paused +=SDL_GetTicks() - tick_start;
									Mix_Resume(-1);
									break;
								case SDLK_ESCAPE:
									return 1;
									break;
								default:
									break;
							}
							break;
						default:
							break;
						}
					}
				}
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		
	}
	
	return 0;
}

int game_update(struct game* game) {
	
	music_play(game);
	game_change_level(game);
	for (int i = 0; i < get_nb_monster(game->maps[game->level]); i++){		//Deplacement monstre
		int num_monster= i + NB_MONSTER_MAX * game->level;
		if (monster_get_actif(game->monsters[num_monster])){
			monster_update(game->monsters[num_monster],game->player, game->maps[game->level], game->level,game->levels, game->tick_paused);
			
		}
		
	}
	
	for (int i=0;i<=15;i++){
		if (bomb_get_status(game->bombes[i])>0 && bomb_get_status(game->bombes[i])<6) {	//update bombe
			bomb_update(game->bombes[i],game->maps[bomb_get_level(game->bombes[i])],game->player, game->tick_paused,game->monsters);
		}
	}

	

	int current_time=SDL_GetTicks() - game->tick_paused;
	
	if (player_get_invulnerabilite(game->player)==0){
		player_update(game->player, game->maps[game->level], game->tick_paused);
		
	}
	if ((current_time - get_invul_tempo(game->player)) > 1000 ) {	//1000= 1s d'invulnerabilité
			player_set_invulnerabilite(game->player, 0, game->tick_paused);
		}
	
	if (game->level == game->levels - 1){
		if (boss_is_alive(game->boss)){
			boss_update(game->monsters,game->bombes,game->boss,game->player,game->maps[game->level],game->level,game->tick_paused);
		}
	}

	if (input_keyboard(game)){
		return 1; // exit game
	}
	return 0;	
}


