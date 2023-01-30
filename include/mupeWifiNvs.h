#ifndef MUPEWIFINVS_H
#define MUPEWIFINVS_H
#include "esp_system.h"


void mupeWifiNvsInit(void);

size_t dapGetStrSize();
char* dapGet(char * dap);
void dapSet(char * dap);

size_t ssidGetStrSize();
char* ssidGet(char * ssid);
void ssidSet(char * ssid);

size_t pwdGetStrSize();
char* pwdGet(char * pwd);
void pwdSet(char * pwd);

size_t urlGetStrSize();
char* urlGet(char * url);
void urlSet(char *url);

#endif
