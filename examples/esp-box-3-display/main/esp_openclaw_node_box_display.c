/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openclaw_node_box_display.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "bsp/esp-box-3.h"
#include "cJSON.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "lvgl.h"
#include "esp_openclaw_node_example_json.h"

static const char *TAG = "esp_openclaw_node_box_display";
static const char *DEFAULT_HEADING = "OpenClaw";
static const char *DEFAULT_TEXT = "Waiting for display.show from the OpenClaw gateway.";

esp_err_t esp_openclaw_node_box_display_build_status_payload(
    const esp_openclaw_node_box_display_t *display,
    char **out_payload_json)
{
    if (display == NULL || out_payload_json == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    cJSON *payload = cJSON_CreateObject();
    if (payload == NULL) {
        return ESP_ERR_NO_MEM;
    }

    cJSON_AddBoolToObject(payload, "ready", display->ready);
    cJSON_AddStringToObject(payload, "heading", display->heading);
    cJSON_AddStringToObject(payload, "text", display->text);
    cJSON_AddNumberToObject(payload, "renderCount", display->render_count);
    cJSON_AddNumberToObject(payload, "lastRenderMs", (double)display->last_render_ms);
    cJSON_AddNumberToObject(payload, "headingMaxLength", ESP_OPENCLAW_NODE_BOX_DISPLAY_MAX_HEADING_LEN);
    cJSON_AddNumberToObject(payload, "textMaxLength", ESP_OPENCLAW_NODE_BOX_DISPLAY_MAX_TEXT_LEN);
    cJSON_AddNumberToObject(payload, "width", BSP_LCD_H_RES);
    cJSON_AddNumberToObject(payload, "height", BSP_LCD_V_RES);

    return esp_openclaw_node_example_take_json_payload(payload, out_payload_json);
}

static void apply_render_locked(esp_openclaw_node_box_display_t *display)
{
    lv_label_set_text(display->heading_label, display->heading);
    lv_label_set_text(display->text_label, display->text);
}

esp_err_t esp_openclaw_node_box_display_render(
    esp_openclaw_node_box_display_t *display,
    const char *heading,
    const char *text)
{
    if (display == NULL || heading == NULL || text == NULL || !display->ready) {
        return ESP_ERR_INVALID_STATE;
    }
    if (!bsp_display_lock(0)) {
        return ESP_ERR_TIMEOUT;
    }

    snprintf(display->heading, sizeof(display->heading), "%s", heading);
    snprintf(display->text, sizeof(display->text), "%s", text);
    apply_render_locked(display);
    bsp_display_unlock();

    display->render_count += 1U;
    display->last_render_ms = esp_timer_get_time() / 1000LL;
    return ESP_OK;
}

static void create_display_ui_locked(esp_openclaw_node_box_display_t *display)
{
    lv_obj_t *screen = lv_scr_act();
    lv_obj_clean(screen);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x0f172a), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    display->container = lv_obj_create(screen);
    lv_obj_set_size(display->container, lv_pct(100), lv_pct(100));
    lv_obj_center(display->container);
    lv_obj_set_style_radius(display->container, 0, 0);
    lv_obj_set_style_border_width(display->container, 0, 0);
    lv_obj_set_style_bg_opa(display->container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_left(display->container, 18, 0);
    lv_obj_set_style_pad_right(display->container, 18, 0);
    lv_obj_set_style_pad_top(display->container, 20, 0);
    lv_obj_set_style_pad_bottom(display->container, 20, 0);
    lv_obj_set_style_pad_row(display->container, 12, 0);
    lv_obj_set_scrollbar_mode(display->container, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_flex_flow(display->container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(
        display->container,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_START);

    display->heading_label = lv_label_create(display->container);
    lv_obj_set_width(display->heading_label, lv_pct(100));
    lv_label_set_long_mode(display->heading_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_color(display->heading_label, lv_color_hex(0xf8fafc), 0);
    lv_obj_set_style_text_font(display->heading_label, &lv_font_montserrat_22, 0);

    display->text_label = lv_label_create(display->container);
    lv_obj_set_width(display->text_label, lv_pct(100));
    lv_label_set_long_mode(display->text_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_color(display->text_label, lv_color_hex(0xcbd5e1), 0);
    lv_obj_set_style_text_line_space(display->text_label, 6, 0);
    lv_obj_set_style_text_font(display->text_label, &lv_font_montserrat_16, 0);
}

esp_err_t esp_openclaw_node_box_display_start(esp_openclaw_node_box_display_t *display)
{
    if (display == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(display, 0, sizeof(*display));
    ESP_RETURN_ON_ERROR(bsp_i2c_init(), TAG, "bsp_i2c_init failed");

    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size = BSP_LCD_H_RES * CONFIG_BSP_LCD_DRAW_BUF_HEIGHT,
        .double_buffer = 0,
        .flags = {
            .buff_dma = true,
        },
    };
    display->lv_display = bsp_display_start_with_config(&cfg);
    if (display->lv_display == NULL) {
        return ESP_FAIL;
    }

    ESP_RETURN_ON_ERROR(bsp_display_backlight_on(), TAG, "bsp_display_backlight_on failed");

    if (!bsp_display_lock(0)) {
        return ESP_ERR_TIMEOUT;
    }
    create_display_ui_locked(display);
    bsp_display_unlock();

    display->ready = true;
    display->last_render_ms = 0;
    return esp_openclaw_node_box_display_render(display, DEFAULT_HEADING, DEFAULT_TEXT);
}
