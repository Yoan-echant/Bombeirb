#include <stdio.h>
#include <stdlib.h>

#include <player.h>
#include <map.h>

int supprimer_min(int* H,int l){
	int min =H[1];
	int indice_min=0;
	for (int i=0; i < l; i++){
		if (H[2*i +1] < min){
			min = H[2*i+1];
			indice_min=i;
		}
	}
	int el_min = H[2*indice_min];
	for (int k=indice_min; k < l-1; k++){
		H[2*k]=H[2*k+2];
		H[2*k+1]=H[2*k+3];
	}
	return el_min;
}

void augmenter_prio(int* H,int v,int c, int l){
     int test =0;
     int i =0;
     while (i<l && test ==0){
        if (H[2*i] == v){
            H[2*i+1]=c;
            test = 1;
        }
        i++;
    }
}

void calcul_dist(int* H, int* distance, int* prev, int* L, int u, int v, int l){
    if (distance[v]> distance[u]+ L[u]){
        distance[v] = distance[u] + L[u];
        prev[v] = u;/*
        int x1=v%STATIC_MAP_WIDTH;
        int y1=v/STATIC_MAP_WIDTH;
        int x2=u%STATIC_MAP_WIDTH;
        int y2=u/STATIC_MAP_WIDTH;
        printf("prev de x=%i,y=%i: x=%i, y=%i ie prev %i = %i\n",x1,y1,x2,y2,v,u);
        */
        augmenter_prio(H,v,distance[v],l);
    }
}

int dijkstra(int *Path,struct map* map, struct player* player, int i, int j){
	int w =STATIC_MAP_WIDTH;
	int h = STATIC_MAP_HEIGHT;
    int l = w*h;
    int xp =player_get_x(player);
    int yp =player_get_y(player);
	int* H = (int*)malloc(2*l*sizeof(int)); //File de priorité
	int* distance = (int*)malloc(l*2*sizeof(int));
	int* prev = (int*)malloc(l*20*sizeof(int));
    int* L = malloc(l*2*sizeof(int));
    for (int k=0;k<l*2;k++){
        L[k]=0;
        distance[k]=1;
    }
    for (int k=0;k<l*20;k++){
        prev[k]=-1;
    }
	for (int x=0; x< w;x++){
		for (int y=0; y< h;y++){
			distance[x+y*w] = 50*w*h; // La taille du chemin est forcément < a w*h
			H[2*(x+y*w)]= x+y*w;
			H[2*(x+y*w)+1]=2*w*h;
            int test =0;
            switch(map_get_cell_type(map,x,y)){
                case CELL_SCENERY:
		            L[x+y*w]=2*w*h;
		            break;

	            case CELL_BOX:
		            L[x+y*w]=2*w*h;
		            break;

	            case CELL_BONUS:
		            L[x+y*w]=1;
		            break;
	            case CELL_KEY:
	        	    L[x+y*w]=2*w*h;
	        	    break;
	            case CELL_BOMB:
		            L[x+y*w]=w*h; //Plus long qu'une case vide, mais moins long qu'un obstacle!
		            break;

	            case CELL_MONSTER:
                    L[x+y*w]=1;
		         break;

	            case CELL_EXPLOSION:
                    L[x+y*w]=1;
		            break;
	            case CELL_DOOR:
                    L[x+y*w]=2*w*h;
		            break;
                default:
                    if (x>0){ // Les monstres ne peuvent pas s'approcher des portes!
                        if (map_get_cell_type(map,x-1,y)==CELL_DOOR){
                            L[x+y*w]=2*w*h;
                            test = 1;
                        }
                    }
                    if (x < w-1){
                        if (map_get_cell_type(map,x+1,y)==CELL_DOOR){
                            L[x+y*w]=2*w*h;
                            test = 1;
                        }
                    }
                    if (y>0){
                        if (map_get_cell_type(map,x,y-1)==CELL_DOOR){
                            L[x+y*w]=2*w*h;
                            test = 1;
                        }
                    }
                    if (y < h-1){
                        if (map_get_cell_type(map,x,y+1)==CELL_DOOR){
                            L[x+y*w]=2*w*h;
                            test = 1;
                        }
                    }
                    if (test==0)
                        L[x+y*w]=1;
		            break;
                }
		    }
	    }
    int pos = i + j*w;
    distance[pos]=0;
    augmenter_prio(H,pos,0,l);
    //printf("coord player: %i, %i\n",xp,yp);
    while ((prev[xp +yp*w]==-1 || distance[xp +yp*w] > h*w) && l>=0){
        int u = supprimer_min(H,l);
        l--;
        //printf("case: %i\n",u);
        if (u%w  >0){
            calcul_dist(H,distance, prev,L,u,u-1,l);
            }
        if (u%w + 1 < w){
            calcul_dist(H,distance, prev,L,u,u+1,l);
            }
        if (u/w > 0){
            calcul_dist(H,distance, prev,L,u,u-w,l);
            }
        if (u/w < w){
            calcul_dist(H,distance, prev,L,u,u+w,l);    
            }
    }
    if (l ==0){
        printf("Erreur, pas de chemin trouvé\n");
        return 0;
    }
    if (distance[xp + yp*w]>l)
        return 0;
    int current_cell = xp + yp*w;
    Path[0]=current_cell;
    int l_path=0;
    while (current_cell != i+j*w && l_path < l && current_cell <= 2*l){
        current_cell = prev[current_cell];
        Path[l_path]= current_cell;
        l_path++;
    }
    if (l_path >= h*w)
        return 0;
    return l_path;
}