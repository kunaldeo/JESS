void analyser(unsigned char *buffer);

void fusee(unsigned char *buffer, int new);

void super_spectral(unsigned char *buffer);

void super_spectral_balls(unsigned char *buffer);

void
grille_3d(unsigned char *buffer, gint16 data[2][512], float alpha, float beta, float gamma, int persp, int dist_cam);

void l2_grilles_3d(unsigned char *buffer, gint16 data[2][512], float alpha, float beta, float gamma, int persp,
                   int dist_cam);

void burn_3d(unsigned char *buffer, gint16 data[2][512], float alpha, float beta, float gamma, int persp, int dist_cam,
             int mode);

void courbes(unsigned char *buffer, gint16 data[2][512], unsigned char color, int type);

void sinus_rect(unsigned char *buffer, int k);

void stars_create_state(float pos[3][STARS_MAX], int mode);

void stars_manage(unsigned char *buffer, int new, float alpha, float beta, float gamma, int persp, int dist_cam);
