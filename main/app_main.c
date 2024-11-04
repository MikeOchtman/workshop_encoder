/**
 * @file app_main.c
 * @author Mike Ochtman 
 * @brief 
 * @version 0.1
 * @date 01 November 2024
 * 
 * @copyright Copyright (c) 2024
 */

#include "esp_log.h"
#include "wse_led.h"

// global vars



void app_main(void) {

esp_err_t err = ESP_OK;
wse_led_hsv_t led_color;
led_color.hsv.hue = 0;

err = wse_onboard_led_start(0,onboard_led_queue);

if (ESP_OK != err) 
{
    ESP_LOGI(__func__, " failed: err %d, %s\n", err, esp_err_to_name(err));

};


for(;;) {

    led_color.hsv.saturation = 240;
    led_color.hsv.value = 50;
    xQueueSend(onboard_led_queue,(void *)&led_color,2);
    led_color.hsv.hue++;
    vTaskDelay(20);
}


};
