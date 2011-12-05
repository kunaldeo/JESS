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
#include "draw.h"
#include "misc.h"
#include "jess.h"
#include "analyser.h"
#include "analyser_struct.h"
#include "renderer.h"
#include "palette.h"

extern unsigned int *table1, *table2, *table3, *table4, pitch, quit_renderer, resolution_change,video;
extern unsigned char dim[256],dimR[256],dimG[256],dimB[256], *bits, bpp, *pixel, *buffer;
extern gint16 data[2][512];
extern int resx, resy, xres2, yres2;
extern SDL_Surface *screen;
extern SDL_Color colors_used[256];
extern struct conteur_struct conteur;
extern struct analyser_struct lys;
extern SDL_Thread *render_thread; 
/*extern  SDL_Thread *keybtest_thread;  */
extern SDL_mutex *mutex_one, *mutex_two;
extern int its_first_time/* ,session */;

void draw_mode(int mode)
{

#ifdef DEBUG
  printf("draw: mode = %d\n", mode); 
#endif
  switch (lys.montee)
    {
    case NON: /* bruit calme */
      if (conteur.courbe <= 255 - 35) /* le bruit calme revient */
	conteur.courbe += 32; /* on fait re-apparaitre la courbe */

      if (mode == 0)
	courbes (pixel, data, conteur.courbe,0);
      else if (mode == 1)
	l2_grilles_3d (pixel, data, conteur.angle2 / 200, 0,
		       conteur.angle2 / 30, 200, 130);
      else if (mode == 2)
	burn_3d (pixel, data, conteur.angle2 / 400, 0,
		 conteur.angle2 / 60, 200, 130,conteur.burn_mode);
      else if ((mode == 3) && (conteur.k3 > 700)) /* mode 3 */
	burn_3d (pixel, data, conteur.angle / 200, 0,
		 conteur.angle / 30, 200, 130,conteur.burn_mode);
      else if (mode == 4) /* mode ligne */
	{
	  super_spectral_balls(pixel);
	  	courbes (pixel, data, conteur.courbe,1);
	}
      else if (mode == 6) 
	  super_spectral(pixel);

      else if (mode == 5) /* mode stars */
	stars_manage(pixel, MANAGE, conteur.angle2 / 400, 0,
		     conteur.angle2 / 60, 200, 130);
      break;

    case OUI: /* bruit modere */
      conteur.courbe = 0;
      if (mode == 0)
	grille_3d (pixel, data, conteur.angle / 200, 0, conteur.angle / 30,
		   100, -lys.E_moyen*20 + 130);
      else if (mode == 1)
	l2_grilles_3d (pixel, data, conteur.angle2 / 200, 0,
		       conteur.angle2 / 30, 200, -lys.E_moyen*20 + 130);
      else if (mode == 2)
	burn_3d (pixel, data, conteur.angle2 / 400, 0,
		 conteur.angle2 / 60, 200, 130,conteur.burn_mode);
      else if ((mode == 3) && (conteur.k3 > 700)) /* mode 3 */
	burn_3d (pixel, data, conteur.angle / 200, 0,
		 conteur.angle / 30, 200, 130,conteur.burn_mode);
      else if (mode == 4) /* mode ligne */
	{
	  super_spectral_balls(pixel);
	  courbes (pixel, data, conteur.courbe,1); 
	}
      else if (mode == 6) 
	  super_spectral(pixel);
      else if (mode == 5) /* mode stars */
	stars_manage(pixel, MANAGE,  conteur.angle2 / 400, 0,
		     conteur.angle2 / 60, 200, 130);
      break;

   // default:
    }

  conteur.k3 += 10;
  if (conteur.k3 < 300)  /* Ici c'est les boules qui se barrent */
    burn_3d (pixel, data, conteur.angle2 / 200, 0,
	     conteur.angle2 / 200, 200, -50 + 3 * conteur.k3,conteur.burn_mode);
  


#ifdef DEBUG
  printf("end draw\n");
#endif

  fusee(pixel, MANAGE);;

  on_beat(lys.beat);

  on_reprise();	
  
}

void *renderer (void)
{
 
  gint16 data_tmp[2][512];

  nice (10);

 again:;
  printf("Renderer loop (re)started \n");
  
  while (!quit_renderer)
    {   
#ifdef DEBUG  
 	printf("Debut renderer\n");   
#endif 
      if(conteur.freeze == 0)
	{

	  SDL_mutexP(mutex_one); 
	  memcpy (data_tmp, data, sizeof( data ));
	  SDL_mutexV(mutex_one);

#ifdef DEBUG
	  printf("ips()\n");
#endif
	  ips (); 


#ifdef DEBUG
	  printf("manage_dynamic_and_states_open\n");
#endif
	  manage_dynamic_and_states_open();

#ifdef DEBUG
	  printf("render_deformation\n");
#endif
	  render_deformation(conteur.blur_mode);
	
#ifdef DEBUG
	  printf("render_blur\n");
#endif
	  render_blur(0); 
	
#ifdef DEBUG
	  printf("draw_mode\n");
#endif
	  draw_mode(conteur.draw_mode);

#ifdef DEBUG
	  printf("copy_and_fade\n");
#endif
	  copy_and_fade(DEDT_FACTOR*lys.dEdt_moyen);

	  if (conteur.analyser == 1) 
	    {
#ifdef DEBUG
	      printf("analyser\n");
#endif
	      analyser(pixel);
	    }
#ifdef DEBUG
	  printf("manage_states_close\n");
#endif
	  manage_states_close();
	}

#ifdef DEBUG
      printf("SDL_UpdateRect\n");
#endif
     SDL_UpdateRect (screen,0,0,resx,resy);
     
#ifdef DEBUG
     printf("keyboard\n");
#endif
     keyboard ();

    }

  printf("Renderer Ok je quitte\n");
  quit_renderer = 0;
  
  if(resolution_change == 1)
    {
      jess_cleanup();
      jess_init();
      resolution_change = 0;
      goto again;
    }
 
  return NULL;
}

void manage_dynamic_and_states_open(void)
{
  conteur.general++;
  conteur.k2++;
  conteur.last_flash++;

  conteur.angle += conteur.dt * 50;

  conteur.v_angle2 = 0.97 * conteur.v_angle2 ;
  conteur.angle2 += conteur.v_angle2 * conteur.dt ;

  detect_beat()  ;

  if (lys.dEdt_moyen > 0)
    lys.montee = OUI;

  if ((lys.montee == OUI) && (lys.beat == OUI))
    lys.reprise = OUI ;

}

void manage_states_close(void)
{
  lys.beat = NON ;
  lys.montee = NON;
  lys.reprise = NON;
}

void on_beat(int beat)
{
  if (lys.beat == OUI)
    {

    /*   if( conteur.draw_mode !=4)  */
	fusee(pixel,NEW);

      /* on fou des etoiles */
/*       if (conteur.draw_mode != 2 && conteur.draw_mode != 3)     */
/* 	sinus_rect (pixel, conteur.k1); */
      conteur.k1 += 4;

      /* vitesse a l angle 2 */
      conteur.v_angle2 += (rand () % 2 - 0.5) * 16 * 32; 

      if (conteur.draw_mode == 3)
	conteur.k3 = 0;
      
      if(conteur.draw_mode == 5)
	stars_manage(pixel, NEW, conteur.angle2 / 400, 0,
		     conteur.angle2 / 60, 200, 130);
    }
}

void on_reprise(void)
{
  unsigned int j;
  unsigned char *pix = pixel;

  if (lys.reprise == OUI ) 
    {
   
	  	    
      if (conteur.last_flash > 5 * conteur.fps) /* si y  pas eu de flash depuis longtemps: flash */
	{
	   if(conteur.draw_mode == 5)
	stars_manage(pixel, NEW_SESSION, conteur.angle2 / 400, 0,
		   conteur.angle2 / 60, 200, 130);	

	  /* FLASH */
#ifdef DEBUG
	  printf("Flash\n");
#endif
	  pix = pixel;
	  for (j = 0; j < resy * pitch; j++)
	      *(pix++) = 250;  
      
	  if (conteur.freeze_mode == NON) /* si c pas freeze_mode on change tout */
	    {      
	      conteur.burn_mode = rand() % 4;      
	      
	      conteur.draw_mode = rand () % 7;    
	   
	      conteur.blur_mode = rand () % 5 ; 
	      if(conteur.draw_mode==2)
		conteur.blur_mode=0;
	  
	      random_palette();  
	    }
	  conteur.last_flash = 0;
	}


      else /* il y a eu un flash y a pas longtemps, donc on fait juste des etoiles */
	{



	  /* on change de mode blur */
	  if ((conteur.freeze_mode == 0) && (conteur.mix_reprise >5) && (conteur.draw_mode!=2) ) /* si c pas freeze_mode on change tout */
	    {
	      conteur.blur_mode = rand () % 5 ; 
	    }
	}

#ifdef DEBUG
      if (conteur.term_display == OUI)
	printf("Blur m %i | Draw m %i | Burn m %i\n",conteur.blur_mode, conteur.draw_mode, conteur.burn_mode);
#endif
      
      /*   conteur.mix_reprise = 0; */
    }
  
  /*  if (conteur.mix_reprise < 2 * conteur.fps) */
  /*       conteur.mix_reprise++; */
}








void copy_and_fade(float factor)
{
  unsigned int j;
  unsigned char *pix, *buf;

  
  buf = buffer;
  pix = pixel;

  if(video == 8)    
    {
      fade(factor, dim);
      for (j = 0; j <  resy * resx; j++)
	{	    
	  *(buf++) = dim[*(pix++)];
	}
    }
  else
    {
      fade(cos(0.125*factor)*factor*2, dimR);
      fade(cos(0.25*factor)*factor*2, dimG);
      fade(cos(0.5*factor)*factor*2, dimB);

      for (j = 0; j <  resy * resx; j++)
	{	    
	  *(buf++) = dimR[*(pix++)];
	  *(buf++) = dimG[*(pix++)];
	  *(buf++) = dimB[*(pix++)];
	  buf++;
	  pix++;
	  
      }
    }
}


void fade(float variable, unsigned char * dim)
{
  unsigned int aux2,j ;
  float aux;
  
  aux = 1-exp(-fabs(variable));
  /*     aux2 = (int) (((ifl * ifl * 1000*aux) / (256 * 4)  + 0*(1-aux) * j / 4)); */
  if (aux>1)
    aux=1;
  if (aux<0)
    aux=0;

  for (j= 0; j < 256; j++)
    {
   
      aux2 = (unsigned char) ((float) j * 0.245 * aux);

      if (aux2>255)
	aux2=255;
      if (aux2<0)
	aux2=0;

      dim[j]= aux2;
      /*      dim2[i] = (unsigned char) ((float) i * 0.245); */
    }
}
 
void render_deformation(int defmode)
{
   unsigned int bmax, *tab1, *tab2, *tab3, *tab4, i;
   unsigned char *pix = pixel , *buf = buffer, *aux;
   

   SDL_LockSurface(screen);

   /**************** BUFFER DEFORMATION ****************/

 
   
   if (video == 8)
     {
       buf = buffer;
       tab1 = table1;
       tab2 = table2;
       tab3 = table3;
       tab4 = table4;
       bmax = resx * resy + (unsigned int) pixel;

       switch(defmode)
	 {
	 case 0:
	   memcpy(pixel,buffer,resx * resy);
	   break;
	 case 1:
	   for (pix = pixel; pix < (unsigned char *) bmax ; pix++)
	     *pix = *(buffer + *(tab1++)) ;
	   break;
	 case 2:
	   for (pix = pixel; pix < (unsigned char *) bmax; pix++)
	     *pix = *(buffer + *(tab2++)) ;
	   break;
	 case 3:
	   for (pix = pixel; pix < (unsigned char *) bmax; pix++)
	     *pix = *(buffer + *(tab3++)) ;
	   break;
	 case 4:
	   for (pix = pixel; pix < (unsigned char *) bmax; pix++)
	     *pix = *(buffer + *(tab4++)) ;
	   break;   
	 default:
	   printf("Problem with blur_mode\n");
	 }
     }
   else
     {
       pix = pixel;
    
       bmax = resx * resy;
       switch(defmode)
	 {
	 case 0:
	   memcpy(pixel, buffer, pitch * resy);
	   goto nodef;
	   break;
	 case 1:
	   tab1 = table1;
	   break;
	 case 2:
	   tab1 = table2;
	   break;
	 case 3:
	   tab1 = table3;
	   break;
	 case 4:
	   tab1 = table4;
	   break;

	 default:
	   printf("Problem with blur_mode\n");
	 }
       for (i = 0; i < resy*resx; i++)
	 {
	   aux  =  (unsigned char *) ((*(tab1) << 2 ) + (unsigned int) buffer);
	   *pix = *(aux) ;
	   pix++;
	   *pix = *(aux + 1);  
	   pix++;
	   *pix = *(aux + 2);  
	   pix+=2;
	   tab1++;
	 }
       nodef:;
       
     }


   SDL_UnlockSurface(screen);
}

void render_blur(int blur)
{

  /***************** Annotation par Karl Soulabaille:     ***/
  /* Quel est la valeur d'initialisation de pix ? */
  /* (d'ou le segfault) */
  /* j'ai mis pixel par defaut... */

  unsigned char *pix = pixel;
  unsigned int bmax,pitch_4;

  pix = pixel;
  if (pixel == NULL) return;
 /*  goto non; */
#ifdef DEBUG
  printf("Render_blur(Pixel = %p)\n", pix);
#endif

  /* Annotation par Karl Soulabaille: */
  /* Il y avait des overflows sur les boucles (indice supérieur trop élevé de 1) */

  if (video == 8)
    {
      bmax = resx * (resy-1) + (unsigned int) pixel;
      for (pix = pixel; pix < (unsigned char *) bmax-1; pix++)
	{
	  *pix += *(pix+1) + *(pix+resx) + *(pix+resx+1); 
	}
    }
  else
    {
      pitch_4 = pitch+4;
      bmax = pitch*(resy-1) + (unsigned int) pixel;
      for (pix = pixel; pix < (unsigned char *) bmax-4; )
	{
	  *pix += *(pix + 4) + *(pix + pitch) + *(pix + pitch_4); 
	  pix++;
	  *pix += *(pix + 4) + *(pix + pitch) + *(pix + pitch_4);   
	  pix++;
	  *pix += *(pix + 4) + *(pix + pitch) + *(pix + pitch_4);  
	  pix++;
 	  pix++; 
	}
    }
  non:;
}
