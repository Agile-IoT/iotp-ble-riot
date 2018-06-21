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

static const char *manuf_name = "Texas Instruments";
static const char *model_num = "CC2650 SensorTag";
uint16_t hrs_hrm_handle;
uint16_t variation = 0;

static int
gatt_svr_chr_access_temperature_data(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);

static int
gatt_svr_chr_access_hum_data(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);

static int
gatt_svr_chr_access_press_data(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);

static int
gatt_svr_chr_access_temperature_conf(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);
                               
static int
gatt_svr_chr_access_temperature_peri(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);

static int
gatt_svr_chr_access_optical_data(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);

static int
gatt_svr_chr_access_device_info(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg);

static int
gatt_svr_chr_access_io_data(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);

static int
gatt_svr_chr_access_io_conf(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /* Service: Device Information */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(GATT_DEVICE_INFO_UUID),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            /* Characteristic: * Manufacturer name */
            .uuid = BLE_UUID16_DECLARE(GATT_MANUFACTURER_NAME_UUID),
            .access_cb = gatt_svr_chr_access_device_info,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            /* Characteristic: Model number string */
            .uuid = BLE_UUID16_DECLARE(GATT_MODEL_NUMBER_UUID),
            .access_cb = gatt_svr_chr_access_device_info,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            0, /* No more characteristics in this service */
        }, }
    },
    {
        /* Service: Temperature */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = (ble_uuid_t*)&gatt_svr_chr_temp_serv_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) { {
            /* Characteristic: Temperature data */
            .uuid = (ble_uuid_t*)&gatt_svr_chr_temp_data_uuid.u,
            .access_cb = gatt_svr_chr_access_temperature_data,
         /*   .val_handle = &hrs_hrm_handle, */
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
        }, {
            /* Characteristic: Temperature conf */
            .uuid = (ble_uuid_t*)&gatt_svr_chr_temp_conf_uuid.u,
            .access_cb = gatt_svr_chr_access_temperature_conf,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
        }, {
            /* Characteristic: Temperature peri */
            .uuid = (ble_uuid_t*)&gatt_svr_chr_temp_peri_uuid.u,
            .access_cb = gatt_svr_chr_access_temperature_peri,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
        }, {
            0, /* No more characteristics in this service */
        }, }
    },
        {
        /* Service: Humidity */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = (ble_uuid_t*)&gatt_svr_chr_hum_serv_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) { {
            /* Characteristic: Humidity data */
            .uuid = (ble_uuid_t*)&gatt_svr_chr_hum_data_uuid.u,
            .access_cb = gatt_svr_chr_access_hum_data,
         /*   .val_handle = &hrs_hrm_handle, */
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
        }, {
            /* Characteristic: Humidity conf */
            .uuid = (ble_uuid_t*)&gatt_svr_chr_hum_conf_uuid.u,
            /* TO DO CALL BACK */
            .access_cb = gatt_svr_chr_access_temperature_conf,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
        }, {
            /* Characteristic: Humidity peri */
            .uuid = (ble_uuid_t*)&gatt_svr_chr_hum_peri_uuid.u,
            /* TO DO CALL BACK */
            .access_cb = gatt_svr_chr_access_temperature_peri,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
        }, {
            0, /* No more characteristics in this service */
        }, }
    },
        {
        /* Service: Pressure */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = (ble_uuid_t*)&gatt_svr_chr_press_serv_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) { {
            /* Characteristic: Pressure data */
            .uuid = (ble_uuid_t*)&gatt_svr_chr_press_data_uuid.u,
            .access_cb = gatt_svr_chr_access_press_data,
         /*   .val_handle = &hrs_hrm_handle, */
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
        }, {
            /* Characteristic: Pressure conf */
            .uuid = (ble_uuid_t*)&gatt_svr_chr_press_conf_uuid.u,
            /* TO DO CALL BACK */
            .access_cb = gatt_svr_chr_access_temperature_conf,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
        }, {
            /* Characteristic: Pressure peri */
            .uuid = (ble_uuid_t*)&gatt_svr_chr_press_peri_uuid.u,
            /* TO DO CALL BACK */
            .access_cb = gatt_svr_chr_access_temperature_peri,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
        }, {
            0, /* No more characteristics in this service */
        }, }
    },
        {
        /* Service: Optical */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = (ble_uuid_t*)&gatt_svr_chr_opt_serv_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) { {
            /* Characteristic: Optical data */
            .uuid = (ble_uuid_t*)&gatt_svr_chr_opt_data_uuid.u,
            .access_cb = gatt_svr_chr_access_optical_data,
         /*   .val_handle = &hrs_hrm_handle, */
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
        }, {
            /* Characteristic: Optical conf */
            .uuid = (ble_uuid_t*)&gatt_svr_chr_opt_conf_uuid.u,
            /* TO DO CALL BACK */
            .access_cb = gatt_svr_chr_access_temperature_conf,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
        }, {
            /* Characteristic: Optical peri */
            .uuid = (ble_uuid_t*)&gatt_svr_chr_opt_peri_uuid.u,
            /* TO DO CALL BACK */
            .access_cb = gatt_svr_chr_access_temperature_peri,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
        }, {
            0, /* No more characteristics in this service */
        }, }
    },
        {
        /* Service: IO */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = (ble_uuid_t*)&gatt_svr_chr_io_serv_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) { {
            /* Characteristic: IO data(?) */
            .uuid = (ble_uuid_t*)&gatt_svr_chr_io_data_uuid.u,
            .access_cb = gatt_svr_chr_access_io_data,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
        }, {
            /* Characteristic: IO conf (?) */
            .uuid = (ble_uuid_t*)&gatt_svr_chr_io_conf_uuid.u,
            .access_cb = gatt_svr_chr_access_io_conf,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
        }, {
            0, /* No more characteristics in this service */
        }, }
    },
    {
            0, /* No more services */
    },
};

static int
gatt_svr_chr_access_temperature_data(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    /* DUMMY temperature value: 54 67 08 8b */
    static uint32_t dummy_temperature = 0x4867088b;
    /* DUMMY variation */
    /* dummy_temperature = dummy_temperature + variation;
    variation = (dummy_temperature + 10)%9; */
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;
    int rc = os_mbuf_append(ctxt->om, &dummy_temperature, sizeof(dummy_temperature));

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

static int
gatt_svr_chr_access_hum_data(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    /* DUMMY humidity value: 0x740a480d */
    static uint32_t dummy_humidity = 0x640a480d;

    (void)conn_handle;
    (void)attr_handle;
    (void)arg;
    int rc = os_mbuf_append(ctxt->om, &dummy_humidity, sizeof(dummy_humidity));

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

static int
gatt_svr_chr_access_press_data(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    /* DUMMY pressure value: 0x780a00fa8a01 */
    static uint8_t dummy_pressure[6];
    dummy_pressure[0] = 0x79;
    dummy_pressure[1] = 0x0a;
    dummy_pressure[2] = 0x00;
    dummy_pressure[3] = 0xf7;
    dummy_pressure[4] = 0x8a;
    dummy_pressure[5] = 0x01;

    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    int rc = os_mbuf_append(ctxt->om, &dummy_pressure, sizeof(dummy_pressure));

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}


static int
gatt_svr_chr_access_optical_data(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    /* DUMMY optical value: 3c 4c */
    static uint16_t dummy_optical_value = 0x284c;

    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    int rc = os_mbuf_append(ctxt->om, &dummy_optical_value, sizeof(dummy_optical_value));

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

static int
gatt_svr_chr_access_temperature_conf(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    /* DUMMY value */
    static uint8_t dummy_value = 0x01;
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    int rc = os_mbuf_append(ctxt->om, &dummy_value, sizeof(dummy_value));

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

static int
gatt_svr_chr_access_temperature_peri(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    /* DUMMY value */
    static uint8_t dummy_value = 0x02;
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    int rc = os_mbuf_append(ctxt->om, &dummy_value, sizeof(dummy_value));

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

static int
gatt_svr_chr_access_io_data(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    /* DUMMY value */
    static uint8_t dummy_value = 0x7F;
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    int rc = os_mbuf_append(ctxt->om, &dummy_value, sizeof(dummy_value));

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}


static int
gatt_svr_chr_access_io_conf(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    /* DUMMY value */
    static uint8_t dummy_value = 0x00;
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;
    /* uint16_t uuid; */
    int rc = os_mbuf_append(ctxt->om, &dummy_value, sizeof(dummy_value));

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

static int
gatt_svr_chr_access_device_info(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    uint16_t uuid;
    int rc;
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    uuid = ble_uuid_u16(ctxt->chr->uuid);

    if (uuid == GATT_MODEL_NUMBER_UUID) {
        rc = os_mbuf_append(ctxt->om, model_num, strlen(model_num));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    if (uuid == GATT_MANUFACTURER_NAME_UUID) {
        rc = os_mbuf_append(ctxt->om, manuf_name, strlen(manuf_name));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

int gatt_svr_init(void)
{
    int rc;

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    return 0;
}
