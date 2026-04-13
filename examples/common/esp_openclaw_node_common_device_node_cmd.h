/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_openclaw_node.h"

/**
 * @brief Register the common device commands shared by the examples.
 *
 * The helper registers `device.info`, `device.status`, and `wifi.status`.
 *
 * @param[in] node OpenClaw Node instance to extend.
 *
 * @return
 *      - `ESP_OK` on success
 *      - an ESP-IDF error code if registration fails
 */
esp_err_t esp_openclaw_node_common_register_device_node_commands(esp_openclaw_node_handle_t node);
