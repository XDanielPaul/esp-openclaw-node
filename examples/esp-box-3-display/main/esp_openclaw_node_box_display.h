/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

/** @brief Maximum UTF-8 heading length accepted by `display.show`. */
#define ESP_OPENCLAW_NODE_BOX_DISPLAY_MAX_HEADING_LEN 64
/** @brief Maximum UTF-8 body length accepted by `display.show`. */
#define ESP_OPENCLAW_NODE_BOX_DISPLAY_MAX_TEXT_LEN 512

typedef struct _lv_display_t lv_display_t;
typedef struct _lv_obj_t lv_obj_t;

/**
 * @brief Display state shared by the ESP-BOX-3 example modules.
 */
typedef struct {
    bool ready; /**< Whether the display runtime has been initialized successfully. */
    char heading[ESP_OPENCLAW_NODE_BOX_DISPLAY_MAX_HEADING_LEN + 1]; /**< Last rendered heading text. */
    char text[ESP_OPENCLAW_NODE_BOX_DISPLAY_MAX_TEXT_LEN + 1]; /**< Last rendered body text. */
    uint32_t render_count; /**< Number of successful render operations since boot. */
    int64_t last_render_ms; /**< Timestamp of the most recent render in milliseconds since boot. */
    lv_display_t *lv_display; /**< Underlying LVGL display handle owned by the example. */
    lv_obj_t *container; /**< Root LVGL container for the example screen. */
    lv_obj_t *heading_label; /**< LVGL label used for the heading line. */
    lv_obj_t *text_label; /**< LVGL label used for the body text block. */
} esp_openclaw_node_box_display_t;

/**
 * @brief Initialize the ESP-BOX-3 display runtime and render the boot screen.
 *
 * @param[out] display Display state to initialize.
 *
 * @return
 *      - `ESP_OK` on success
 *      - `ESP_ERR_INVALID_ARG` if `display` is `NULL`
 *      - an ESP-IDF error code if board or LVGL setup fails
 */
esp_err_t esp_openclaw_node_box_display_start(esp_openclaw_node_box_display_t *display);

/**
 * @brief Render new heading and body text on the display.
 *
 * @param[in,out] display Initialized display state.
 * @param[in] heading Short heading text.
 * @param[in] text Body text.
 *
 * @return
 *      - `ESP_OK` on success
 *      - `ESP_ERR_INVALID_ARG` if any argument is invalid
 *      - `ESP_ERR_INVALID_STATE` if the display is not ready
 */
esp_err_t esp_openclaw_node_box_display_render(
    esp_openclaw_node_box_display_t *display,
    const char *heading,
    const char *text);

/**
 * @brief Build the JSON payload returned by `display.status` and `display.show`.
 *
 * Ownership of the returned buffer transfers to the caller, which must free it
 * with a `malloc()`-compatible allocator.
 *
 * @param[in] display Display state to serialize.
 * @param[out] out_payload_json Allocated UTF-8 JSON string on success.
 *
 * @return
 *      - `ESP_OK` on success
 *      - `ESP_ERR_INVALID_ARG` if any argument is invalid
 *      - `ESP_ERR_NO_MEM` if allocation fails
 */
esp_err_t esp_openclaw_node_box_display_build_status_payload(
    const esp_openclaw_node_box_display_t *display,
    char **out_payload_json);
