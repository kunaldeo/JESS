/*		JESS v2.9.1

         Copyright (c) 2000 by Remi Arquier
 Freely redistributable under the Gnu GPL version 2 or later

                   <arquier@crans.org>
	http://arquier.free.fr
17 dec 2001
*/

#include <xmms/plugin.h>
#include <stdlib.h>
#include <unistd.h>
#include <glib.h>
#include <math.h>
#include <SDL.h>
#include <string.h>  

#include "SDL_thread.h"
#include "def.h"
#include "struct.h"
#include "distorsion.h"
#include "misc.h"
#include "analyser.h"
#include "analyser_struct.h"
#include "renderer.h"
#include "palette.h"
#include "draw_low_level.h"

//void usleep (unsigned long usec); 
int nice(int inc);  
void jess_init (void);
void jess_init_xmms (void);
void jess_cleanup (void);
void init_video_8 (void);
void init_video_32 (void);
static void jess_playback_start (void);
static void jess_playback_stop (void);
static void jess_render_pcm (gint16 data[2][512]);
static void jess_render_freq (gint16 data[2][256]);

VisPlugin jess_vp = {
  NULL,
  NULL,
  0,
  "JESS 2.9.1",
  2,
  2,
  jess_init_xmms,		/* init */
  jess_cleanup,			/* cleanup */
  NULL,				/* about */
  NULL,				/* configure */
  NULL,	                /* disable_plugin */
  jess_playback_start,		/* playback_start */
  jess_playback_stop,		/* playback_stop */
  jess_render_pcm,		/* render_pcm */
				/* jess_render_freq */ 
  jess_render_freq,
};

unsigned int *table1, *table2, *table3, *table4, pitch, quit_renderer, resolution_change, video;
unsigned char dim[256], dimR[256],dimG[256],dimB[256], *bits, bpp, *pixel, *buffer;
int its_first_time/* ,session */;
int resx, resy, xres2, yres2;
struct conteur_struct conteur;
struct analyser_struct lys;

gint16 data[2][512];

SDL_Surface *screen;
SDL_Color colors_used[256];
SDL_Thread *render_thread; 
SDL_mutex *mutex_one;


VisPlugin *
get_vplugin_info (void)
{
  return &jess_vp;
}


void
jess_init_xmms (void)
{
  conteur.burn_mode = 4;
  conteur.draw_mode = 3;
  conteur.blur_mode = 3;
  its_first_time = 1;
  video = 32;
  resolution_change = 0;
  conteur.term_display = OUI;
  resx = RESX;
  resy = RESY;

  printf ("\n\n\n------- JESS First Init ------\n"); 
  printf("\n Create mutex\n");
  mutex_one = SDL_CreateMutex();

  jess_init();

  ball_init();
  
  if (resolution_change == 0)
    {
      render_thread = SDL_CreateThread ( (void *) renderer, NULL, NULL);
      if ( render_thread == NULL) {
	fprintf (stderr, "Pthread_create error for thread renderer\n");
	exit (1);
      }
      printf("Pthread_create renderer passed\n");
    }
}

void
jess_init (void)
{

  /*  quit_keyb = 0; */
  quit_renderer = 0;

  xres2 = resx/2;
  yres2 = resy/2;

  conteur.fullscreen = 0;
  conteur.blur_mode = 1;

  /******* ALLOCATION OF TABLES AND BUFFERS *****************/
  table1 = (unsigned int *) malloc (resx * resy * sizeof (int));
  if (!table1)
    {
      printf ("Not enought memory for allocating tables\n");
      exit (1);
    }
  table2 = (unsigned int *) malloc (resx * resy * sizeof (int));
  if (!table2)
    {
      printf ("Not enought memory for allocating tables\n");
      exit (1);
    }
  table3 = (unsigned int *) malloc (resx * resy * sizeof (int));
  if (!table3)
    {
      printf ("Not enought memory for allocating tables\n");
      exit (1);
    }
  table4 = (unsigned int *) malloc (resx * resy * sizeof (int));
  if (!table4)
    {
      printf ("Not enought memory for allocating tables\n");
      exit (1);
    }
  printf ("Tables created\n");

  printf("Video mode = %d*%d*%d bits\n", resx, resy, video);
  if (video == 8)
    buffer = (unsigned char *) malloc (resx * resy); 
  else
    {
      buffer = (unsigned char *) malloc (resx * resy * 4); 
      printf("Allocating memory done.\n");
    }
  if (!buffer)
    {
      printf ("Not enought memory for allocating buffer\n");
      exit (1);
    }

  printf ("Buffer created\n");
  /**********************************************************/

  /******* TABLES COMPUTATION *******************************/
  create_tables();
  printf("Table computation Ok\n");
  /***********************************************************/

  srand (343425);

  if (video == 8)
    init_video_8();
  else 
    init_video_32();
  


}

void init_video_8(void)
{


  /***************** DISPLAY INITIALISATION ******************/
  if (SDL_Init (SDL_INIT_VIDEO) < 0)
    {
      fprintf (stderr, "SDL Init failed : %s\n", SDL_GetError ());
      exit (1);
    }
  printf("SDL init Ok\n");

  screen = SDL_SetVideoMode (resx, resy, 8, SCREENFLAG);
  if (screen == NULL)
    {
      fprintf (stderr, "Graphic mode is not available: %s\n",
	       SDL_GetError ());
      exit (1);
    }

 

  printf("SDL Setvideo mode Ok\n");

  SDL_ShowCursor(0);
  SDL_EventState (SDL_ACTIVEEVENT, SDL_IGNORE);
  SDL_EventState (SDL_MOUSEMOTION, SDL_IGNORE);

  printf("SDL Event State Ok\n");
  /***************************************************************/

  /******************* COLOR COMPUTATION  ********************/

  random_palette();

  printf("SDL set color Ok\n");

  /****************** INDEX *************************************/
  pitch = screen->pitch;
  printf ("Pitch : %i\n", pitch);
  pixel = ((unsigned char *) screen->pixels);
  bpp = screen->format->BytesPerPixel;
  printf ("Bytes per pixels: %i\n", bpp);
}


void init_video_32(void)
{


  /***************** DISPLAY INITIALISATION ******************/
  if (SDL_Init (SDL_INIT_VIDEO) < 0)
    {
      fprintf (stderr, "SDL Init failed : %s\n", SDL_GetError ());
      exit (1);
    }
  printf("SDL init Ok\n");

  screen = SDL_SetVideoMode (resx, resy, 32, SCREENFLAG);
  if (screen == NULL)
    {
      fprintf (stderr, "Graphic mode is not available: %s\n",
	       SDL_GetError ());
      exit (1);
    }

 

  printf("SDL Setvideo mode Ok\n");

  SDL_ShowCursor(0);
  SDL_EventState (SDL_ACTIVEEVENT, SDL_IGNORE);
  SDL_EventState (SDL_MOUSEMOTION, SDL_IGNORE);

  printf("SDL Event State Ok\n");
  /***************************************************************/

  /******************* COLOR COMPUTATION  ********************/

/*   random_palette(); */

/*   printf("SDL set color Ok\n"); */

  /****************** INDEX *************************************/
  pitch = screen->pitch;
  printf ("Pitch : %i\n", pitch);
  pixel = ((unsigned char *) screen->pixels);
#ifdef DEBUG
  printf("Screen address: %p\n", pixel);
#endif
  bpp = screen->format->BytesPerPixel;
#ifdef DEBUG
  printf ("Bytes per pixels: %i\n", bpp);
#endif
}









void
jess_cleanup (void)
{
  int i=0;
  
  if (resolution_change==0)
    {
      i = 0;
      printf("Send quit information to renderer\n");
      quit_renderer = 1;
      while (quit_renderer)
	{
	  i++;
	  usleep(100000);
	  if (i>10*2)
	    {
	      printf("The renderer dont respond, try to kill it\n");
	      SDL_KillThread(render_thread);
	      quit_renderer = 0;
	    }
	}
      printf("Renderer quitted\n");
  

      printf("Destroy mutex\n");
      SDL_DestroyMutex(mutex_one);
    /*   SDL_DestroyMutex(mutex_two); */
    }

  free (table1);
  free (table2);
  free (table3);
  free (table4);
  free (buffer);
  SDL_FreeSurface(screen);
  printf("Buffer freed\n");

  SDL_Quit ();
  printf("SDL Quit\n");
}

static void
jess_playback_start (void)
{
  conteur.courbe = 0;
  conteur.angle = 0;
  conteur.angle2 = 0;
  conteur.k1 = 0;
  conteur.k2 = 0;
  conteur.k3 = 10000;
 
  conteur.fps = 40;
  
  conteur.mix_reprise = 1000;
  conteur.last_flash = 1000;
  conteur.burn_mode = 2;
  conteur.draw_mode = 5;
  conteur.v_angle2 = 1;
  conteur.general = 0;
  conteur.blur_mode = 3;
  conteur.freeze = 0;
  conteur.freeze_mode = 0;
  

  lys.E_moyen = 0;
  lys.dEdt_moyen = 0;
}







static void
jess_playback_stop (void)
{
  int j;

  for (j = 0; j < resy * resx; j++)
    {
      *(pixel + j) = 0;
    }
  if (conteur.fullscreen == 1)
    {
      SDL_WM_ToggleFullScreen (screen);
      conteur.fullscreen = 1 - conteur.fullscreen;
    }
}









static void
jess_render_pcm (gint16 data_tmp[2][512])
{
  SDL_mutexP(mutex_one);
  memcpy (data, data_tmp, sizeof( data ));
  SDL_mutexV(mutex_one); 
}



static void
jess_render_freq (gint16 data_freq_tmp[2][256])
{
 
  lys.conteur[ZERO]++;
  lys.conteur[ONE]++;

 /*  if ( (lys.conteur[ZERO] % 50) == 0) { */
/*       printf("Appel jess_render_freq %f \n", 50/time_last(ZERO, OUI) ); */
/*     }   */

  lys.dt = time_last(TWO, OUI);

  spectre_moyen(data_freq_tmp); 
  C_E_moyen(data_freq_tmp); 
  C_dEdt_moyen();
  C_dEdt();

#ifdef DEBUG
  if ( (lys.conteur[ONE] % 25) == 0) {
      printf("E %f | dEdt %f  \n", lys.E_moyen, lys.dEdt_moyen); 
    }
#endif
}






