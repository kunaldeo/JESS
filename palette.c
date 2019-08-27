#include <SDL.h>
#include <stdlib.h>
#include <math.h>

#include "def.h"
#include "struct.h"
#include "analyser_struct.h"

extern SDL_Color colors_used[256];
extern SDL_Surface *screen;
extern struct conteur_struct conteur;
extern struct analyser_struct lys;

long int random(void);

static const float __magic = 2.0 * PI / 256;

unsigned char courbes_palette(unsigned char i, int no_courbe) {
    /* Optimisation par Karl Soulabaille */
    switch (no_courbe) {
        case 0:
            /*      return (unsigned char) ((float) (i * i * i) / (256 * 256)); */
            return (i * i * i) >> 16;
            break;
        case 1:
            /*    return (unsigned char) ((float) (i * i) / (256)); */
            return (i * i) >> 8;
            break;
        case 2:
            return (unsigned char) i;
            break;
        case 3:
            return (unsigned char) (128 * fabs(sin(__magic * i)));
            break;
        case 4:
            return 0;
            break;
    }
    return 0;
}

void
random_palette(void) {
    int i, j, k, l;
    /*  int ugly [ ] = { 322,514,23,550,413,512,152,504,23,150,414,332,142, */
/* 		230,132,552,231,150,31,521,252,300,324,245,224,542, */
/* 		135,244,411,230,320,135,130,531,141,43,45,400,21,24, */
/* 		144,525,14,25,342,422,42,451,213,512,524,144,505, */
/* 		305,51,35,31,540,243,311,430,42,514,13,403,520,332, */
/* 		424,504,532,404,523,310,541,135,34,123,524,244,41, */
/* 		134,114,304,322,444,413,405,503,331,321,31,235,530, */
/* 		440,241,423,313,515,450,442,340,341,301,410,155,522, */
/* 		351,225,145,251,412,351,415,113,421,155,302,452,240, */
/* 		242,233,410,325,323,441,510,340,315,44,431,151,131, */
/* 	        666,140,516,143,600,255,416,303,50,256,32}; */

    again_mister:;

    if (conteur.psy == 1)
        i = 5;
    else
        i = 3;

    j = rand() % i;
    k = rand() % i;
    l = rand() % i;

    conteur.triplet = j + 10 * k + 100 * l;
/*   for ( i = 0; i <sizeof(ugly); i++) */
/*    { */
/*      if (conteur.triplet == ugly[i]) */
/*        { */
/* 	 printf("Ugly mode dectected: re-randomizing\n"); */
/* 	 goto again_mister; */
/* 	} */
/*    } */

    /*  if (j==k) */
    /*    if(j==l) */
/*       goto again_mister; */

/*   if ((j>2) || (k>2) || (l>2)  ) */
/*     { */
/*       if (random()%20!=0) */
/* 	 goto again_mister; */

/*       if ((j<=2) || (k<=2) || (l<=2) || (j==6) || (k==6) || (l==6)) */
/* 	{ */
/* 	  printf("Ugly mode dectected: re-randomizing\n"); */
/* 	  goto again_mister; */
/* 	} */
/*     } */

    if ((j == k) || (j == l) || (l == k))
        goto again_mister;

    for (i = 0; i < 256; i++) {
        colors_used[i].r = courbes_palette(i, j);
        colors_used[i].g = courbes_palette(i, k);
        colors_used[i].b = courbes_palette(i, l);
    }

    printf("Switch to color mode %i\n", j + 10 * k + 100 * l);
    SDL_SetColors(screen, colors_used, 0, 256);
}











/*
  void
  mix_palette (unsigned char pal_start, unsigned char pal_end, float landa)
  {
  unsigned int c;
  for (c = 0; c < 256; c++)
  {
  colors_used[c].r =
  (unsigned char) colors_orig[c][pal_end].r * landa +
  colors_orig[c][pal_start].r * (1 - landa);
  colors_used[c].g =
  (unsigned char) colors_orig[c][pal_end].g * landa +
  colors_orig[c][pal_start].g * (1 - landa);
  colors_used[c].b =
  (unsigned char) colors_orig[c][pal_end].b * landa +
  colors_orig[c][pal_start].b * (1 - landa);
  
  }
  SDL_SetColors (screen, colors_used, 0, 256);
  }
  
  
  float
  transition (guint noimage, float nbimages, guint no_trans)
  {
  return 0.5 - 0.5 * cos (noimage / nbimages * 3 * PI);
  }
*/
