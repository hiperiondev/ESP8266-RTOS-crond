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

#ifndef _ISC_CRON_H_
#define _ISC_CRON_H_

#include "isc_common.h"

esp_err_t isc_cron_start(int8_t tz);
     void isc_cron_stop(void);
  int32_t isc_cron_add(char *expr, void* isc_cron_cb_ptr);
esp_err_t isc_cron_enable(uint32_t id, bool enable);
//esp_err_t isc_cron_delete(uint32_t id);
     void isc_cron_list(void);

#endif
