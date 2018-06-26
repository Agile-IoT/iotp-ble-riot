/*
 * Copyright (C) 2018 Freie Universität Berlin
 *               2018 Codecoup
 *               2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       BLE peripheral example using NimBLE
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Andrzej Kaczmarek <andrzej.kaczmarek@codecoup.pl>
 * @author      Emmanuel Baccelli <emmanuel.baccelli@inria.fr>
 *
 * @}
 */

#include "ble_sensors.h"
#include "thread.h"
#include "xtimer.h"

char notificationstack[THREAD_STACKSIZE_MAIN];
int static_sensor_value;

static const char device_name[] = "SensorTag RIOT";
static uint8_t own_addr_type;

static void start_advertise(void);

void *notificationthread_handler(void *arg)
{
    /* remove warning unused parameter arg */
    (void)arg;
    uint16_t chr_val_handle;
    int rc=0;
    xtimer_sleep(1);
    int counter = 0;
    
    while(1) {
    
        rc = ble_gatts_find_chr(&gatt_svr_chr_otp_serv_uuid.u, &gatt_svr_chr_otp_data_uuid.u, NULL, &chr_val_handle);
        if(rc == 0){
        ble_gatts_chr_updated(chr_val_handle);
        }
        else puts("Error: finding OTP service handle");
        xtimer_sleep(1);
    
        rc = ble_gatts_find_chr(&gatt_svr_chr_temp_serv_uuid.u, &gatt_svr_chr_temp_data_uuid.u, NULL, &chr_val_handle);
        if(rc == 0){
        ble_gatts_chr_updated(chr_val_handle);
        }
        else puts("Error: finding temperature service handle");
        xtimer_sleep(2);
        
        rc = ble_gatts_find_chr(&gatt_svr_chr_hum_serv_uuid.u, &gatt_svr_chr_hum_data_uuid.u, NULL, &chr_val_handle);
        if(rc == 0){
        ble_gatts_chr_updated(chr_val_handle);
        }
        else puts("Error: finding humidity service handle");
        xtimer_sleep(2);
        
        rc = ble_gatts_find_chr(&gatt_svr_chr_press_serv_uuid.u, &gatt_svr_chr_press_data_uuid.u, NULL, &chr_val_handle);
        if(rc == 0){
        ble_gatts_chr_updated(chr_val_handle);
        }
        else puts("Error: finding pressure service handle");
        xtimer_sleep(2);
        
        rc = ble_gatts_find_chr(&gatt_svr_chr_opt_serv_uuid.u, &gatt_svr_chr_opt_data_uuid.u, NULL, &chr_val_handle);
        if(rc == 0){
        ble_gatts_chr_updated(chr_val_handle);
        }
        else puts("Error: finding optical service handle");
        
        xtimer_sleep(10);

    }

    return NULL;
}

static void put_ad(uint8_t ad_type, uint8_t ad_len, const void *ad, uint8_t *buf,
                   uint8_t *len)
{
    buf[(*len)++] = ad_len + 1;
    buf[(*len)++] = ad_type;

    memcpy(&buf[*len], ad, ad_len);

    *len += ad_len;
}

static void update_ad(void)
{
    uint8_t ad[BLE_HS_ADV_MAX_SZ];
    uint8_t ad_len = 0;
    uint8_t ad_flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    put_ad(BLE_HS_ADV_TYPE_FLAGS, 1, &ad_flags, ad, &ad_len);
    put_ad(BLE_HS_ADV_TYPE_COMP_NAME, sizeof(device_name), device_name, ad, &ad_len);

    ble_gap_adv_set_data(ad, ad_len);
}

static int gap_event_cb(struct ble_gap_event *event, void *arg)
{
    (void)arg;

    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status) {
                start_advertise();
            }
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            start_advertise();
            break;
    }

    return 0;
}

static void start_advertise(void)
{
    struct ble_gap_adv_params advp;
    int rc;

    memset(&advp, 0, sizeof advp);
    advp.conn_mode = BLE_GAP_CONN_MODE_UND;
    advp.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER,
                           &advp, gap_event_cb, NULL);
    assert(rc == 0);
}

static void app_ble_sync_cb(void)
{
    int rc;

    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    assert(rc == 0);

    /* generate the advertising data */
    update_ad();

    start_advertise();
}

int ble_sensor_init(void)
{
    /* initialize NimBLE's controller */
    nimble_riot_controller_init();

    /* register the synchronization callback that is triggered once the host has
     * finished its initialization */
    ble_hs_cfg.sync_cb = app_ble_sync_cb;

    /* initialize NimBLE porting layer and the default GATT and GAP services*/
    nimble_port_init();
    ble_svc_gap_init();
    ble_svc_gatt_init();
    
    /* add gatt services */
    printf("Advertised device name: '%s'\n", device_name);
    puts("Starting BLE sensor notifications!");
    puts("Type 'help' for available shell commands.\n");
    int rc = gatt_svr_init();
    assert(rc == 0);

    /* set the device name */
    ble_svc_gap_device_name_set(device_name);
    
    /* launch notification thread */
    thread_create(notificationstack, sizeof(notificationstack),
                    THREAD_PRIORITY_MAIN - 1,
                    THREAD_CREATE_STACKTEST,
                    notificationthread_handler,
                    NULL, "notification thread");

  /*  ps(); */
    /* and finally run NimBLE's host event loop. The event loop contains a pre-
     * configured event which will trigger the hosts initialization */
    nimble_port_run();

    return 0;
}
