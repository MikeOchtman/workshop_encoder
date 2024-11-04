/**
 * @file wse_color_table.h
 * @author Mike Ochtman 
 * @brief 
 * @version 0.1
 * @date 29 October 2024
 * 
 * @copyright Copyright (c) 2024
 */

#ifndef WSE_COLOR_TABLE_H
#define WSE_COLOR_TABLE_H


typedef enum wse_led_hues_numbers {
    NO_COLOR_SELECTED = -1,
    RED_0 = 0,
    ORANGE_30 = 30,
    YELLOW_60 = 60,
    CHARTREUSE_90 = 90,
    GREEN_120 = 120,
    SPRING_GREEN_150 = 150,
    CYAN_180 = 180,
    AZURE_210 = 210,
    BLUE_240 = 240,
    VIOLET_270 = 270,
    MAGENTA_300 = 300,
    ROSE_330 = 330,
    RED_360 = 360
} wse_led_hue_number_t;


typedef enum wse_led_hue {
    NO_COLOR = -1,
    RED = 0,
    ORANGE = 30,
    YELLOW = 60,
    CHARTREUSE = 90,
    GREEN = 120,
    SPRING_GREEN = 150,
    CYAN = 180,
    AZURE = 210,
    BLUE = 240,
    VIOLET = 270,
    MAGENTA = 300,
    ROSE = 330
    //RED = 360
} wse_led_hue_t;

#endif