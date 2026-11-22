#include "wifi_manager.h"

#include "core/system_state.h"

#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>

#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_netif.h>
#include <esp_random.h>
#include <nvs_flash.h>

#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_ble.h>

#include <time.h>
#include "esp_sntp.h"

static const char *TAG = "WIFI_MANAGER";
static int try_count = 0;

provisioning_info_t provisioning_info = {
    .device_name = "",
    .pin_code = ""};

provisioning_info_t network_state_get_device_info()
{
    return provisioning_info;
}

static void get_device_name(char *device_name, size_t max)
{
    uint8_t eth_mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    snprintf(device_name, max, "PROV_%02X%02X%02X", eth_mac[0], eth_mac[1], eth_mac[2]);
}

static void generate_random_pin(char *pin, size_t max)
{
    uint32_t random_number = esp_random() % 10000;
    snprintf(pin, max, "%04lu", (unsigned long)random_number);
}

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_PROV_EVENT)
    {
        switch (event_id)
        {
        case WIFI_PROV_START:
        {
            system_state_set(SYS_STATE_PROVISIONING);
            ESP_LOGI(TAG, "Le Provisioning a commencé.");
            break;
        }
        case WIFI_PROV_CRED_RECV:
        {
            wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *)event_data;
            ESP_LOGI(TAG, "Identifiants Wi-Fi reçus !");
            ESP_LOGI(TAG, "\tSSID     : %s", (const char *)wifi_sta_cfg->ssid);
            ESP_LOGI(TAG, "\tPassword : %s", (const char *)wifi_sta_cfg->password);
            system_state_set(SYS_STATE_CONNECTING);
            break;
        }
        case WIFI_PROV_CRED_FAIL:
        {
            wifi_prov_sta_fail_reason_t *reason = (wifi_prov_sta_fail_reason_t *)event_data;
            ESP_LOGE(TAG, "Échec du Provisioning ! Raison : %s", (*reason == WIFI_PROV_STA_AUTH_ERROR) ? "Authentification Wi-Fi échouée (Mauvais mot de passe)" : "Point d'accès introuvable");
            system_state_set(SYS_STATE_ERROR);
            break;
        }
        case WIFI_PROV_CRED_SUCCESS:
        {
            ESP_LOGI(TAG, "Provisioning réussi ! Connexion en cours au point d'accès...");
            break;
        }
        case WIFI_PROV_END:
        {
            wifi_prov_mgr_deinit();
            break;
        }
        default:
            break;
        }
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        system_state_set(SYS_STATE_CONNECTING);
        esp_wifi_connect();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Connecté avec succès ! Adresse IP : " IPSTR, IP2STR(&event->ip_info.ip));
        system_state_set(SYS_STATE_CONNECTED);

        setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
        tzset();

        esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
        esp_sntp_setservername(0, "pool.ntp.org");
        esp_sntp_init();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ESP_LOGI(TAG, "Déconnecté. Tentative de reconnexion...");
        system_state_set(SYS_STATE_CONNECTING);
        esp_wifi_connect();

        try_count++;

        if (try_count >= 3)
        {
            ESP_LOGE(TAG, "Échec de la reconnexion après 3 tentatives.");
            wifi_prov_mgr_reset_provisioning();
            try_count = 0;
        }
    }
}

void wifi_manager_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_prov_mgr_config_t config = {
        .scheme = wifi_prov_scheme_ble,
        .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM};
    ESP_ERROR_CHECK(wifi_prov_mgr_init(config));

    bool provisioned = false;
    ESP_ERROR_CHECK(wifi_prov_mgr_is_provisioned(&provisioned));

    if (!provisioned)
    {
        ESP_LOGI(TAG, "Aucune configuration Wi-Fi trouvée. Lancement du mode Provisioning...");

        char pin_code[5];
        generate_random_pin(pin_code, sizeof(pin_code));

        char device_name[12];
        get_device_name(device_name, sizeof(device_name));

        snprintf(provisioning_info.device_name, sizeof(provisioning_info.device_name), "%s", device_name);
        snprintf(provisioning_info.pin_code, sizeof(provisioning_info.pin_code), "%s", pin_code);

        ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(WIFI_PROV_SECURITY_1, pin_code, device_name, NULL));
    }
    else
    {
        ESP_LOGI(TAG, "Déjà provisionné ! Démarrage direct du Wi-Fi STA.");

        wifi_prov_mgr_deinit();

        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_start());
    }

    ESP_LOGI(TAG, "OK");
}