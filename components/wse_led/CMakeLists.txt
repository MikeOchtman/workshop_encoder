idf_component_register(SRCS "wse_led.c"
                    INCLUDE_DIRS "include"
                    REQUIRES 
                    "esp_timer" 
                    "driver" 
                    "espressif__led_strip")
