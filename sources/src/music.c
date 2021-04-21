#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <game.h>

#define MENU_SOUND "music/menu.mp3"

#define EXPLOSION_SOUND "music/explo.wav"
#define WIN_SOUND "music/win_sound.wav"
#define WASTED_SOUND "music/wasted.wav"
#define LEVEL0_SOUND "music/level5.mp3"
#define LEVEL1_SOUND "music/level1.mp3"
#define LEVEL2_SOUND "music/level2.mp3"
#define LEVEL3_SOUND "music/level3.mp3"
#define LEVEL4_SOUND "music/level4.mp3"
#define LEVEL5_SOUND "music/level0.mp3"
#define LEVEL6_SOUND "music/level6.mp3"
#define LEVEL7_SOUND "music/level7.mp3"

Mix_Music* menu_sound;
Mix_Chunk* explosion_sound;
Mix_Chunk* wasted_sound;
Mix_Music* level_sound;
Mix_Chunk* win_sound;
char *SON[8]={LEVEL0_SOUND,LEVEL1_SOUND,LEVEL2_SOUND,LEVEL3_SOUND,LEVEL4_SOUND,LEVEL5_SOUND,LEVEL6_SOUND,LEVEL7_SOUND};

void music_menu_free(){
   Mix_FreeMusic(menu_sound);
}
void music_level_free(){
   Mix_FreeMusic( level_sound);
}

void music_get_menu_theme()
{
   if(Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 1000) == -1) //Initialisation de l'API Mixer
   {
      printf("%s", Mix_GetError());
   }
   menu_sound=Mix_LoadMUS(MENU_SOUND);
   Mix_Volume(0, MIX_MAX_VOLUME);
   Mix_PlayMusic(menu_sound,0);         
}


void music_get_explosion()
{

   if(Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 256) == -1) //Initialisation de l'API Mixer
   {
      printf("%s", Mix_GetError());
   }
   Mix_AllocateChannels(15); // Allouer 2 cannaux 
   Mix_Volume(0, MIX_MAX_VOLUME); // Mets le son a 100% en volume pour le premier cannaux
   Mix_Volume(1, MIX_MAX_VOLUME ); 
   Mix_Volume(2, MIX_MAX_VOLUME); 
   Mix_Volume(3, MIX_MAX_VOLUME ); 
   Mix_Volume(4, MIX_MAX_VOLUME);
   Mix_Volume(5, MIX_MAX_VOLUME ); 
   Mix_Volume(6, MIX_MAX_VOLUME); 
   Mix_Volume(7, MIX_MAX_VOLUME ); 
   Mix_Volume(8, MIX_MAX_VOLUME);
   Mix_Volume(9, MIX_MAX_VOLUME ); 
   Mix_Volume(10, MIX_MAX_VOLUME); 
   Mix_Volume(11, MIX_MAX_VOLUME ); 
   Mix_Volume(12, MIX_MAX_VOLUME);
   Mix_Volume(13, MIX_MAX_VOLUME ); 
   Mix_Volume(14, MIX_MAX_VOLUME); 
   Mix_Volume(15, MIX_MAX_VOLUME ); 
   explosion_sound = Mix_LoadWAV(EXPLOSION_SOUND); 
   Mix_PlayChannel(-1,explosion_sound,0); 

}

void music_get_win(){
   Mix_PauseMusic();
   if(Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 512) == -1) //Initialisation de l'API Mixer
   {
      printf("%s", Mix_GetError());
   }
   Mix_AllocateChannels(2); // Allouer 2 cannaux 
   Mix_Volume(0, MIX_MAX_VOLUME); // Mets le son a 100% en volume pour le premier cannaux
   win_sound = Mix_LoadWAV(WIN_SOUND); 
   Mix_PlayChannel(-1,win_sound,0);
}

void music_get_wasted(){
   Mix_PauseMusic();
   if(Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 512) == -1) //Initialisation de l'API Mixer
   {
      printf("%s", Mix_GetError());
   }
   Mix_AllocateChannels(2); // Allouer 2 cannaux 
   Mix_Volume(0, MIX_MAX_VOLUME); // Mets le son a 100% en volume pour le premier cannaux
   wasted_sound = Mix_LoadWAV(WASTED_SOUND); 
   Mix_PlayChannel(-1,wasted_sound,0); 
}

void music_play_levels_sound(int level){
   int tab_echantillonnage[8]={48000,48000,48000,48000,48000,48000,48000,48000};
   int tab_nb_echantillons[8]={900,900,900,1500,900,900,1024,1024};
   level_sound=Mix_LoadMUS(SON[level]);
   if(Mix_OpenAudio(tab_echantillonnage[level], MIX_DEFAULT_FORMAT, 2, tab_nb_echantillons[level]) == -1) //Initialisation de l'API Mixer
   {
      printf("%s", Mix_GetError());
   }
   Mix_Volume(0, MIX_MAX_VOLUME);
   Mix_PlayMusic(level_sound,-1);
}

void music_play(struct game* game){
   if (!game_get_start(game)){
      music_get_menu_theme();
   }
   if (game_get_start(game)==1 && !Mix_PlayingMusic()){
      music_play_levels_sound(game_get_level(game));
   }
}