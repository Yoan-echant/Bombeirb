/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL_image.h>
#include <assert.h>

#include <sprite.h>
#include <misc.h>

// Sprites general
#define MAP_CASE        "sprite/wood_box.png"
#define MAP_KEY			"sprite/key.png"
#define MAP_DOOR_OPENED	"sprite/door_opened.png"
#define MAP_DOOR_CLOSED	"sprite/door_closed.png"

// Scenery elements
#define MAP_STONE		"sprite/stone.png"
#define MAP_TREE        "sprite/tree.png"
#define PRINCESS 		"sprite/bomberwoman.png"
#define BOMBOX 			"sprite/bombox.png"

// Sprites of Banner
#define BANNER_LINE		"sprite/banner_line.png"
#define BANNER_LIFE		"sprite/banner_life.png"
#define BANNER_BOMB		"sprite/bomb3.png"
#define BANNER_RANGE		"sprite/banner_range.png"
#define DIGIT_0			"sprite/banner_0.jpg"
#define DIGIT_1			"sprite/banner_1.jpg"
#define DIGIT_2			"sprite/banner_2.jpg"
#define DIGIT_3			"sprite/banner_3.jpg"
#define DIGIT_4			"sprite/banner_4.jpg"
#define DIGIT_5			"sprite/banner_5.jpg"
#define DIGIT_6			"sprite/banner_6.jpg"
#define DIGIT_7			"sprite/banner_7.jpg"
#define DIGIT_8			"sprite/banner_8.jpg"
#define DIGIT_9			"sprite/banner_9.jpg"

// Sprites of Bombs
#define BOMB_TTL1       "sprite/bomb1.png"
#define BOMB_TTL2       "sprite/bomb2.png"
#define BOMB_TTL3       "sprite/bomb3.png"
#define BOMB_TTL4       "sprite/bomb4.png"
#define EXPLOSION	 	"sprite/explosion.png" //ajout de l'image de l'explosion

// Sprites of Bonus
#define IMG_BONUS_BOMB_RANGE_INC  "sprite/bonus_bomb_range_inc.png"
#define IMG_BONUS_BOMB_RANGE_DEC  "sprite/bonus_bomb_range_dec.png"
#define IMG_BONUS_BOMB_NB_INC     "sprite/bonus_bomb_nb_inc.png"
#define IMG_BONUS_BOMB_NB_DEC     "sprite/bonus_bomb_nb_dec.png"

// Sprites of Players
#define PLAYER_LEFT     "sprite/player_left.png"
#define PLAYER_UP       "sprite/player_up.png"
#define PLAYER_RIGHT    "sprite/player_right.png"
#define PLAYER_DOWN     "sprite/player_down.png"

// Sprites of BOSS
#define BOSS_LEFT     "sprite/boss_left.png"
#define BOSS_UP       "sprite/boss_up.png"
#define BOSS_RIGHT    "sprite/boss_right.png"
#define BOSS_DOWN     "sprite/boss_down.png"

// Sprites of Monsters
#define MONSTER_LEFT     "sprite/monster_left.png"
#define MONSTER_UP       "sprite/monster_up.png"
#define MONSTER_RIGHT    "sprite/monster_right.png"
#define MONSTER_DOWN     "sprite/monster_down.png"

// Sprites of the menu
#define MENU_1 "sprite/Bomber_new_game.png"
#define MENU_2 "sprite/Bomber_load_game.png"
#define MENU_3 "sprite/Bomber_help.png"
#define MENU_4 "sprite/Bomber_quit.png"
#define MENU_5 "sprite/menu_1.png"
#define MENU_6 "sprite/menu_2.png"
#define MENU_7 "sprite/menu_3.png"
#define MENU_8 "sprite/menu_4.png"

// Sprite of  help
#define HELP "sprite/help.png"

//Sprite of win and lose
#define WIN "sprite/victory.png"
#define LOSE "sprite/lose.png"

// banner
SDL_Surface* numbers[10];
SDL_Surface* banner_life;
SDL_Surface* banner_bomb;
SDL_Surface* banner_range;
SDL_Surface* banner_line;

// menu
SDL_Surface* menu[8];

//help
SDL_Surface* help;

//Win and lose
SDL_Surface* win;
SDL_Surface* lose;

// map
SDL_Surface* box;
SDL_Surface* goal;
SDL_Surface* key;
SDL_Surface* door_opened;
SDL_Surface* door_closed;
SDL_Surface* stone;
SDL_Surface* tree;
SDL_Surface* princess;
SDL_Surface* bombox;

// bonus
#define NB_BONUS 4
SDL_Surface* bonus[NB_BONUS + 1];

// player
SDL_Surface* player_img[4];

//boss
SDL_Surface* boss_img[4];

//monster
SDL_Surface* monster_img[4];

//bomb
SDL_Surface* bomb_img[4]; 
SDL_Surface* explosion;

static void menu_load(){
	menu[0] = image_load(MENU_1);
	menu[1] = image_load(MENU_2);
	menu[2] = image_load(MENU_3);
	menu[3] = image_load(MENU_4);
	menu[4] = image_load(MENU_5);
	menu[5] = image_load(MENU_6);
	menu[6] = image_load(MENU_7);
	menu[7] = image_load(MENU_8);
}

static void menu_unload(){
	for (int i=0;i<8;i++){
		SDL_FreeSurface(menu[i]);
	}
}

static void help_load(){
	help=image_load(HELP);
}

static void help_unload(){
	SDL_FreeSurface(help);
}

static void win_load(){
	win=image_load(WIN);
}

static void win_unload(){
	SDL_FreeSurface(win);
}

static void lose_load(){
	lose=image_load(LOSE);
}

static void lose_unload(){
	SDL_FreeSurface(lose);
}

static void banner_load() {
	// numbers imgs
	numbers[0] = image_load(DIGIT_0);
	numbers[1] = image_load(DIGIT_1);
	numbers[2] = image_load(DIGIT_2);
	numbers[3] = image_load(DIGIT_3);
	numbers[4] = image_load(DIGIT_4);
	numbers[5] = image_load(DIGIT_5);
	numbers[6] = image_load(DIGIT_6);
	numbers[7] = image_load(DIGIT_7);
	numbers[8] = image_load(DIGIT_8);
	numbers[9] = image_load(DIGIT_9);

	// other banner sprites
	banner_life = image_load(BANNER_LIFE);
	banner_bomb = image_load(BANNER_BOMB);
	banner_range = image_load(BANNER_RANGE);
	banner_line = image_load(BANNER_LINE);
}

static void banner_unload() {
	// numbers imgs
	for (int i = 0; i < 10; i++) {
		SDL_FreeSurface(numbers[i]);
	}

	// other banner sprites
	SDL_FreeSurface(banner_life);
	SDL_FreeSurface(banner_bomb);
	SDL_FreeSurface(banner_range);
	SDL_FreeSurface(banner_life);
}

static void map_load() {
	// Sprite loading
	tree = image_load(MAP_TREE);
	box = image_load(MAP_CASE);
	key = image_load(MAP_KEY);
	stone = image_load(MAP_STONE);
	door_opened = image_load(MAP_DOOR_OPENED);
	door_closed = image_load(MAP_DOOR_CLOSED);
	princess = image_load(PRINCESS);
	bombox = image_load(BOMBOX);
}

static void map_unload() {
	SDL_FreeSurface(tree);
	SDL_FreeSurface(box);
	SDL_FreeSurface(goal);
	SDL_FreeSurface(key);
	SDL_FreeSurface(stone);
	SDL_FreeSurface(door_opened);
	SDL_FreeSurface(door_closed);
	SDL_FreeSurface(princess);
	SDL_FreeSurface(bombox);
}

static void bonus_load() {
	bonus[0] = NULL;
	bonus[BONUS_BOMB_RANGE_DEC] = image_load(IMG_BONUS_BOMB_RANGE_DEC);
	bonus[BONUS_BOMB_RANGE_INC] = image_load(IMG_BONUS_BOMB_RANGE_INC);
	bonus[BONUS_BOMB_NB_INC] = image_load(IMG_BONUS_BOMB_NB_INC);
	bonus[BONUS_BOMB_NB_DEC] = image_load(IMG_BONUS_BOMB_NB_DEC);
}

static void bonus_unload() {
	for (int i = 0; i < NB_BONUS; i++)
		if(bonus[i])
			SDL_FreeSurface(bonus[i]);
}

static void player_load() {
	player_img[WEST] = image_load(PLAYER_LEFT);
	player_img[EAST] = image_load(PLAYER_RIGHT);
	player_img[NORTH] = image_load(PLAYER_UP);
	player_img[SOUTH] = image_load(PLAYER_DOWN);
}

static void boss_load() {
	boss_img[WEST] = image_load(BOSS_LEFT);
	boss_img[EAST] = image_load(BOSS_RIGHT);
	boss_img[NORTH] = image_load(BOSS_UP);
	boss_img[SOUTH] = image_load(BOSS_DOWN);
}
static void monster_load() {
	monster_img[WEST] = image_load(MONSTER_LEFT);
	monster_img[EAST] = image_load(MONSTER_RIGHT);
	monster_img[NORTH] = image_load(MONSTER_UP);
	monster_img[SOUTH] = image_load(MONSTER_DOWN);
}

static void player_unload() {
	for (int i = 0; i < 4; i++)
		SDL_FreeSurface(player_img[i]);
}

static void boss_unload() {
	for (int i = 0; i < 4; i++)
		SDL_FreeSurface(boss_img[i]);
}
void bomb_load(){
	bomb_img[0] = image_load(BOMB_TTL1);
	bomb_img[1] = image_load(BOMB_TTL2);
	bomb_img[2] = image_load(BOMB_TTL3);
	bomb_img[3] = image_load(BOMB_TTL4);
}
 
void bomb_unload(){
	for (int i = 0; i<4 ; i++)
		SDL_FreeSurface(bomb_img[i]);
}

void explosion_load(){
	explosion=image_load(EXPLOSION);
}

void explosion_unload(){
	SDL_FreeSurface(explosion);
}

static void monster_unload() {
	for (int i = 0; i < 4; i++)
		SDL_FreeSurface(monster_img[i]);
}

void sprite_load() {
	map_load();
	bonus_load();
	banner_load();
	player_load();
	boss_load();
	bomb_load();
	monster_load();
	explosion_load();
	menu_load();
	help_load();
	win_load();
	lose_load();
}

void sprite_free() {
	map_unload();
	bonus_unload();
	banner_unload();
	player_unload();
	boss_unload();
	bomb_unload();
	monster_unload();
	explosion_unload();
	menu_unload();
	help_unload();
	win_unload();
	lose_unload();
}

SDL_Surface* sprite_get_win(){
	return win;
}

SDL_Surface* sprite_get_lose(){
	return lose;
}

SDL_Surface* sprite_get_menu(int i){
	return menu[i];
}

SDL_Surface* sprite_get_help(){
	return help;
}

SDL_Surface* sprite_get_number(short number) {
	assert(number >= 0 && number <= 9);
	return numbers[number];
}

SDL_Surface* sprite_get_player(enum direction direction) {
	assert(player_img[direction]);
	return player_img[direction];
}

SDL_Surface* sprite_get_boss(enum direction direction) {
	assert(boss_img[direction]);
	return boss_img[direction];
}

SDL_Surface* sprite_get_monster(enum direction direction) {
	assert(monster_img[direction]);
	return monster_img[direction];
}

SDL_Surface* sprite_get_banner_life() {
	assert(banner_life);
	return banner_life;
}

SDL_Surface* sprite_get_banner_bomb() {
	assert(banner_bomb);
	return banner_bomb;
}

SDL_Surface* sprite_get_banner_line() {
	assert(banner_line);
	return banner_line;
}

SDL_Surface* sprite_get_banner_range() {
	assert(banner_range);
	return banner_range;
}

SDL_Surface* sprite_get_bonus(enum bonus_type bonus_type) {
	assert(bonus[bonus_type]);
	return bonus[bonus_type];
}

SDL_Surface* sprite_get_bomb(int val) { //ajout du graphique de la bombe
	assert(bomb_img[val]);
	return bomb_img[val];
}

SDL_Surface* sprite_get_explosion(){
	assert(explosion);
	return explosion;
}

SDL_Surface* sprite_get_tree() {
	assert(tree);
	return tree;
}

SDL_Surface* sprite_get_box() {
	assert(box);
	return box;
}
SDL_Surface* sprite_get_princess(){
	assert(princess);
	return princess;
}

SDL_Surface* sprite_get_bombox(){
	assert(bombox);
	return bombox;
}

SDL_Surface* sprite_get_key() {
	assert(key);
	return key;
}

SDL_Surface* sprite_get_stone() {
	assert(stone);
	return stone;
}

SDL_Surface* sprite_get_door_opened() {
	assert(door_opened);
	return door_opened;
}

SDL_Surface* sprite_get_door_closed() {
	assert(door_closed);
	return door_closed;
}
