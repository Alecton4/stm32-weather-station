# IoT-Smart-Weather-Station

## Overall View

## Demo Video

## Features
- [ ] Temperature measurement using BME280 (I2C).
- [ ] Humidity measurement using BME280 (I2C).
- [ ] Barometric pressure measurement using BME280 (I2C).
- [ ] UV measurement using LTR390-UV-01 (I2C).
- [ ] Ambient light measurement using LTR390-UV-01 (I2C).
- [ ] Ambient sound detection using LM386 (ADC).
- [ ] PM2.5 measurement using GP2Y1014AU0F (ADC).
- [ ] GUI and touch control (FSMC, SPI).
- [ ] LCD auto-adjusts brightness according to ambient light (I2C, PWM).
- [ ] Camera streaming and photo taking. Photos can be displayed on LCD and stored in SD card (SCCB, SDIO).
- [ ] ESP8266 Wi-Fi connection (UART, AT Commands).
- [ ] Real time clock is enabled, and the board can synchronize time with cloud server through Internet (RTC, SNTP).
- [ ] Internet-of-Things. The board can communicate with cloud server through Internet via TCP protocol. It can send data to the server and receive command from the server (UART, TCP, AT Commands).
- [ ] Hot-swapping pins. There are some pins used by both camera ov7725 and ESP8266. The system can configure the correct states for the pin for each component in run time (UART, GPIO).
- [ ] RTOS integration.
- [ ] System crash detection and auto reboot (IWDG).

## CubeMX Config

## Dependencies
- RT-Thread (RTOS): https://github.com/RT-Thread/rt-thread
- LVGL (GUI library): https://github.com/lvgl/lvgl

## How to build
See [RT-Thread documentation](https://www.rt-thread.org/document/site/#/development-tools/env/env).