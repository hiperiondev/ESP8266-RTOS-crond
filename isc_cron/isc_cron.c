/*
 * This file is part of the esp-iot-secure-core distribution (https://github.com/hiperiondev/esp-iot-secure-core).
 * Copyright (c) 2020 Emiliano Augusto Gonzalez (comercial@hiperion.com.ar)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "isc_common.h"
#include "timers.h"
#include "ccronexpr.h"
#include "ll.h"
#include "isc_cron.h"
static const char* TAG = "isc_cron";

#define CRON_TIMER_MS 500
#define CRON_QUEUE_LEN 5

typedef struct isc_cron_t {
     uint32_t id;
    cron_expr expr;
       time_t next;
         bool enabled;
        void* fn_ptr;
} isc_cron_t;

int8_t cron_tz = 0;
static uint32_t next_id;
static isc_cron_t *cron_list;
static TimerHandle_t cron_timer_hndl;
static QueueHandle_t cron_queue;
static TaskHandle_t cron_task_hndl;

static void isc_cron_execute_task(void *pvParameters) {
    void (*cron_fn)(void);
    while (1) {
        if (cron_queue) {
            if (xQueueReceive(cron_queue, &cron_fn, (portTickType) portMAX_DELAY)) {
                cron_fn();
            }
        }
    }
}

static void isc_cron_timer(TimerHandle_t xTimer) {
    time_t now = 0;

    time(&now);
    now += 3600 * cron_tz;

    ll_foreach(cron_list, item)
    {
        if (item != NULL && item->enabled) {
            if (difftime(item->next, now) <= 0) {
                item->next = cron_next(&item->expr, now);
                xQueueSend(cron_queue, &(item->fn_ptr), (portTickType)10);
                break;
            }
        }
    }
}

esp_err_t isc_cron_start(int8_t tz) {
    ESP_LOGI(TAG, "isc_cron_start");
    next_id = 0;
    cron_list = NULL;
    cron_tz = tz;
    cron_queue = xQueueCreate(CRON_QUEUE_LEN, sizeof(uint32_t));

    ESP_LOGI(TAG, "start cron timer");
    cron_timer_hndl = xTimerCreate(
            "isc_cron_timer",
            pdMS_TO_TICKS(CRON_TIMER_MS),
            pdTRUE,
            (void*) 0,
            isc_cron_timer
            );

    if (xTimerStart(cron_timer_hndl, 0) != pdPASS) {
        ESP_LOGI(TAG, "error starting timer");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "start cron task");
    xTaskCreate(
                isc_cron_execute_task,
                "isc_cron_execute_task",
                4096,
                NULL,
                12,
                &cron_task_hndl
                );

    return ESP_OK;
}

void isc_cron_stop(void) {
    xTimerStop(cron_timer_hndl, 0);
    vTaskDelete(cron_task_hndl);
    vQueueDelete(cron_queue);
    ll_free(cron_list);
}

int32_t isc_cron_add(char *expr, void* isc_cron_cb_ptr) {
    const char* err = NULL;
    cron_expr parsed;
    time_t now = 0;

    time(&now);
    now += 3600 * cron_tz;

    cron_parse_expr(expr, &parsed, &err);
    if (err != NULL)
        return ESP_FAIL;

    cron_list = ll_new(cron_list);
    memcpy(&(cron_list->expr), &parsed, sizeof(cron_expr));
    cron_list->enabled = true;
    cron_list->id = next_id++;
    cron_list->next = cron_next(&parsed, now);
    cron_list->fn_ptr = isc_cron_cb_ptr;
    return cron_list->id;
}

esp_err_t isc_cron_enable(uint32_t id, bool enable) {
    bool found = false;

    ll_foreach(cron_list, item) {
        if (item != NULL && item->id == id) {
            item->enabled = enable;
            found = true;
            break;
        }
    }

    return found ? ESP_OK : ESP_FAIL;
}

/*
esp_err_t isc_cron_delete(uint32_t id) {
    bool found = false;

    ll_foreach(cron_list, item)
    {
        if (item != NULL && item->id == id) {
            ESP_LOGI(TAG, "delete id=%d", id);
            ll_pop(item);
            found = true;
            break;
        }
    }
    return found ? ESP_OK : ESP_FAIL;
}
*/

void isc_cron_list(void) {
    ESP_LOGI(TAG, "cron list");

    ll_foreach(cron_list, item)
    {
        if (item != NULL)
            ESP_LOGI(TAG, "id=%d enabled=%d cb=%p", item->id, item->enabled, item->fn_ptr);
    }
}
