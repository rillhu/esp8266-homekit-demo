/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#include <stdio.h>
#include <sdkconfig.h>

#ifdef CONFIG_IDF_TARGET_ESP8266
#include "driver/pwm.h"
#else

#endif
#include "esp_log.h"

typedef struct hsp
{
    uint16_t h; // 0-360
    uint16_t s; // 0-100
    uint16_t b; // 0-100
} hsp_t;

static hsp_t s_hsb_val;
static uint16_t s_brightness;
static bool s_on = false;

static const char *TAG = "lightbulb";

#define PWM_PERIOD (500)
#define PWM_IO_NUM 2

/*
// pwm pin number
const uint32_t PWM_OUT_IO_NUM = 5;

// dutys table, (duty/PERIOD)*depth
uint32_t duties = 250;

// phase table, (phase/180)*depth
float phase = 0;
*/

const uint32_t PWM_0_OUT_IO_NUM = 5;
const uint32_t PWM_1_OUT_IO_NUM = 15;

// pwm pin number
const uint32_t pin_num[PWM_IO_NUM] = {
    PWM_0_OUT_IO_NUM,
    PWM_1_OUT_IO_NUM
};

// duties table, real_duty = duties[x]/PERIOD
uint32_t duties[PWM_IO_NUM] = {
    250, 250
};

// phase table, delay = (phase[x]/360)*PERIOD
float phase[PWM_IO_NUM] = {
    0, 0
};

/**
 * @brief initialize the lightbulb lowlevel module
 */
void lightbulb_init(void)
{
    pwm_init(PWM_PERIOD, duties, PWM_IO_NUM, pin_num);
    //pwm_set_channel_invert(0x1 << 0);
    pwm_set_phases(phase);
    pwm_start();
}

/**
 * @brief deinitialize the lightbulb's lowlevel module
 */
void lightbulb_deinit(void)
{
}

/**
 * @brief turn on/off the lowlevel lightbulb
 */
int lightbulb_set_on(bool value)
{
    ESP_LOGI(TAG, "lightbulb_set_on : %s", value == true ? "true" : "false");

    if (value == true)
    {
        s_hsb_val.b = s_brightness;
        s_on = true;
    }
    else
    {
        s_brightness = s_hsb_val.b;
        s_hsb_val.b = 0;
        s_on = false;
    }
    ESP_LOGI(TAG, "lightbulb_set_on : %d", s_hsb_val.b);
    pwm_stop(0x3);
    pwm_set_duty(0, duties[0] * s_hsb_val.b / 100);
    pwm_set_duty(1, duties[1] * s_hsb_val.b / 100);
    pwm_start();

    return 0;
}

/**
 * @brief set the brightness of the lowlevel lightbulb
 */
int lightbulb_set_brightness(int value)
{
    ESP_LOGI(TAG, "lightbulb_set_brightness : %d", value);

    s_hsb_val.b = value;
    s_brightness = s_hsb_val.b;
    if (true == s_on)
    {
        ESP_LOGI(TAG, "lightbulb_set_brightness 222 : %d", s_hsb_val.b);
        pwm_stop(0x3);
        pwm_set_duty(0, duties[0] * s_hsb_val.b / 100);
        pwm_set_duty(1, duties[1] * s_hsb_val.b / 100);
        pwm_start();
    }
    return 0;
}
