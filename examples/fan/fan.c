#include <stdio.h>
#include <espressif/esp_wifi.h>
#include <espressif/esp_sta.h>
#include <esp/uart.h>
#include <esp8266.h>
#include <FreeRTOS.h>
#include <task.h>

#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include "wifi.h"

#include <pwm.h>



static void wifi_init() {
    struct sdk_station_config wifi_config = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASSWORD,
    };

    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&wifi_config);
    sdk_wifi_station_connect();
}

// Fan
const int pwm_gpio = 16;
float pwm_value = 0;
uint8_t pins[1];

// Light
const int led_gpio = 2;
bool led_on = false;

void led_write(bool on) {
    gpio_write(led_gpio, on ? 0 : 1);
}

void led_init() {
    // Light
    gpio_enable(led_gpio, GPIO_OUTPUT);
    led_write(led_on);

    // Fan
    pins[0] = pwm_gpio;
    pwm_init(1, pins, false);
    printf("on = false  bri = 0 %%\n");
    pwm_set_freq(1000);
    printf("PWM Frequency = 1000 Hz  Duty Cycle = 0%%\n");
    pwm_set_duty(50);
    pwm_start();
}

void led_identify_task(void *_args) {
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            led_write(true);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            led_write(false);
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }

        vTaskDelay(250 / portTICK_PERIOD_MS);
    }

    led_write(led_on);

    vTaskDelete(NULL);
}

void led_identify(homekit_value_t _value) {
    printf("LED identify\n");
    xTaskCreate(led_identify_task, "LED identify", 128, NULL, 2, NULL);
}

// Light

homekit_value_t led_on_get() { return HOMEKIT_BOOL(led_on); }

void led_on_set(homekit_value_t value) {
    if (value.format != homekit_format_bool) {
        printf("Invalid value format: %d\n", value.format);
        return;
    }

    led_on = value.bool_value;
    led_write(led_on);
}

// Fan

homekit_value_t light_bri_get() { return HOMEKIT_FLOAT(pwm_value); }

void light_bri_set(homekit_value_t value) {
    if (value.format != homekit_format_float) {
        printf("Invalid pwm_value format: %d\n", value.format);
        return;
    }
    //bri = value.int_value;
    
    pwm_value = value.float_value;
    int pwm_value_reg = (UINT16_MAX - UINT16_MAX*pwm_value/100);
    pwm_set_duty(pwm_value_reg);
    printf("Duty cycle set to %3d [%5d]\n", (int)pwm_value , pwm_value_reg);
}


homekit_accessory_t *accessories[] = {
    HOMEKIT_ACCESSORY(.id=1, .category=homekit_accessory_category_lightbulb, .services=(homekit_service_t*[]){
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]){
            HOMEKIT_CHARACTERISTIC(NAME, "Fan Light and Dimmer Control"),
            HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Always Hardcore Design"),
            HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "AHD_FLDC_MASTER"),
            HOMEKIT_CHARACTERISTIC(MODEL, "Master Fan Light and Dimmer Control"),
            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.1"),
            HOMEKIT_CHARACTERISTIC(IDENTIFY, led_identify),
            NULL
        }),
        HOMEKIT_SERVICE(LIGHTBULB, .primary=true, .characteristics=(homekit_characteristic_t*[]){
            HOMEKIT_CHARACTERISTIC(NAME, "Master Light"),
            HOMEKIT_CHARACTERISTIC(ON, false, .getter=led_on_get, .setter=led_on_set),
            // HOMEKIT_CHARACTERISTIC(BRIGHTNESS, 100, .getter=light_bri_get, .setter=light_bri_set),
            NULL
        }),
        HOMEKIT_SERVICE(FAN, .characteristics=(homekit_characteristic_t*[]){
            HOMEKIT_CHARACTERISTIC(NAME, "Master Fan"),
            HOMEKIT_CHARACTERISTIC(ON, true),
            HOMEKIT_CHARACTERISTIC(ROTATION_SPEED, 100, .getter=light_bri_get, .setter=light_bri_set),
            NULL
        }),
        NULL
    }),
    NULL
};

homekit_server_config_t config = {
    .accessories = accessories,
    .password = "333-33-333",
    .setupId="3333"
};

void user_init(void) {
    uart_set_baud(0, 115200);

    wifi_init();
    led_init();
    homekit_server_init(&config);
}
