#include <stdlib.h>
#include <math.h>
#include "draw_low_level.h"
#define MAXE(a,b) ( (a) > (b) ? (b) : (a)) 
/*#define MAXE(a,b) (((int)(a>b))&1)*b+(((int)(!(a>b))&1)*a) */
#define MAX255(a) ((a<=255)*a+(!(a<=255))*255)

extern int resx, resy, xres2, yres2, pitch, video;
unsigned char *big_ball;
unsigned int *big_ball_scale[BIG_BALL_SIZE];

void tracer_point_add (unsigned char * buffer, int x, int y, unsigned char color)
{
  unsigned char *point;
  int calcul;

 /*   goto non;  */

  if (x >= xres2 || x <= -xres2 || y >= yres2 || y <= -yres2)
    goto non;

  point = buffer + (-y + yres2) * resx + x + xres2;
  calcul = (int)(color) + (int)(*point);
  *point = MAX255(calcul);
  /*  *point = MAXE ((int)color + *point, 255);
   */
 
non:
  ;
}

void tracer_point_add_32 (unsigned char * buffer, int x, int y, unsigned char color)
{
  unsigned char *point;
  int calcul;

 /*  goto non;  */

  if (x >= xres2 || x <= -xres2 || y >= yres2 || y <= -yres2)
    goto non;

  point = buffer + (-y + yres2) * pitch + ((x + xres2) << 2);
  /* *point = MAXE ((int)(color) + *point, 255); */
  calcul = (int)(color) + (int)(*point);
  *point = MAX255(calcul);

  point++;

  calcul = (int)(color) + (int)(*point);
  *point = MAX255(calcul);

  /*  *point = MAXE ((int)color + *point, 255); */
  point++;
  calcul = (int)(color) + (int)(*point);
  *point = MAX255(calcul);
  
/*  *point = MAXE ((int)color + *point, 255); */

non:
  ;
}


void cercle (unsigned char * buffer, int h, int k, int y, unsigned char color)
{
  int x = -1, d = 3 - 2 * y;

  while (x <= y)
    {
      if (d < 0)
	{
	  d += 4 * x + 6;
	  x++;
	}
      else
	{
	  d += 4 * (x - y) + 10;
	  x++;
	  y--;
	}
      tracer_point_add (buffer, h + x, y + k, color);
      tracer_point_add (buffer, h + y, x + k, color);
      tracer_point_add (buffer, h - y, x + k, color);
      tracer_point_add (buffer, h - x, y + k, color);
      tracer_point_add (buffer, h - x, -y + k, color);
      tracer_point_add (buffer, h - y, -x + k, color);
      tracer_point_add (buffer, h + y, -x + k, color);
      tracer_point_add (buffer, h + x, -y + k, color);
    }
}

void cercle_32 (unsigned char * buffer, int h, int k, int y, unsigned char color)
{
  int x = -1, d = 3 - 2 * y;

  while (x <= y)
    {
      if (d < 0)
	{
	  d += 2* x+6 ;
	  x++;
	}
      else
	{
	  d += 4 * (x - y)+10 ;
	  x++;
	  y--;
	}
      tracer_point_add_32 (buffer, h + x, y + k, color);
      tracer_point_add_32 (buffer, h + y, x + k, color);
      tracer_point_add_32 (buffer, h - y, x + k, color);
      tracer_point_add_32 (buffer, h - x, y + k, color);
      tracer_point_add_32 (buffer, h - x, -y + k, color);
      tracer_point_add_32 (buffer, h - y, -x + k, color);
      tracer_point_add_32 (buffer, h + y, -x + k, color);
      tracer_point_add_32 (buffer, h + x, -y + k, color);
    }
}





void boule_random (unsigned char * buffer, int x, int y, int r, unsigned char color)
{
  int i, j, ecart = (int) rand()%5+1;
  j = color;
  if (video == 8)
    for (i = 0; i <= r; i+=ecart)
      {
	
	cercle (buffer, x, y, i, (int) ((float) (j * j) / (256)));
	j = color - color * ((float) i) / r;
      }
  else
   for (i = 0; i <= r; i+=ecart)
      {
	
	cercle_32 (buffer, x, y, i, (int) ((float) (j * j) / (256)));
	j = color - color * ((float) i) / r;
      }
}

void
boule (unsigned char * buffer, int x, int y, int r, unsigned char color)
{
  int i, j;
  if (video == 8)
    for (i = r; i >= 0; i--)
      {
	j = color - color * ((float) i) / r;
	/*	cercle (buffer, x, y, i, (int) ((float) (j * j) / (256))); */
	/* Optimisation by Karl Soulabaille. 
	   Remplace un flottant par un entier */
	cercle (buffer, x, y, i, (j*j)>>8);
      }
  else
    for (i = 0; i < r; i++)
      {
	j = color - color * ((float) i) / r;
	/*	cercle_32 (buffer, x, y, i, (int) ((float) (j * j) / (256))); */
	cercle_32(buffer, x, y, i, (j*j)>>8);
      }
}

void droite (unsigned char * buffer, int x1, int y1, int x2, int y2, unsigned char color)
{

  int lx, ly, dx, dy;
  int i, j, k;

  lx = abs(x1-x2);
  ly = abs(y1-y2);
  dx = (x1>x2) ? -1 : 1;
  dy = (y1>y2) ? -1 : 1;
	
  if( video ==8)
    if (lx>ly) 
      {
	for (i=x1,j=y1,k=0;i!=x2;i+=dx,k+=ly) 
	  {
	    if (k>=lx) 
	      {
		k -= lx;
		j += dy;
	      }
	    tracer_point_add (buffer, i, j, color);
	  }
      } 
    else 
      {
	for (i=y1,j=x1,k=0;i!=y2;i+=dy,k+=lx) 
	  {
	    if (k>=ly) 
	      {
		k -= ly;
		j += dx;
	      }
	    tracer_point_add (buffer, j, i, color);
	  }
      }	
  else
    if (lx>ly) 
      {
	for (i=x1,j=y1,k=0;i!=x2;i+=dx,k+=ly) 
	  {
	    if (k>=lx) 
	      {
		k -= lx;
		j += dy;
	      }
	    tracer_point_add_32 (buffer, i, j, color);
	  }
      } 
    else 
      {
	for (i=y1,j=x1,k=0;i!=y2;i+=dy,k+=lx) 
	  {
	    if (k>=ly) 
	      {
		k -= ly;
		j += dx;
	      }
	    tracer_point_add_32 (buffer, j, i, color);
	  }
      }	

}
























void ball_init(void)
{
  int i,j,k;
  int x,y;


  big_ball = (unsigned char *) malloc(BIG_BALL_SIZE*BIG_BALL_SIZE);

  for (i=0 ; i < BIG_BALL_SIZE ;i++)
    big_ball_scale[i] = (unsigned int *) malloc((i+1)*sizeof(int));

  for (i=0 ; i < BIG_BALL_SIZE ;i++)
    for (j=0 ; j<i ;j++)
      big_ball_scale[i][j] = (int) floor((float)j*BIG_BALL_SIZE/(i+1));

  /*creation de la grande boulle */
 for (i =0; i< BIG_BALL_SIZE/2; i++) /*pour chaque rayon*/
   for (j=0; j< 2000; j++)
   {
     x = (int) ((float) (BIG_BALL_SIZE/2 + i*0.5*cos((float)j/2000*2*3.1416)) );
     y = (int) ((float) (BIG_BALL_SIZE/2 + i*0.5*sin((float)j/2000*2*3.1416)) );

     k = 255-255 * ((float) (i) / (BIG_BALL_SIZE/2));

     big_ball[ y * BIG_BALL_SIZE + x] = MAX255(3*((k*k)>>9));  
   }


}

void ball(unsigned char *buffer, int x, int y, int r, unsigned char color)
{
  int i,j,k,d= 2*r;
  int *pt=big_ball_scale[d],colorc;
  float fcolor = (float) color;

  int a,b,c,e,f,g,h,l;
  if (d>=BIG_BALL_SIZE)
    r = BIG_BALL_SIZE/ 2 - 1;
  if (video==8) 
    for (j=-r+1; j <= 0; j++)
      {
	k = pt[j+r-1] * BIG_BALL_SIZE;
	a = j + y;
	b = j + x ;
      c = -j + x ;
      e = -j + y  ;
    for (i=-r+1; i <= j; i++)
      {	
	f = i + x ;
	g = -i + x;
	h = i + y;
	l = -i + y;
	
	colorc = (int) (fcolor/256* (float)big_ball[k + pt[i+r-1]]);

	tracer_point_add(buffer, f   ,a        ,	colorc );
    	tracer_point_add(buffer, g     ,a        ,	colorc );  
  	tracer_point_add(buffer, f     ,e        ,	colorc ); 
    	tracer_point_add(buffer, g     ,e        ,	colorc ); 

	tracer_point_add(buffer, b         ,h     ,	colorc );
    	tracer_point_add(buffer, b         ,l     , 	colorc );  
  	tracer_point_add(buffer, c         ,h     ,	colorc ); 
    	tracer_point_add(buffer, c         ,l     ,	colorc ); 
      }
    }
  else
  for (j=-r+1; j <= 0; j++)
    {
      k = pt[j+r-1] * BIG_BALL_SIZE;
      a = j + y;
      b = j + x ;
      c = -j + x ;
      e = -j + y  ;
    for (i=-r+1; i <= j; i++)
      {	
	f = i + x ;
	g = -i + x;
	h = i + y;
	l = -i + y;
	
	colorc = (int) (fcolor/256* (float)big_ball[k + pt[i+r-1]]);

	tracer_point_add_32(buffer, f   ,a        ,	colorc );
    	tracer_point_add_32(buffer, g     ,a        ,	colorc );  
  	tracer_point_add_32(buffer, f     ,e        ,	colorc ); 
    	tracer_point_add_32(buffer, g     ,e        ,	colorc ); 

	tracer_point_add_32(buffer, b         ,h     ,	colorc );
    	tracer_point_add_32(buffer, b         ,l     , 	colorc );  
  	tracer_point_add_32(buffer, c         ,h     ,	colorc ); 
    	tracer_point_add_32(buffer, c         ,l     ,	colorc ); 
      }
    }
    
}
    











void
boule_no_add (unsigned char * buffer, int x, int y, int r, unsigned char color)
{
  int i, j;
  for (i = r; i >= 0; i--)
    {
      j = color - color * ((float) i) / r;
      cercle_no_add (buffer, x, y, i, (j*j)>>8);
    }
 
}

void cercle_no_add (unsigned char * buffer, int h, int k, int y, unsigned char color)
{
  int x = -1, d = 3 - 2 * y;

  while (x <= y)
    {
      if (d < 0)
	{
	  d += 4 * x + 6;
	  x++;
	}
      else
	{
	  d += 4 * (x - y) + 10;
	  x++;
	  y--;
	}
      tracer_point_no_add (buffer, h + x, y + k, color);
      tracer_point_no_add (buffer, h + y, x + k, color);
      tracer_point_no_add (buffer, h - y, x + k, color);
      tracer_point_no_add (buffer, h - x, y + k, color);
      tracer_point_no_add (buffer, h - x, -y + k, color);
      tracer_point_no_add (buffer, h - y, -x + k, color);
      tracer_point_no_add (buffer, h + y, -x + k, color);
      tracer_point_no_add (buffer, h + x, -y + k, color);
    }
}

void tracer_point_no_add (unsigned char * buffer, int x, int y, unsigned char color)
{
  unsigned char *point;

  point = buffer + (-y + BIG_BALL_SIZE/2) * BIG_BALL_SIZE + x + BIG_BALL_SIZE/2;
  *point = (int)(color); 

}
