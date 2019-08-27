#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "def.h"
#include "draw.h"
#include "struct.h"
#include "analyser_struct.h"
#include "palette.h"
#include "draw_low_level.h"
#include "projection.h"

extern int resx, resy, xres2, yres2, video;
extern struct conteur_struct conteur;
extern struct analyser_struct lys;


/*******************************************************************/
/**************** ANALYSER *****************************************/
/*******************************************************************/

void analyser(unsigned char *buffer) {
    int i;

    droite(buffer, -xres2, 0, -xres2 + 10, 0, 30);

    for (i = -xres2; i < -xres2 + 5; i++) {
        droite(buffer, i, 0, i, (int) (lys.E_moyen * 2000), 250);
        droite(buffer, i + 5, 0, i + 5, (int) (lys.dEdt_moyen * 25000), 230);
        droite(buffer, i + 10, 0, i + 10, (int) (lys.dEdt * 25000), 200);
    }
}

/*******************************************************************/
/**************** FUSEES *******************************************/
/*******************************************************************/

void fusee(unsigned char *buffer, int new) {
    int i;
    int x = 0, y = 0;
    static int xi[FUSEE_MAX], yi[FUSEE_MAX];
    static float life[FUSEE_MAX];
    float factor;

    if (new == NEW) /* on creer une nouvelle fusee*/
    {
        i = 0;
        while (life[i] > 0) {
            i++;
            if (i == FUSEE_MAX + 1)
                goto quit; /* pas de place desole */
        }
        xi[i] = rand() % resx - xres2;
        yi[i] = -rand() % yres2;
        life[i] = FUSEE_VIE; /* on donne la vie max*/
    } else  /* on gere les autres */
    {
        for (i = 0; i < FUSEE_MAX; i++)
            if (life[i] > 0) /* si elle est vivante */
            {
                factor = life[i] / FUSEE_VIE;
                x = xi[i] * factor;
                y = yi[i] * factor;
                life[i]--;
                ball(buffer, x, y, (int) FUSEE_RAYON * factor, FUSEE_COLOR);
            }
    }
    quit:;
}

/*******************************************************************/
/**************** SUPER SPECTRAL ***********************************/
/*******************************************************************/

void super_spectral_balls(unsigned char *buffer) {
    int i, j;
    static float life[256][LINE_MAX];
    static float x[256][LINE_MAX], y[256][LINE_MAX];
    static float vx[256][LINE_MAX], vy[256][LINE_MAX];

    float dt = conteur.dt;

    for (i = 0; i < 256; i++) /* pour chaque frequence */
    {
        if (lys.dbeat[i] == NEW) /*creation d'une nouvelle ligne */
        {
            lys.dbeat[i] = MANAGE;
            j = 0;
            while (life[i][j] > 0) {
                j++;
                if (j == LINE_MAX + 1)
                    goto quit; /* pas de place desole */
            }
            /* initialisation de la ligne */
            life[i][j] = LINE_VIE;
            vx[i][j] = RESFACTXF(0.025 * ((float) i - 128.0) * 32 + 0 * (1 - (float) rand() / RAND_MAX));
            vy[i][j] = RESFACTYF((10 + i) * i * lys.Ed_moyen[i] * 5000 * ((float) j + 1) / 4);
            x[i][j] = RESFACTXF(2 * (i - 128)) + ((float) j * (i - 128)) / 2;
            y[i][j] = 0 * RESFACTXF(yres2 / 2 - (float) ((i - 128) * (i - 128)) / 256) - 20 * j;
        }
        quit:; /* pas de nouvelle ligne */
        for (j = 0; j < LINE_MAX; j++)
            if (life[i][j] > 0) {

                vy[i][j] += -0.5 * dt * 1024;
                x[i][j] += vx[i][j] * dt;
                y[i][j] += vy[i][j] * dt;

                boule(buffer, (int) x[i][j], (int) y[i][j], 5, 250 * (LINE_VIE - life[i][j]) / LINE_VIE);


                if ((y[i][j] < resy) && (y[i][j] > -resy)) /*oui !*/
                {
                    /* 	  droite (buffer,0 ,(int) 0*y[i][j]/8, (int) x[i][j], (int) y[i][j], 50*(LINE_VIE-life[i][j])/LINE_VIE); */
                    if (i > 128)
                        droite(buffer, xres2, (int) y[i][j] / 32, (int) x[i][j], (int) y[i][j],
                               50 * (LINE_VIE - life[i][j]) / LINE_VIE);
                    else
                        droite(buffer, -xres2, (int) y[i][j] / 32, (int) x[i][j], (int) y[i][j],
                               50 * (LINE_VIE - life[i][j]) / LINE_VIE);
                }
                life[i][j]--;
            }
    }
}

/*******************************************************************/
/**************** SUPER SPECTRAL ***********************************/
/*******************************************************************/

void super_spectral(unsigned char *buffer) {
    int i, j;
    static float life[256][LINE_MAX];
    static float x[256][LINE_MAX], y[256][LINE_MAX];
    static float vx[256][LINE_MAX], vy[256][LINE_MAX];
    static float theta[256][LINE_MAX], omega[256][LINE_MAX];
/*   float dt = 1/conteur.fps; */
    float nx, ny;
    float taille, dt = conteur.dt;

    for (i = 0; i < 256; i++) /* pour chaque frequence */
    {
        if (lys.dbeat[i] == NEW) /*creation d'une nouvelle ligne */
        {
            lys.dbeat[i] = MANAGE;
            j = 0;
            while (life[i][j] > 0) {
                j++;
                if (j == LINE_MAX + 1)
                    goto quit; /* pas de place desole */
            }
            /* initialisation de la ligne */
            life[i][j] = LINE_VIE;
            vx[i][j] = 0 * RESFACTXF(0.025 * ((float) i - 128.0) * 32 + (float) rand() / RAND_MAX * 60);
            vy[i][j] = 0 * RESFACTYF(64 + (float) rand() / RAND_MAX * 64);
            x[i][j] = RESFACTXF(2 * (i - 128)) + ((float) j * (i - 128)) / 2;
            y[i][j] = 0 * RESFACTXF(yres2 / 2 - (float) ((i - 128) * (i - 128)) / 256) - 20 * j + 60;
            theta[i][j] = 0;
            omega[i][j] = (10 + i) * i * lys.Ed_moyen[i] * 32;
        }
        quit:; /* pas de nouvelle ligne */
        for (j = 0; j < LINE_MAX; j++)
            if (life[i][j] > 0) {
                theta[i][j] += omega[i][j] * dt;
                vy[i][j] += -0.5 * dt * 1024 * 0;
                x[i][j] += vx[i][j] * dt;
                y[i][j] += vy[i][j] * dt;
                taille = RESFACTXF(70) * (0 + 2 * (LINE_VIE - life[i][j])) / LINE_VIE * (j + 1) / 6;
                nx = taille * sin(theta[i][j]);
                ny = taille * cos(theta[i][j]);
/* 	      droite (buffer, (int) x[i][j]+nx, (int) y[i][j]+ny, (int) x[i][j]-0*nx, (int) y[i][j]-0*ny, 50*(life[i][j])/LINE_VIE); */
/* 	      cercle_32 (buffer, (int) x[i][j]+nx, (int) y[i][j]+ny,(int) ( (float) 5*(life[i][j]) / LINE_VIE), 150*(life[i][j])/LINE_VIE); */
/* 	      cercle_32 (buffer, (int) x[i][j]-0*nx, (int) y[i][j]-0*ny,(int) ( (float) 5*(life[i][j]) / LINE_VIE) , 150*(life[i][j])/LINE_VIE); */
/* 	      droite (buffer, (int) x[i][j]+nx, (int) y[i][j]+ny, (int) x[i][j]-0*nx, (int) y[i][j]-0*ny, 50*(LINE_VIE-life[i][j])/LINE_VIE); */
/* 	      cercle_32 (buffer, (int) x[i][j]+nx, (int) y[i][j]+ny,(int) ( (float) 5*(LINE_VIE-life[i][j]) / LINE_VIE), 150*(LINE_VIE-life[i][j])/LINE_VIE); */
/* 	      cercle_32 (buffer, (int) x[i][j]-0*nx, (int) y[i][j]-0*ny,(int) ( (float) 5*(LINE_VIE-life[i][j]) / LINE_VIE) , 150*(LINE_VIE-life[i][j])/LINE_VIE); */

                droite(buffer, (int) x[i][j] + nx, (int) y[i][j] + ny, (int) x[i][j], (int) y[i][j],
                       50 * (LINE_VIE - life[i][j]) / LINE_VIE);

                if (video == 8)
                    cercle(buffer, (int) x[i][j] + nx, (int) y[i][j] + ny, 3 * j,
                           150 * (LINE_VIE - life[i][j]) / LINE_VIE);
                else
                    cercle_32(buffer, (int) x[i][j] + nx, (int) y[i][j] + ny, 3 * j,
                              150 * (LINE_VIE - life[i][j]) / LINE_VIE);
/* 	      cercle_32 (buffer, (int) x[i][j], (int) y[i][j], 3*j , 150*(LINE_VIE-life[i][j])/LINE_VIE); */

                life[i][j]--;
            }
    }
}


/*******************************************************************/
/**************** GRANDE GRILLE ************************************/
/*******************************************************************/

void grille_3d(unsigned char *buffer, gint16 data[2][512], float alpha, float beta,
               float gamma, int persp, int dist_cam) {

    float x, y, z, xres2 = (float) (resx >> 1), yres2 = (float) (resy >> 1);
    gint16 ax = 0, ay = 0, ix, iy, i, j, nb_x, nb_y;
    /*  int tx[32][32], ty[32][32]; */
    unsigned char color/* [32][32] */;
    nb_x = 32;
    nb_y = 32;

    for (i = 0; i < nb_x; i++) {
        /* Optimisation par Karl Soulabaille. Factorisation et expressions communes. */

        for (j = 0; j < nb_y; j++) {
            x = RESFACTXF ((i - ((float) nb_x) / 2) * 10);
            y = RESFACTYF ((j - ((float) nb_y) / 2) * 10);

            if (j >= 16) {
                z = RESFACTXF ((float) (data[0][i + nb_x * (j - 16)]) / 256);
                color/* [i][j] */ = data[0][i + nb_x * (j - 16)] / 512 + 100;
            } else {
                z = RESFACTXF ((float) (data[1][i + nb_x * j]) / 256);
                color/* [i][j] */ = (data[1][i + nb_x * j]) / 512 + 100;
            }

            rotation_3d(&x, &y, &z, alpha, beta, gamma);
            perspective(&x, &y, &z, persp, dist_cam);

            if (x >= xres2) {
                x = xres2 - 1;
                color/* [i][j] */ = 0;
            }
            if (x <= -xres2) {
                x = -xres2 + 1;
                color/* [i][j] */ = 0;
            }
            if (y >= yres2) {
                y = yres2 - 1;
                color/* [i][j] */ = 0;
            }
            if (y <= -yres2) {
                y = -yres2 + 1;
                color/* [i][j] */ = 0;
            }

            ix = (int) x;
            iy = (int) y;

/* 	  tx[i][j] = ix; */
/* 	  ty[i][j] = iy; */

            if (j != 0)
                droite(buffer, ix, iy, ax, ay, color/* [i][j] */);

            ax = ix;
            ay = iy;
        }
    }

}




/*******************************************************************/
/**************** 2 GRILLES JUMELLES *******************************/
/*******************************************************************/

void l2_grilles_3d(unsigned char *buffer, gint16 data[2][512], float alpha, float beta,
                   float gamma, int persp, int dist_cam) {

    float x, y, z, xres4 = (float) (resx >> 2);
    float fyres2 = yres2;
    gint16 ax = 0, ay = 0, ix, iy, i, j, nb_x, nb_y;
    int tx[16][16], ty[16][16];
    unsigned char color[16][16];
    nb_x = 16;
    nb_y = 16;

    for (i = 0; i < nb_x; i++) {
        /* Optimisation par Karl Soulabaille. Factorisation de calcul & expression commune */
        x = RESFACTXF ((i - ((float) nb_x / 2)) * 15);
        for (j = 0; j < nb_y; j++) {
            y = RESFACTYF ((j - ((float) nb_y / 2)) * 15);

            z = abs(RESFACTXF ((float) (data[1][i + nb_x * j]) / 256));
            color[i][j] = data[1][i + nb_x * j] / 512 + 100;

            rotation_3d(&x, &y, &z, alpha, beta, gamma);
            perspective(&x, &y, &z, persp, dist_cam);

/* 	  if (x >= xres4) */
/* 	    { */
/* 	      x = xres4 - 1; */
/* 	      color[i][j] = 0; */
/* 	    } */
/* 	  if (x <= -xres4) */
/* 	    { */
/* 	      x = -xres4 + 1; */
/* 	      color[i][j] = 0; */
/* 	    } */
/* 	  if (y >= fyres2) */
/* 	    { */
/* 	      y = fyres2 - 1; */
/* 	      color[i][j] = 0; */
/* 	    } */
/* 	  if (y <= -fyres2) */
/* 	    { */
/* 	      y = -fyres2 + 1; */
/* 	      color[i][j] = 0; */
/* 	    } */


            ix = (int) x;
            iy = (int) y;

            tx[i][j] = ix;
            ty[i][j] = iy;

            if (j != 0) {
                droite(buffer, ix - xres4, iy, ax - xres4, ay, color[i][j]);
                droite(buffer, ix + xres4, iy, ax + xres4, ay, color[i][j]);
            }

            ax = ix;
            ay = iy;
        }
    }

/*   for (j = 0; j < nb_y; j++) */
/*     { */
/*       for (i = 0; i < nb_x - 1; i++) */
/* 	{ */
/* 	  droite (buffer, tx[i][j] - xres4, ty[i][j], tx[i + 1][j] - xres4, */
/* 		  ty[i + 1][j], color[i][j]); */
/* 	  droite (buffer, tx[i][j] + xres4, ty[i][j], tx[i + 1][j] + xres4, */
/* 		  ty[i + 1][j], color[i][j]); */
/* 	} */
/*     } */
}

/*******************************************************************/
/**************** BURN 3D ******************************************/
/*******************************************************************/

void burn_3d(unsigned char *buffer, gint16 data[2][512], float alpha, float beta,
             float gamma, int persp, int dist_cam, int mode) {
#ifdef DEBUG
    printf("Burn3D\n");
#endif
    float x, y, z, xres2 = (float) (resx >> 1), yres2 = (float) (resy >> 1);
    gint16 ix, iy, i, j, nb_x, nb_y, color, taille;

    /* Variable temporaire pour la factorisation de constantes */
    float temp1, temp2, temp3, temp3b, temp4, temp4b, temp5, temp6, temp7, temp8, temp9;

    nb_x = 12;
    nb_y = 12;

#ifdef DEBUG
    printf("Entrer burn 3d\n");
#endif

    /* 4 cas:  */
    switch (mode) {
        case 0:

            temp1 = 2 * PI / nb_x * cos(alpha);
            temp2 = 2 * PI / nb_y;
            for (i = 0; i < nb_x; i++) {
                temp3 = (float) ((int) (i * i));
                for (j = 0; j < nb_y; j++) {
                    x = RESFACTXF ((float) 25 * (i + 1) * cos(temp3 * temp1 + temp2 * j));
                    y = RESFACTYF ((float) 25 * (i + 1) * sin(temp3 * temp1 + temp2 * j));
                    z = RESFACTXF (40 * cos(5 * alpha));
                    rotation_3d(&x, &y, &z, alpha, beta, gamma);
                    perspective(&x, &y, &z, persp, dist_cam);
                    if (!(x >= xres2 || x <= -xres2 || y >= yres2 || y <= -yres2)) {
                        ix = (int) x;
                        iy = (int) y;

                        color = ((int) (float) (100 + 0.4 * z));
                        if (color < 0)
                            color = 0;
                        taille = color / 8;

                        /*		printf("Appel boule\n"); */
                        boule/*_rand*/ (buffer, ix, iy, taille, color);
                    }
                }
            }
            break;
        case 1:
            temp1 = PI / (nb_x * nb_x * nb_x) * fabs(lys.dEdt_moyen * 5000);
            temp6 = 2 * PI / nb_y;
            for (i = 0; i < nb_x; i++) {
                temp3 = (float) (i * i * i);
                temp4 = sin((i + 1) * PI / nb_x);
                temp5 = 5 * alpha * 2 * PI / nb_x * i;
                temp3b = temp3 * temp1;
                temp4b = temp4 * temp1;

                for (j = 0; j < nb_y; j++) {
                    x = RESFACTXF ((float) 50 * (temp3b + temp4 * cos(temp5 + temp6 * j)));
                    y = RESFACTYF ((float) 50 * (temp4b + temp4 * sin(temp5 + temp6 * j)));
                    z = RESFACTXF ((float) 100 * cos((float) i / nb_x * PI) * (1 + lys.dEdt_moyen * 1000));
                    rotation_3d(&x, &y, &z, alpha, beta, gamma);
                    perspective(&x, &y, &z, persp, dist_cam);
                    if (!(x >= xres2 || x <= -xres2 || y >= yres2 || y <= -yres2)) {
                        ix = (int) x;
                        iy = (int) y;

                        color = ((int) (float) (100 + 0.4 * z));
                        if (color < 0)
                            color = 0;
                        taille = color / 8;

                        /*	      printf("Appel boule\n"); */
                        boule/*_rand*/ (buffer, ix, iy, taille, color);
                    }
                }
            }
            break;
        case 2:
            temp5 = 2 * PI / (5 * nb_x);
            temp6 = 2 * PI / nb_y;
            for (i = 0; i < nb_x; i++) {
                temp1 = sin((i + 1) * PI / nb_x);
                temp2 = temp5 * -i;

                for (j = 0; j < nb_y; j++) {
                    x = RESFACTXF ((float) 130 * temp1 * cos(temp2 + j * temp6));
                    y = RESFACTYF ((float) 130 * temp1 * sin(temp2 + j * temp6));
                    z = -RESFACTXF (130 * cos((float) i / nb_x * PI) * lys.dEdt_moyen * 1000);
                    rotation_3d(&x, &y, &z, alpha, beta, gamma);
                    perspective(&x, &y, &z, persp, dist_cam);
                    if (!(x >= xres2 || x <= -xres2 || y >= yres2 || y <= -yres2)) {
                        ix = (int) x;
                        iy = (int) y;

                        color = ((int) (float) (100 + 0.4 * z));
                        if (color < 0)
                            color = 0;
                        taille = color / 8;

                        /*		printf("Appel boule\n"); */
                        boule/*_rand*/ (buffer, ix, iy, taille, color);
                    }
                }
            }
            break;
        case 3:
            temp5 = 25;
            temp6 = ((float) (-2)) * PI / (10 * nb_x);
            temp8 = (float) (2) * PI / nb_y;
            temp9 = 10 * alpha;
            for (i = 0; i < nb_x; i++) {
                temp5 += 25;
                temp7 = -i * temp6;
                for (j = 0; j < nb_y; j++) {
                    x = RESFACTXF ((float) temp5 * cos(temp7 + temp8 * j));
                    y = RESFACTYF ((float) temp5 * sin(temp7 + temp8 * j));
                    z = RESFACTXF (60 * (cos(temp8 * j + temp9) + cos(-temp7)));
                    rotation_3d(&x, &y, &z, alpha, beta, gamma);
                    perspective(&x, &y, &z, persp, dist_cam);
                    if (!(x >= xres2 || x <= -xres2 || y >= yres2 || y <= -yres2)) {
                        ix = (int) x;
                        iy = (int) y;

                        color = ((int) (float) (100 + 0.4 * z));
                        if (color < 0)
                            color = 0;
                        taille = color / 8;

                        /*printf("Appel boule\n"); */
                        boule/*_rand*/ (buffer, ix, iy, taille, color);
                    }
                }
            }
            break;
            //default:
    }
    /*  printf("  Appel rotation_3d\n");
    printf("  Appel perspective\n");
    */



    /***********/
/*      for (i = 0; i < nb_x; i++) */
/*        { */
/*        for (j = 0; j < nb_y; j++) */
/*  	{ */
/*  	  switch(mode) */
/*  	    { */
    /*  	    case 0: /* galaxie z commande par alpha et spirale par alpha */
/*  	      x = RESFACTXF ((float) 25*(i+1)*cos(i*2*PI/nb_x*cos(alpha)*i+ j*2*PI/nb_y) ); */
/*  	      y = RESFACTYF ((float) 25*(i+1)*sin(i*2*PI/nb_x*cos(alpha)*i+ j*2*PI/nb_y) ); */
/*  	      z = RESFACTXF (40*cos(5*alpha)); */
/*  	      break; */
/*  	    case 1: /* ouverture */
/*  	      x = RESFACTXF ((float) 50*(i*i*i*PI/(nb_x*nb_x*nb_x)*fabs(lys.dEdt_moyen*5000) + sin((i+1)*PI/nb_x))*cos(5*alpha*i*2*PI/nb_x +  j*2*PI/nb_y)  ); */
/*  	      y = RESFACTYF ((float) 50*(i*i*i*PI/(nb_x*nb_x*nb_x)*fabs(lys.dEdt_moyen*5000) + sin((i+1)*PI/nb_x))*sin(5*alpha*i*2*PI/nb_x +  j*2*PI/nb_y) ); */
/*  	      z = RESFACTXF ((float) 100*cos((float)i/nb_x*PI)*(1 + lys.dEdt_moyen*1000) ); */
/*  	      break; */
/*  	    case 2: /* oeuf */
/*  	      x = RESFACTXF ((float) 130*sin((i+1)*PI/nb_x)*cos(-i*2*PI/(5*nb_x)+ j*2*PI/nb_y) ); */
/*  	      y = RESFACTYF ((float) 130*sin((i+1)*PI/nb_x)*sin(-i*2*PI/(5*nb_x)+ j*2*PI/nb_y) ); */
/*  	      z = -RESFACTXF (130*cos((float)i/nb_x*PI)*lys.dEdt_moyen*1000 ); */

/*  	      break; */
/*  	    case 3: /* forme bizzare se deforme*/
/*  	      x = RESFACTXF ((float) 25*(i+1)*cos(-i*2*PI/(10*nb_x)+ j*2*PI/nb_y) ); */
/*  	      y = RESFACTYF ((float) 25*(i+1)*sin(-i*2*PI/(10*nb_x)+ j*2*PI/nb_y) ); */
/*  	      z = RESFACTXF (60* (cos(j*2*PI/nb_y+10*alpha)+cos(i*2*PI/nb_y)) ); */
/*  	      break; */
/*  	    default: */
/*  	    } */

/*  	  rotation_3d (&x, &y, &z, alpha, beta, gamma); */
/*  	  perspective (&x, &y, &z, persp, dist_cam); */

/*  	  if (x >= xres2 || x <= -xres2 || y >= yres2 || y <= -yres2) */
/*  	    goto non; */

/*  	  ix = (int) x; */
/*  	  iy = (int) y; */

/*  	  color = ((int)(float)(100+0.4*z)); */
/*  	  if (color<0) */
/*  	    color = 0; */
/*  	  taille = color/8; */

    /*  	  boule*//*_rand*//* (buffer, ix, iy, taille, color); */

/*  	non: */
/*  	  ; */

/*  	} */
/*      } */
#ifdef DEBUG
    printf("Quitter burn_3d\n");
#endif
}


/*******************************************************************/
/**************** STARS MORPH **************************************/
/*******************************************************************/


void stars_create_state(float pos[3][STARS_MAX], int mode) {
    int i, j, sq = sqrt(STARS_MAX);
    float fsq = (float) sq;


    switch (mode) {
        case 0:
            for (i = 0; i < STARS_MAX; i++)
                for (j = 0; j < 3; j++) {
                    pos[j][i] = 0;
                }
            break;
        case 1:
            for (i = 0; i < STARS_MAX; i++)
                for (j = 0; j < 3; j++) {
                    pos[j][i] = ((float) rand() / RAND_MAX - 0.5);

                }
            break;
        case 2:
            for (j = 0; j < sq; j++)
                for (i = 0; i < sq; i++) {
                    pos[0][i + j * sq] = (i - fsq / 2) * 2 / fsq;
                    pos[1][i + j * sq] = (j - fsq / 2) * 2 / fsq;
                    pos[2][i + j * sq] = 0;
                }
            break;
        case 3:
            for (j = 0; j < sq; j++)
                for (i = 0; i < sq; i++) {
                    pos[0][i + j * sq] = sin((i + 1) * PI / fsq);
                    pos[1][i + j * sq] = sin(-i * 2 * PI / (10 * fsq) + j * 2 * PI / fsq);
                    pos[2][i + j * sq] = cos(j * 2 * PI / fsq);
                }
            break;

            //default:
    }
}

void stars_manage(unsigned char *buffer, int new, float alpha, float beta,
                  float gamma, int persp, int dist_cam) {
    static float pos[2][3][STARS_MAX], morpheur;
    static int select;
    float x[STARS_MAX], y[STARS_MAX], z[STARS_MAX], xres2 = (float) (resx >> 1), yres2 = (float) (resy >> 1);
    float x_t, y_t, z_t;
    float mult;
    int ix, iy, i, color, taille;

    if (new == NEW_SESSION) /* explosion */
    {
        morpheur = 0;
        select = 1;
        stars_create_state(pos[0], 0);
        stars_create_state(pos[1], 1);
        printf("NEW SESSION\n");
    } else if (new == NEW) /* on creer une nouvelle forme */
    {
        mult = 1;
        if ((rand() % 3) == 0)
            mult = 4;

        for (i = 0; i < STARS_MAX; i++) {
            pos[select][0][i] = mult * x[i];
            pos[select][1][i] = mult * y[i];
            pos[select][2][i] = mult * z[i];
        }
        select = 1 - select;
        stars_create_state(pos[select], rand() % 2 + 1);
        printf("NEW\n");
    } else  /* on gere */
    {
        morpheur += (2 * (float) select - 1) * MORPH_SPEED * conteur.dt;

        if (morpheur > 1)
            morpheur = 1;
        else if (morpheur < 0)
            morpheur = 0;

        for (i = 0; i < STARS_MAX; i++) {
            x[i] = (pos[1][0][i] * morpheur + (1 - morpheur) * pos[0][0][i]);
            y[i] = (pos[1][1][i] * morpheur + (1 - morpheur) * pos[0][1][i]);
            z[i] = (pos[1][2][i] * morpheur + (1 - morpheur) * pos[0][2][i]);

            x_t = 250 * x[i];
            y_t = 250 * y[i];
            z_t = 250 * z[i];

            rotation_3d(&x_t, &y_t, &z_t, alpha, beta, gamma);
            perspective(&x_t, &y_t, &z_t, persp, dist_cam);

            if ((int) x_t >= xres2 || (int) x_t <= -xres2 || (int) y_t >= yres2 || (int) y_t <= -yres2 ||
                z_t > 2 * dist_cam)
                goto non;

            ix = (int) x_t;
            iy = (int) y_t;

            color = ((int) (float) (100 + 0.4 * z_t));
            if (color < 0)
                color = 0;
            taille = color / 8;


            droite(buffer, ix, iy, xres2 / 2, -yres2, color / 8);

            /* 	  droite(buffer,ix,iy, 0,0 , color/8);  */
            boule(buffer, ix, iy, taille, color);
            non:;
        }
    }

}





/*******************************************************************/
/**************** OSCILLO ******************************************/
/*******************************************************************/


unsigned char
couleur(gint16 x) {
    float xf = (float) x, resx2f = (float) resx / 2;
    return (unsigned char) ((float) -(xf - resx2f) * (xf + resx2f) * 150 /
                            (resx2f * resx2f));
}


void
courbes(unsigned char *buffer, gint16 data[2][512], unsigned char color, int type) {
    int j, i, x1, y1, x2 = 0, y2 = 0;
    int r;

#define RAYON 100

    switch (type) {
        case 0:
            for (i = 0; i < resx - 1 && i < 511; i++) {
                j = i - 256;
                droite(buffer, j, data[0][i] / (256) + resy / 6, j + 1,
                       data[0][i + 1] / (256) + resy / 6, couleur(j));
                droite(buffer, j, data[1][i] / (256) - resy / 6, j + 1,
                       data[1][i + 1] / (256) - resy / 6, couleur(j));
            }
            break;
        case 1:
            r = data[0][255] >> 8;
            x2 = (RAYON + r) * cos(255 * 2 * PI / 256);
            y2 = (RAYON + r) * sin(255 * 2 * PI / 256);
            for (i = 0; i < 256; i++) {
                r = data[0][i] >> 8;
                x1 = (RAYON + r) * cos(i * 2 * PI / 256);
                y1 = (RAYON + r) * sin(i * 2 * PI / 256);
                droite(buffer, x1, y1, x2, y2, 100);
                x2 = x1;
                y2 = y1;
            }
            break;
        default:
            break;
    }

}


/*******************************************************************/
/**************** SINUS_RECT ***************************************/
/*******************************************************************/


void
sinus_rect(unsigned char *buffer, int k) {
    float fk = k;
    ball(buffer, RESFACTXF (250 * cos(fk / 25)),
         RESFACTYF (100 * sin(2 * fk / 25)), RESFACTXF (100), 200);
    ball(buffer, RESFACTXF (-250 * cos(fk / 25)),
         RESFACTYF (100 * sin(2 * fk / 25)), RESFACTXF (100), 200);
}








/*

void rectangle(unsigned char *buffer, int x, int y, int weight , int height, unsigned char color)
{
  int i,j;

  for (i=-weight/2; i<weight/2; i++)
    for (j=-height/2; j<height/2; j++)
      {
	*(buffer+ (-(j+y)+resy/2)*resx+(x+i)+resx/2)=maxe(*(buffer+ (-(j+y)+resy/2)*resx+(x+i)+resx/2)+180 -abs(2*i)-abs(2*j),255);
      }
}
  
*/


