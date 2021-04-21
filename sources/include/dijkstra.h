#include <map.h>
#include <player.h>

//Met a jour un tableau contenant le chemin jusqu'au joueur et retourne sa longueur
int dijkstra(int* Path,struct map* map, struct player* player, int i, int j);