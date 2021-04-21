/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#ifndef CONSTANT_H_
#define CONSTANT_H_

#define WINDOW_NAME "[PG110] Bombeirb 2020-2021 / Charles Guitton / Echantillon Yoan"

// Size (# of pixels) of a cell of the map
#define SIZE_BLOC       40

// Size (# of pixels) of banner
#define LINE_HEIGHT	4
#define BANNER_HEIGHT	40

// Time management
#define DEFAULT_GAME_FPS 30

// map size
#define STATIC_MAP_WIDTH  12
#define STATIC_MAP_HEIGHT 12

enum direction {
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3
};
// Constant useful for the boss
#define BOSS_LIVES 10
#define BOSS_PHASE2 7
#define BOSS_PHASE3 4
#define NB_BOMB_BOSS_MAX 3 //nb de bombes max envoyé par le boss dans les phases
#define NB_BOMB_BOSS 3 //nb de bombes envoyé à chaque phase
#define NB_BOMB_BOSS_2 3
#define NB_BOMB_BOSS_3 1
#define HEIGHT_BOMB 2 //nb de case horizontale et verticale autour du joueur ou le boss cherche a lancer ses bombes
#define WIDTH_BOMB 2
#define VITESSE_BOMB 8	//vitesse a laquelle se déplace les bombes du boss en 1/4 case/s

//Nombre de monstres maximum par niveaux:
#define NB_MONSTER_MAX 10


//Fonction carré
#define square(a)  (a)*(a)

//Fonction donnant l'intervalle de temps moyen entre deux mouvement des monstres/boss
#define delay_monstre(level) (750)+(1000/(1+level))

//Fonction donnant la distance de détection des monstres en fonction du niveaux (cette distance est infini au dernier niveau dans tt les cas)
#define DIST_DETECTION(level) (5+(level))/(2)
#endif /* CONSTANT */
