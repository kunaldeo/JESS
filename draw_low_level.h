void boule_random (unsigned char * buffer, int x, int y, int r, unsigned char color);
void cercle(unsigned char *buffer, int h, int k, int y, unsigned char color);
void cercle_32(unsigned char *buffer, int h, int k, int y, unsigned char color);
void boule(unsigned char *buffer, int x, int y, int r, unsigned char color);
void droite(unsigned char *buffer,int x1, int y1, int x2, int y2, unsigned char color);
void tracer_point_add(unsigned char *buffer, int x, int y, unsigned char color);
void tracer_point_add_32(unsigned char *buffer, int x, int y, unsigned char color);

void ball_init(void);
void boule_no_add(unsigned char *buffer, int x, int y, int r, unsigned char color);
void cercle_no_add(unsigned char *buffer, int h, int k, int y, unsigned char color);
void tracer_point_no_add(unsigned char *buffer, int x, int y, unsigned char color);
void ball(unsigned char *buffer, int x, int y, int r, unsigned char color);

#define BIG_BALL_SIZE 1024
