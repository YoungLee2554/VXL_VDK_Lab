#include "global.h"

int led_buffer[4] = {0, 0, 0, 0};
static int index_led = 0;

void display7SEG(int num) {
    uint8_t seg7[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
    if (num < 0 || num > 9) return;
    uint8_t temp = seg7[num];
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, (temp >> 0) & 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, (temp >> 1) & 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, (temp >> 2) & 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, (temp >> 3) & 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, (temp >> 4) & 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, (temp >> 5) & 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, (temp >> 6) & 1);
}

void updateClockBuffer(int val1, int val2) {
    led_buffer[0] = val1 / 10;
    led_buffer[1] = val1 % 10;
    led_buffer[2] = val2 / 10;
    led_buffer[3] = val2 % 10;
}

void update7SEG(int index) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10, GPIO_PIN_SET);

    switch (index) {
        case 0:
            display7SEG(led_buffer[0]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
            break;
        case 1:
            display7SEG(led_buffer[1]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
            break;
        case 2:
            display7SEG(led_buffer[2]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
            break;
        case 3:
            display7SEG(led_buffer[3]);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
            break;
        default:
            break;
    }
}

void scan7SEG_run(void) {
    if (timer_flag[1] == 1) {
        setTimer(1, 100);
        update7SEG(index_led);
        index_led++;
        if (index_led >= 4) {
            index_led = 0;
        }
    }
}

void turnOffAllLeds(void) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 |
                             GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12, GPIO_PIN_SET);
}

void setTrafficLightRoad1(int red, int yellow, int green) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, red ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, yellow ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, green ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void setTrafficLightRoad2(int red, int yellow, int green) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, red ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, yellow ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, green ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void blinkLedsMode(int current_mode) {
    if (timer_flag[2] == 1) {
        setTimer(2, 250);
        if (current_mode == MODE_2_RED) {
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7 | GPIO_PIN_10);
        } else if (current_mode == MODE_3_AMBER) {
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8 | GPIO_PIN_11);
        } else if (current_mode == MODE_4_GREEN) {
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9 | GPIO_PIN_12);
        }
    }
}
