#define MAXBUF 256  // max number of characteres allowed on command line
#define PATH _PC_PATH_MAX //the path on the user's system

void dshCheck(char *cmdline);
void dshProcess(char *args[]);
void dshDetermine(char *args[]);
int dshRunMode2(char *path, char *dir, char *args[]);
char* getCmdline();
