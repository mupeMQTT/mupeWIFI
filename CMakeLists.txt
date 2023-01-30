idf_component_register(SRCS  "mupeWifi.c" "mupeWifiWeb.c" "mupeWifiNvs.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "esp_wifi" "nvs_flash" "mdns" "esp_http_server"  "esp_event" "mupeWeb"
                    EMBED_TXTFILES  "wifi.html" )
