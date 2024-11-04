/**
 * @file wse_led.h
 * @author Mike Ochtman
 * @brief This works exclusively in the HSV color gamut for the onboard LED
 * @version 0.1
 * @date 01 November 2024
 *
 * @copyright Copyright (c) 2024
 */
#ifndef WSE_LED_H
#define WSE_LED_H

#include "led_strip.h"

typedef union wse_led_hsv
{
    struct hsv_
    {
        uint8_t h;
        uint8_t s;
        uint8_t v;
    } hsv_;
    struct hsv_name
    {
        uint8_t hue;
        uint8_t saturation;
        uint8_t value;
    } hsv;
} wse_led_hsv_t;

typedef union wse_led_rgbw
{
    struct rgbw_
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t w;
    } rgbw_;
    struct grbw
    {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t white;
    } rgbw;
} wse_led_grbw_t;


typedef enum led_strip_backend
{
    BACKEND_UNKNOWN,
    BACKEND_RMT,
    BACKEND_SPI
} led_strip_backend_t;



/**
 * @brief this queue can be used to send color data to
 * the onboard led to signal program status or activity
 * The queue handles only wse_led_hsv_t values
 */
extern QueueHandle_t onboard_led_queue;



/**
 * @brief configures, creates, and returns an led_strip handle
 * that can be used to drive a strip of LEDs of any length
 * @param model one of LED_MODEL_SK6812 or LED_MODEL_WS2812
 * @param gpio gpio_num_t pin number
 * @param pixels number of LEDs in the string
 * @param backend one of BACKEND_RMT or BACKEND_SPI
 * @param with_dma 0 no DMA, 1 use DMA
 * @param invert invert the pixels
 * @param handle the led_strip_handle_t 
 * @return ESP_OK if successful
 */
esp_err_t wse_led_strip_config(
    led_model_t model,
    gpio_num_t gpio,
    uint32_t pixels,
    led_strip_backend_t backend,
    int with_dma,
    int invert,
    led_strip_handle_t *handle);


/**
 * @brief starts up the onboard LED process and returns
 * the address of the pre-configured led color queue
 * QueueHandle_t onboard_led_queue that accepts wse_led_hsv_t
 * color information to display
 * 
 * @param use_DMA  0 no DMA, 1 use DMA
 * @param queue address of the onboard_led_queue
 * @return ESP_OK if successful. ESP_INVALID_STATE 
 * if configuration failed, and ESP_ERR_NOT_FOUND if
 * the queue is not available
 */
esp_err_t wse_onboard_led_start(int use_DMA, QueueHandle_t queue);



#endif // WSE_LED_H