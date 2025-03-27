

extern char bit_seek_read(char dir, char * param_ptr);
extern char bit_seek_save(char dir, char * param_ptr);
char vcc_manual_save(char mode, char val);
char iap_read(int adr);
void iap_write(int adr, char dat);
void iap_erase(int adr);