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


void test_cron_cb(void) {
    ESP_LOGI(TAG, "test_cron_cb");
}

void app_main() {
    int32_t cronid1, cronid2;

    ESP_LOGI(TAG, "cron start");
    isc_cron_start(-3);
    ESP_LOGI(TAG, "cron add");
    cronid1 = isc_cron_add("*/5 * * * * *", (void* ) &test_cron_cb);
    cronid2 = isc_cron_add("*/6 * * * * *", (void* ) &test_cron_cb);
    isc_cron_list();
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "cron disable");
    isc_cron_enable(cronid2, false);
    isc_cron_list();
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "cron stop");
    isc_cron_stop();

    while (1)
        vTaskDelay(5000 / portTICK_PERIOD_MS);
}
