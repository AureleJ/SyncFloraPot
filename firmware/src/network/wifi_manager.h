#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

typedef struct {
    char device_name[32];
    char pin_code[16];
} provisioning_info_t;

provisioning_info_t network_state_get_device_info();

void wifi_manager_init();

#endif // WIFI_MANAGER_H