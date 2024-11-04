/**
 * @file wse_led.c
 * @author Mike Ochtman
 * @brief
 * @version 0.1
 * @date 01 November 2024
 *
 * @copyright Copyright (c) 2024
 */

#include "wse_led.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "led_strip.h"
#include "wse_color_table.h"

QueueHandle_t onboard_led_queue = NULL;
static led_strip_handle_t onboard_led_handle = NULL;

// MARK: wse_onboard_led_config
/**
 * @brief configures the onboard led specifically by selecting
 * options from the Kconfig project settings and SDKconfig from espressif
 * esp-idf framework.
 *
 * @param with_dma  0 no DMA, 1 use DMA
 * @return
 */
static esp_err_t wse_onboard_led_config(int with_dma)
{

    ESP_LOGI(__func__,"starting led config\n");

    led_strip_config_t onboard_led_config = {
        .flags.invert_out = 0,
#if defined CONFIG_WSE_LED_STRIP_TYPE_WS2812
        .led_model = LED_MODEL_WS2812,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
#elif defined CONFIG_WSE_LED_STRIP_TYPE_SK6812
        .led_model = LED_MODEL_SK6812,
        .led_pixel_format = LED_PIXEL_FORMAT_GRBW,
#else
#warning select led model type in SDKconfig. Defaulting to WS2812
        .led_model = LED_MODEL_WS2812,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
#endif
        .max_leds = 1,
        .strip_gpio_num = CONFIG_WSE_LED_STRIP_GPIO};

    esp_err_t err = ESP_OK;

#if defined CONFIG_WSE_LED_STRIP_BACKEND_RMT

    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10e6,
        .flags.with_dma = (with_dma == 0) ? 0 : 1,
        .mem_block_symbols = 0,
        .clk_src = RMT_CLK_SRC_DEFAULT};

    err = led_strip_new_rmt_device(
        &onboard_led_config,
        &rmt_config,
        &onboard_led_handle);

    ESP_LOGI(__func__,"&onboard_led_handle = %lu\n", (unsigned long)&onboard_led_handle);
#elif defined CONFIG_WSE_LED_STRIP_BACKEND_SPI

    led_strip_spi_config_t spi_config = {
        .clk_src = SPI_CLK_SRC_DEFAULT,
        .flags.with_dma = (with_dma == 0) ? 0 : 1,
        .spi_bus = SPI_HOST_MAX - 1};

    err = led_strip_new_spi_device(
        &onboard_led_config,
        &spi_config,
        &onboard_led_handle);
#else
// problem! Stop compilation
#error Select an led-strip backend in SDKconfig
#endif

    if (ESP_OK != err)
    {
        ESP_LOGI(__func__, "onboard led config failed: err %d, %s\n", err, esp_err_to_name(err));
    };

    return err;
};

// MARK: wse_led_strip_config

esp_err_t wse_led_strip_config(
    led_model_t model,
    gpio_num_t gpio,
    uint32_t pixels,
    led_strip_backend_t backend,
    int with_dma,
    int invert,
    led_strip_handle_t *handle)
{

    led_strip_config_t strip_config = {
        .flags.invert_out = (invert == 0) ? 0 : 1,
        .led_model = model,
        .led_pixel_format = (model == LED_MODEL_WS2812) ? LED_PIXEL_FORMAT_GRB : LED_PIXEL_FORMAT_GRBW,
        .max_leds = pixels,
        .strip_gpio_num = gpio};

    esp_err_t err = ESP_OK;

    switch (backend)
    {
    case BACKEND_RMT:
        led_strip_rmt_config_t rmt_config = {
            .resolution_hz = 1e6,
            .flags.with_dma = (with_dma == 0) ? 0 : 1,
            .mem_block_symbols = 0,
            .clk_src = RMT_CLK_SRC_DEFAULT};

        err = led_strip_new_rmt_device(
            &strip_config,
            &rmt_config,
            &onboard_led_handle);
        break;

    case BACKEND_SPI:
        led_strip_spi_config_t spi_config = {
            .clk_src = SPI_CLK_SRC_DEFAULT,
            .flags.with_dma = (with_dma == 0) ? 0 : 1,
            .spi_bus = SPI_HOST_MAX - 1};

        err = led_strip_new_spi_device(
            &strip_config,
            &spi_config,
            &onboard_led_handle);
        break;

    default:

        ESP_LOGI(__func__, "Invalid backend selected\n");
    };

    if (ESP_OK != err)
    {
        ESP_LOGI(__func__, "onboard led config failed: err %d, %s\n", err, esp_err_to_name(err));
    };

    return err;
};

// MARK: onboard_led_task
/**
 * @brief this task waits for packets of wse_led_hsv_t data to set
 * the color of the onboard LED
 *
 * @param args NULL. Nothing useful can be sent on this yet
 */
static void onboard_led_task(void *args)
{

    // setup
    wse_led_hsv_t color;
    BaseType_t data_ready;
    esp_err_t err = ESP_OK;

    for (;;)
    {
        data_ready = xQueueReceive(onboard_led_queue, &color, portMAX_DELAY);
        if (data_ready)
        {
            err = led_strip_set_pixel_hsv(
                onboard_led_handle,
                0,
                color.hsv.hue,
                color.hsv.saturation,
                color.hsv.value);
            err += led_strip_refresh(onboard_led_handle);
            if (ESP_OK != err)
            {
                ESP_LOGI(__func__, "task failed to set color: err %d, %s\n", err, esp_err_to_name(err));
            };
        };
    };
    vTaskDelete(NULL);
};

// MARK: onboard_led_task_create
/**
 * @brief prepares the onboard LED after it has been configued to start
 * receiving wse_led_hsv_t data on the onboard_led_queue and leaves the
 * task running.
 * @param  void
 * @return ESP_OK if successfule; ESP_INVALID_STATE if there was a problem
 */
static esp_err_t onboard_led_task_create(void)
{

    esp_err_t err = ESP_OK;

    // start up the queue
    onboard_led_queue = xQueueCreate(8, sizeof(wse_led_hsv_t));
    if (!onboard_led_queue)
    {
        ESP_LOGI(__func__, "Queue create failed: err %d, %s\n", err, esp_err_to_name(err));
        return err;
    };

    TaskHandle_t onboard_led_task_handle = NULL;
    BaseType_t task_status = xTaskCreate(
        onboard_led_task,
        "led_task",
        4 * 256,
        NULL,
        1,
        &onboard_led_task_handle);

    if (pdPASS != task_status)
    {
        err = ESP_ERR_INVALID_STATE;
        ESP_LOGI(__func__, "Task create failed: err %d, %s\n", err, esp_err_to_name(err));
    }
    return err;
};

esp_err_t wse_onboard_led_start(int use_DMA, QueueHandle_t queue)
{

    esp_err_t err = ESP_OK;

    err = wse_onboard_led_config(use_DMA);
    if (ESP_OK != err)
    {
        ESP_LOGI(__func__, "led config failed: err %d, %s\n", err, esp_err_to_name(err));
        return err;
    };

    err = onboard_led_task_create();

    if (ESP_OK != err)
    {
        ESP_LOGI(__func__, "task creation failed: err %d, %s\n", err, esp_err_to_name(err));
        return err;
    };

    queue = onboard_led_queue;
    if (NULL == queue)
    {
        err = ESP_ERR_NOT_FOUND;
        ESP_LOGI(__func__, "led queue startup failed: err %d, %s\n", err, esp_err_to_name(err));
    }
    return err;
};
