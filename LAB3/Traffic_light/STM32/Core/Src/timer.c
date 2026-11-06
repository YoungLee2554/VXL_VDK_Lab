#include "main.h"
#include "input_reading.h"
#include "timer.h"

#define MAX_TIMER 10

int timer_counter[MAX_TIMER];
int timer_flag[MAX_TIMER];

void setTimer(int index, int duration_ms) {
    timer_counter[index] = duration_ms / TIMER_CYCLE;
    timer_flag[index] = 0;
}

void timerRun(void) {
    for (int i = 0; i < MAX_TIMER; i++) {
        if (timer_counter[i] > 0) {
            timer_counter[i]--;
            if (timer_counter[i] <= 0) {
                timer_flag[i] = 1;
            }
        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        button_reading();
        timerRun();
    }
}
