#include <glib.h>
#include <SDL.h>
#include "struct.h"
#include "analyser_struct.h"
#include "analyser.h"
#include "def.h"

extern struct analyser_struct lys;
extern struct conteur_struct conteur;

gint detect_beat(void)
{

 /*  if (time_last(ONE, NON) > (float) 60.0/BPM_MAX) */
    if ( (lys.E/ lys.E_moyen > LEVEL_ONE )/*  && ( lys.dEdt_moyen>0 ) */)
      {
#ifdef DEBUG
	printf("BEAT\n"/* , 60.0 / time_last(ONE, OUI)  */); 
#endif
	lys.beat = OUI;
      }

  return 0;
}


/* Energie discrete moyenne temporelle*/
void spectre_moyen(gint16 data_freq_tmp[2][256])
{
  int i;
  float aux,N;
  N = T_AVERAGE_SPECTRAL;

  for (i=0 ; i<256 ; i++)
    {
      aux = (float) (data_freq_tmp[0][i] + data_freq_tmp[1][i]) * 0.5 / 65536;
      aux = aux * aux;
      lys.Ed_moyen[i] = (N - 1.0) / N * lys.Ed_moyen[i] + 1.0 / N * aux;

      if (aux / lys.Ed_moyen[i] > 9) /* discret beat */
	lys.dbeat[i] = OUI;
    }

}


/* Derivee temporelle de l'energie moyenne */
/* Doit etre appelle apres C_E_moyen */
void C_dEdt_moyen(void)
{
  static float E_old;
  float new,N;
  
  N = T_AVERAGE_DEDT;
  
  new = (lys.E_moyen-E_old)/lys.dt;
  
  lys.dEdt_moyen = (N - 1.0) / N * lys.dEdt_moyen + 1.0 / N * new; 

  E_old = lys.E_moyen;
}

void C_dEdt(void)
{
  static float E_old;
  float new, N;
  
  N = T_DEDT; /* on met un petit filtre qd meme */
  
  new = (lys.E_moyen-E_old)/lys.dt;
  
  lys.dEdt = (N - 1.0) / N * lys.dEdt_moyen + 1.0 / N * new; 
  
  E_old = lys.E_moyen;
}
  

/* Energie moyenne temporelle */
void C_E_moyen(gint16 data_freq_tmp[2][256])
{
  float N;
  N = T_AVERAGE_E;

  lys.E_moyen = (N - 1.0) / N * lys.E_moyen + 1.0 / N * energy(data_freq_tmp,1);

}



/* Energie courante */
float energy(gint16 data_freq_tmp[2][256], gint type_E)
{
  gint i, tmp;
  float energy_=0;

  for (i=0; i<256; i++)  /* 256*/
    {
      tmp = ( data_freq_tmp[1][i]  ) >> 8 ;
      energy_+= tmp * tmp ;
    }

  energy_ =  energy_ / 65536 / 256 * 256; /*ahahah*/

  lys.E = energy_;

  return energy_;
}




/* REINIT */
float time_last(gint i, gint reinit)
{
  float new_time=SDL_GetTicks(),
    delta_t = (new_time - lys.last_time[i])/1000;

  if (reinit == OUI)
    lys.last_time[i] = new_time;

  return delta_t;
} 



inline void ips(void)
{
  
 
      conteur.dt = time_last(FOUR,NON);
      conteur.fps = (gint) 1/time_last(FOUR,REINIT);

       if (conteur.term_display == OUI) 
	printf("FPS :%i\r", conteur.fps);   
}























/*
void C_E_moyen(void)
{
  gint i,j;

  lys.E_moyen = 0;

  for (i = lys.E_history_index; i < SIZE_HISTORY + lys.E_history_index; i++)
    {
      if (i<SIZE_HISTORY)
	j = i;
      else
	j = i - SIZE_HISTORY;

      lys.E_moyen += lys.E_history[j][TOTAL_E];
    }
  lys.E_moyen /= SIZE_HISTORY ;
}
*/

/*
void C_E_history(gint16 data_freq_tmp[2][256])
{
  lys.E_history_index++;

  if (lys.E_history_index == SIZE_HISTORY)
    lys.E_history_index = 0;

  lys.E_history[lys.E_history_index][TOTAL_E] = energy(data_freq_tmp, TOTAL_E);
}
*/
