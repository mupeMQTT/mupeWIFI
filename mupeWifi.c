#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
#include "mupeWifi.h"
#include "mupeWifiWeb.h"
#include "mupeWifiNvs.h"


#define CONFIG_AP_MAX_STA_CONN 3
#define CONFIG_AP_WIFI_CHANNEL 1

static const char *TAG = "muepWifi";

static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;

static bool wifi_ap() {
	wifi_config_t wifi_config = { 0 };
	strcpy((char*) wifi_config.ap.ssid, "MQTT_init");
	strcpy((char*) wifi_config.ap.password, "");
	wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
	wifi_config.ap.ssid_len = strlen("MQTT_ini");
	wifi_config.ap.max_connection = CONFIG_AP_MAX_STA_CONN;
	wifi_config.ap.channel = CONFIG_AP_WIFI_CHANNEL;
	wifi_config.ap.authmode = WIFI_AUTH_OPEN;
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_LOGI(TAG, "WIFI_MODE_AP started. SSID:%s password:%s channel:%d",
			"MQTT_init", "", CONFIG_AP_WIFI_CHANNEL);
	return ESP_OK;
}

static void event_handler(void *arg, esp_event_base_t event_base,
		int32_t event_id, void *event_data) {
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
		ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
		wifi_event_sta_disconnected_t *event =
				(wifi_event_sta_disconnected_t*) event_data;
		if (event->reason == 15 || event->reason == 201) {
			wifi_ap();
		}
		ESP_LOGI(TAG, "WIFI_STA_DISCONNECTED %i", event->reason);
		esp_wifi_connect();
		xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
		xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);

	}
}

static void initialise_wifi(void) {
	static bool initialized = false;
	if (initialized) {
		return;
	}
	ESP_ERROR_CHECK(esp_netif_init());
	wifi_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
	assert(ap_netif);
	esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
	assert(sta_netif);
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(
			esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &event_handler, NULL));
	ESP_ERROR_CHECK(
			esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
	ESP_ERROR_CHECK(esp_wifi_start());

	initialized = true;
}

static bool wifi_sta(int timeout_ms, char *passwd, char *ssid) {
	wifi_config_t wifi_config = { 0 };
	strcpy((char*) wifi_config.sta.ssid, ssid);
	strcpy((char*) wifi_config.sta.password, passwd);

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_connect());

	int bits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
	pdFALSE, pdTRUE, timeout_ms / portTICK_PERIOD_MS);
	ESP_LOGI(TAG, "bits=%x", bits);
	if (bits) {
		ESP_LOGI(TAG, "WIFI_MODE_STA connected. SSID:%s password:%s", ssid,
				passwd);
	} else {
		ESP_LOGI(TAG, "WIFI_MODE_STA can't connected. SSID:%s password:%s",
				ssid, passwd);
	}
	return (bits & CONNECTED_BIT) != 0;
}

static bool wifi_apsta(int timeout_ms, char *passwd, char *ssid) {
	wifi_config_t ap_config = { 0 };
	strcpy((char*) ap_config.ap.ssid, "esp_ini");
	strcpy((char*) ap_config.ap.password, "");
	ap_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
	ap_config.ap.ssid_len = strlen("MQTT_ini");
	ap_config.ap.max_connection = CONFIG_AP_MAX_STA_CONN;
	ap_config.ap.channel = CONFIG_AP_WIFI_CHANNEL;

	ap_config.ap.authmode = WIFI_AUTH_OPEN;

	wifi_config_t sta_config = { 0 };
	strcpy((char*) sta_config.sta.ssid, ssid);
	strcpy((char*) sta_config.sta.password, passwd);

	if (strlen(ssid)==0){
		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
		ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));

	}else{
		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
		ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));
		ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config));
	}


	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_LOGI(TAG, "WIFI_MODE_AP started. SSID:%s password:%s channel:%d", ssid,
			passwd, CONFIG_AP_WIFI_CHANNEL);

	if (strlen(ssid)!=0){

	ESP_ERROR_CHECK(esp_wifi_connect());
	}
	int bits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
	pdFALSE, pdTRUE, timeout_ms / portTICK_PERIOD_MS);
	ESP_LOGI(TAG, "bits=%x", bits);
	if (bits) {
		ESP_LOGI(TAG, "WIFI_MODE_STA connected. SSID:%s password:%s", ssid,
				passwd);
	} else {
		ESP_LOGI(TAG, "WIFI_MODE_STA can't connected. SSID:%s password:%s",
				ssid, passwd);
	}
	return (bits & CONNECTED_BIT) != 0;
}

void mupeWifiInit() {
	ESP_LOGI(TAG, "mupeWifiInit");
	mupeWifiNvsInit();

	initialise_wifi();
	char dap[(dapGetStrSize() == 0) ? 2 : dapGetStrSize()];

	if ( dapGetStrSize()==0) {
		dap[0] = '0';
		dap[1] = 0;
		dapSet(dap);
	}else{
		dapGet(dap);
	}

	char ssid[(ssidGetStrSize()==0) ?1:ssidGetStrSize()];
	if (sizeof(ssid) == 1) {
		ssid[0] = 0;
		ssidSet(ssid);
	}else{
		ssidGet(ssid);
	}

	char pwd[(pwdGetStrSize()==0)?1:pwdGetStrSize()];
	if (sizeof(pwd) == 1) {
		pwd[0] =0;
		pwdSet(pwd);
	}else{
		pwdGet(pwd);
	}


	ESP_LOGI(TAG, "WIFIcfg dap %s ", dap);
	if (dap[0] == '0') {
		ESP_LOGI(TAG, "Start APSTA Mode");
		wifi_apsta(5 * 1000, pwd, ssid);
	} else {
		ESP_LOGI(TAG, "Start STA Mode");
		wifi_sta(5 * 1000, pwd, ssid);
	}
	mupeWifiWebInit();
}
