#include <glib.h>
#include <stdlib.h>
#include <SDL.h>
#include <xmms/plugin.h>
#include <xmmsctrl.h>

#include "palette.h"
#include "def.h"
#include "SDL_thread.h"
#include "struct.h"
#include "jess.h"
#include "draw.h"

extern SDL_Color colors_orig[256][3], colors_used[256];
extern SDL_Surface *screen;
extern struct conteur_struct conteur;
extern int resx, resy, quit_renderer, resolution_change, video;
extern SDL_mutex *mutex_one;

void keyboard(void)
{
  SDL_Event event;

  while(SDL_PollEvent(&event)); /* on prend le dernier event */
    
  switch(event.type)
    {  
    case SDL_KEYDOWN:         
      switch(event.key.keysym.sym) 
	{

/* 	case SDLK_q: */
/* 	  disable_plugin(get_vplugin_info()); */
/* 	  break; */
/* 	case SDLK_k: */
/* 	  printf("Killed (use XMMS plug-in option to quit instead)\n"); */
/* 	  exit (1); */
/* 	  break; */
/* 	case SDLK_RETURN: */
/* 	  printf("Fading palette mode toggled\n"); */
/* 	  conteur.fade_on_off=1-conteur.fade_on_off; */
/* 	  for(i=0;i<256;i++){ */
/* 	    colors_used[i].r=i; */
/* 	    colors_used[i].g=(gint) ( (float) (i*i*i)/(256*256) ); */
/* 	    colors_used[i].b=0; */
/* 	  } */
/* 	  SDL_SetColors(screen, colors_used, 0, 256);       */
/* 	  break; */
	case SDLK_ESCAPE:
	case SDLK_SPACE:
	  printf("Full Screen mode toggled\n");
	  conteur.fullscreen = 1-conteur.fullscreen ;
	  SDL_WM_ToggleFullScreen(screen);     
	  break;
	case SDLK_c:
	  printf("Resolution key 320*180\n");
	  quit_renderer = 1;
	  resolution_change = 1;
	  resx = 320;
	  resy = 180;
	  break;
	case SDLK_v:
	  printf("Resolution key 512*200\n");
	  quit_renderer = 1;
	  resolution_change = 1;
	  resx = 512;
	  resy = 200;
	  break;
	case SDLK_b:
	  printf("Resolution key 640*480\n");
	  quit_renderer = 1;
	  resolution_change = 1;
	  resx = 640;
	  resy = 480;
	  break; 
	case SDLK_n:
	  printf("Resolution key 800*400\n");
	  quit_renderer = 1;
	  resolution_change = 1;
	  resx = 800;
	  resy = 400;
	  break; 
        case SDLK_COMMA:
	  printf("Resolution key 1024x768\n");
	  quit_renderer = 1;
	  resolution_change = 1;
	  resx = 1024;
	  resy = 768;
	  break;
	case SDLK_o:
	  printf("Toggle 32/8bits mode\n");
	  quit_renderer = 1;
	  resolution_change = 1;
	  if (video == 8)
	    video = 32;
	  else
	    video = 8;
	  break; 
	  
	      
	case SDLK_d: 
	  printf("Freeze Image\n"); 
	  conteur.freeze = 1-conteur.freeze; 
	  break; 


	case SDLK_f:
	  printf("Freeze mode\n");
	  conteur.freeze_mode = 1-conteur.freeze_mode;
	  break;
	case SDLK_p:
	  printf("Random palette\n");
	  random_palette();
	  break;
	case SDLK_e:
	  printf("Enable/disable new colors\n");
	  conteur.psy = 1-conteur.psy;
	  break;
	case SDLK_a:
	  printf("Enable/disable Energie analyser\n");
	  conteur.analyser = 1-conteur.analyser;
	  break;

	  /************ Blur mode *************/
	case SDLK_F1:
	  printf("Blur mode 0\n");
	  conteur.blur_mode = 0;
	  break;
	case SDLK_F2:
	  printf("Blur mode 1\n");
	  conteur.blur_mode = 1;
	  break;
	case SDLK_F3:
	  printf("Blur mode 2\n");
	  conteur.blur_mode = 2;
	  break;
	case SDLK_F4:
	  printf("Blur mode 3\n");
	  conteur.blur_mode = 3;
	  break;
	case SDLK_F5:
	  printf("Blur mode 4\n");
	  conteur.blur_mode = 4;
	  break;

	  /************ Draw mode *************/
	case SDLK_F6: /* lignes */
	  printf("Draw mode 5\n");
	  conteur.draw_mode = 6;
	  break;
	case SDLK_F7: /* lignes */
	  printf("Draw mode 5\n");
	  conteur.draw_mode = 5;
	  break;
	case SDLK_F8: /* lignes */
	  printf("Draw mode 4\n");
	  conteur.draw_mode = 4;
	  break;
	case SDLK_F9:
	  printf("Draw mode 0\n");
	  conteur.draw_mode = 0;
	  break;
	case SDLK_F10:
	  printf("Draw mode 1\n");
	  conteur.draw_mode = 1;
	  break;
	case SDLK_F11:
	  printf("Draw mode 2\n");
	  conteur.draw_mode = 2;
	  break;
	case SDLK_F12:
	  printf("Draw mode 3\n");
	  conteur.draw_mode = 3;
	  break;

	  /************* Key binding ************/
	case SDLK_UP:
	  printf("Prev\n");
	  xmms_remote_playlist_prev(0);
	  break;
	case SDLK_DOWN:
	  printf("Next\n");
	  xmms_remote_playlist_next(0);
	  break;
	case SDLK_LEFT:
	  printf("Jump to time -\n");
	  xmms_remote_jump_to_time(0, xmms_remote_get_output_time(0) - 10000);
	  break;
	case SDLK_RIGHT:
	  printf("Jump to time +\n");
	  xmms_remote_jump_to_time(0, xmms_remote_get_output_time(0) + 10000);
	  break;
	  
	case SDLK_u:
	  printf("Color mode : %i\n",conteur.triplet);
	  break;

	//default:
	}   
      break;
   // default:
    }

}
