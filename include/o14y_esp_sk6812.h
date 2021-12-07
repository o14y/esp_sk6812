// (c) 2021 o14y
#pragma once
#include "esp_err.h"
#include "hal/gpio_types.h"
#include "hal/rmt_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef void* o14y_esp_sk6812_t;
esp_err_t o14y_esp_sk6812_init(o14y_esp_sk6812_t* context, gpio_num_t gpio_num, rmt_channel_t channel);
esp_err_t o14y_esp_sk6812_clear(o14y_esp_sk6812_t context);
esp_err_t o14y_esp_sk6812_set(o14y_esp_sk6812_t context, int g8r8b8);
#ifdef __cplusplus
}
#endif