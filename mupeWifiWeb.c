
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

#include "mupeWifiWeb.h"
#include "mupeWeb.h"
#include "mupeWifiNvs.h"
#include "esp_http_server.h"

static const char *TAG = "mupeWifiWeb";

#define STARTS_WITH(string_to_check, prefix) (strncmp(string_to_check, prefix, (strlen(prefix))))

esp_err_t mqttCfg_get_handler(httpd_req_t *req) {
	char value[50];
	httpd_resp_set_type(req, "text/html");
	strcpy(value, dapGet(value));
	strcat(value, "\n");
	httpd_resp_send_chunk(req, value, strlen(value));
	strcpy(value, urlGet(value));
	strcat(value, "\n");
	httpd_resp_send_chunk(req, value, strlen(value));
	strcpy(value, pwdGet(value));
	strcat(value, "\n");
	httpd_resp_send_chunk(req, value, strlen(value));
	strcpy(value, ssidGet(value));
	httpd_resp_send_chunk(req, value, strlen(value));
	httpd_resp_send_chunk(req, NULL, 0);
	return ESP_OK;
}

void rebBoot() {
	vTaskDelay(500);
	esp_restart();
}

esp_err_t wifi_get_handler(httpd_req_t *req) {
	extern const unsigned char mqtt_index_start[] asm("_binary_wifi_html_start");
	extern const unsigned char mqtt_index_end[] asm("_binary_wifi_html_end");
	const size_t mqtt_index_size = (mqtt_index_end - mqtt_index_start);
	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, (const char*) mqtt_index_start, mqtt_index_size);
	return ESP_OK;

}
esp_err_t root_wifi_get_handler(httpd_req_t *req) {
	ESP_LOGI(TAG, "HTTPS req %s ", req->uri);
	if (STARTS_WITH(req->uri, "/wifi/wifiCfg") == 0) {
		return mqttCfg_get_handler(req);
	}
	return wifi_get_handler(req);;
}

httpd_uri_t wifi_uri_get = { .uri = "/wifi/*", .method = HTTP_GET, .handler =
		root_wifi_get_handler };
const char *wifi_uri_txt = "Wifi init";



esp_err_t root_wifi_post_handler(httpd_req_t *req) {

	ESP_LOGI(TAG, "root_post_handler req->uri=[%s]", req->uri);
	ESP_LOGI(TAG, "root_post_handler content length %d", req->content_len);
	char *buf = malloc(req->content_len + 1);
	size_t off = 0;
	while (off < req->content_len) {
		/* Read data received in the request */
		int ret = httpd_req_recv(req, buf + off, req->content_len - off);
		if (ret <= 0) {
			if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
				httpd_resp_send_408(req);
			}
			free(buf);
			return ESP_FAIL;
		}

		off += ret;
		ESP_LOGI(TAG, "root_post_handler recv length %d", ret);
	}
	buf[off] = '\0';
	ESP_LOGI(TAG, "root_post_handler buf=[%s]", buf);

	char value[20];

	if (find_value("ssid=", buf, value) > 0) {
		ssidSet(value);
	}

	if (find_value("pwd=", buf, value) > 0) {
		pwdSet(value);
	}

	if (find_value("url=", buf, value) > 0) {
		urlSet(value);
	}

	if (find_value("dap=", buf, value) > 0) {
		dapSet(value);
	}
	free(buf);

	if (STARTS_WITH(req->uri, "/wifi/reboot") == 0) {
		xTaskCreatePinnedToCore(rebBoot, "rebBoot", 2096, NULL, 1, NULL, 0);
	}
	return wifi_get_handler(req);
}

httpd_uri_t wifi_uri_post = { .uri = "/wifi/*", .method = HTTP_POST, .handler =
		root_wifi_post_handler, .user_ctx = NULL };

void mupeWifiWebInit(void) {
	ESP_LOGI(TAG, "mupeWifiWebInit");
	addHttpd_uri(&wifi_uri_post, wifi_uri_txt);
	addHttpd_uri(&wifi_uri_get, wifi_uri_txt);
}
