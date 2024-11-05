| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Workshop Incremental Encoder Tester Project

This is being developed to test HTL incremental rotary encoders for machines in our workshop.
This is a private project you are welcome to follow along with.

## Advanced esp-idf CMake and user-developed components
The project is structured to use user-defined components to break the project up into logical sections.
We are using esp-idf recommended component structure, i.e.
```
${WorkspaceFolder}/components
                    -----| component-name
                          -------| include
                                   component-name.h
                                   other.h
                                   another.h
                          CMakeLists.txt
                          Kconfig.projbuild
                          source1.c
                          source2.c
                          ...

```
## Managed Components from the esp-idf component registry
We are drawing on led_strip and lvgl libraries, esp-idf keeps them separate in 'managed components' folder.
## Full error checking
We do not use the ubiquitous 'ESP_ERROR_CHECK()' macro.
We do proper error checking and graceful error recoveries

### Hardware Required

An ESP32-S3 board with integrated WS2812 addressable LED
A KY-040 rotary dial
A 24V interface board to read 24V HTL A/!A B/!B N/!N signals from the encoder under test
A display. We are using I2C SSD1306 128x64  

### Configure the Project
