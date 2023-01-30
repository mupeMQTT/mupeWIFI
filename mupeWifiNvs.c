
// Copyright Peter Müller mupe
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.
#include "nvs_flash.h"
#include "mupeWifiNvs.h"
#define NAMESPACE_NAME "WIFIcfg"

void mupeWifiNvsInit(void) {
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
}

size_t dapGetStrSize() {
	nvs_handle_t my_handle;
	size_t strSize=0;
	nvs_open(NAMESPACE_NAME, NVS_READWRITE, &my_handle);
	nvs_get_str(my_handle, "dap", NULL, &strSize);
	nvs_close(my_handle);
	return strSize;
}

char* dapGet(char *dap) {
	nvs_handle_t my_handle;
	size_t strSize=0;
	nvs_open(NAMESPACE_NAME, NVS_READWRITE, &my_handle);
	nvs_get_str(my_handle, "dap", NULL, &strSize);
	nvs_get_str(my_handle, "dap", dap, &strSize);
	nvs_close(my_handle);
	return dap;
}
void dapSet(char *dap) {
	nvs_handle_t my_handle;
	nvs_open(NAMESPACE_NAME, NVS_READWRITE, &my_handle);
	nvs_set_str(my_handle, "dap", dap);
	nvs_commit(my_handle);
	nvs_close(my_handle);
}
size_t ssidGetStrSize() {
	nvs_handle_t my_handle;
	size_t strSize=0;
	nvs_open(NAMESPACE_NAME, NVS_READWRITE, &my_handle);
	nvs_get_str(my_handle, "ssid", NULL, &strSize);
	nvs_close(my_handle);
	return strSize;
}
char* ssidGet(char *ssid) {
	nvs_handle_t my_handle;
	size_t strSize;
	nvs_open(NAMESPACE_NAME, NVS_READWRITE, &my_handle);
	nvs_get_str(my_handle, "ssid", NULL, &strSize);
	nvs_get_str(my_handle, "ssid", ssid, &strSize);
	nvs_close(my_handle);
	return ssid;
}
void ssidSet(char *ssid) {
	nvs_handle_t my_handle;
	nvs_open(NAMESPACE_NAME, NVS_READWRITE, &my_handle);
	nvs_set_str(my_handle, "ssid", ssid);
	nvs_commit(my_handle);
	nvs_close(my_handle);
}
size_t pwdGetStrSize() {
	nvs_handle_t my_handle;
	size_t strSize=0;
	nvs_open(NAMESPACE_NAME, NVS_READWRITE, &my_handle);
	nvs_get_str(my_handle, "pwd", NULL, &strSize);
	nvs_close(my_handle);
	return strSize;
}
char* pwdGet(char *pwd) {
	nvs_handle_t my_handle;
	size_t strSize;
	nvs_open(NAMESPACE_NAME, NVS_READWRITE, &my_handle);
	nvs_get_str(my_handle, "pwd", NULL, &strSize);
	nvs_get_str(my_handle, "pwd", pwd, &strSize);
	nvs_close(my_handle);
	return pwd;
}
void pwdSet(char *ssid) {
	nvs_handle_t my_handle;
	nvs_open(NAMESPACE_NAME, NVS_READWRITE, &my_handle);
	nvs_set_str(my_handle, "pwd", ssid);
	nvs_commit(my_handle);
	nvs_close(my_handle);
}

size_t urlGetStrSize() {
	nvs_handle_t my_handle;
	size_t strSize=0;
	nvs_open(NAMESPACE_NAME, NVS_READWRITE, &my_handle);
	nvs_get_str(my_handle, "url", NULL, &strSize);
	nvs_close(my_handle);
	return strSize;
}
char* urlGet(char *url) {
	nvs_handle_t my_handle;
	size_t strSize=0;
	nvs_open(NAMESPACE_NAME, NVS_READWRITE, &my_handle);
	nvs_get_str(my_handle, "url", NULL, &strSize);
	nvs_get_str(my_handle, "url", url, &strSize);
	nvs_close(my_handle);
	return url;
}
void urlSet(char *url) {
	nvs_handle_t my_handle;
	nvs_open(NAMESPACE_NAME, NVS_READWRITE, &my_handle);
	nvs_set_str(my_handle, "url", url);
	nvs_commit(my_handle);
	nvs_close(my_handle);
}

