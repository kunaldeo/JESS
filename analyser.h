void ips(void);

void spectre_moyen(gint16 data_freq_tmp[2][256]);

void C_dEdt_moyen(void);

void C_E_moyen(gint16 data_freq_tmp[2][256]);

void C_dEdt(void);

float energy(gint16 data_freq_tmp[2][256], int type_E);

float time_last(int i, int reinit);

gint detect_beat(void);
