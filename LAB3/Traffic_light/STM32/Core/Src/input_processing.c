#include "global.h"

enum ButtonState {
    BUTTON_RELEASED,
    BUTTON_PRESSED,
    BUTTON_PRESSED_MORE_THAN_1_SECOND
};

static enum ButtonState buttonState[3] = {BUTTON_RELEASED, BUTTON_RELEASED, BUTTON_RELEASED};

enum TrafficState {
    INIT_TRAFFIC,
    RED1_GREEN2,
    RED1_AMBER2,
    GREEN1_RED2,
    AMBER1_RED2
};

static enum TrafficState trafficState = INIT_TRAFFIC;
static int count_road1 = 0;
static int count_road2 = 0;
static int temp_value = 1;

static void handle_button_press(int btn_index) {
    if (btn_index == 0) {
        mode++;
        if (mode > MODE_4_GREEN) {
            mode = MODE_1_NORMAL;
        }

        turnOffAllLeds();

        if (mode == MODE_1_NORMAL) {
            trafficState = INIT_TRAFFIC;
        } else if (mode == MODE_2_RED) {
            temp_value = time_red;
            updateClockBuffer(temp_value, MODE_2_RED);
            setTimer(2, 250);
        } else if (mode == MODE_3_AMBER) {
            temp_value = time_amber;
            updateClockBuffer(temp_value, MODE_3_AMBER);
            setTimer(2, 250);
        } else if (mode == MODE_4_GREEN) {
            temp_value = time_green;
            updateClockBuffer(temp_value, MODE_4_GREEN);
            setTimer(2, 250);
        }
    }
    else if (btn_index == 1) {
        if (mode != MODE_1_NORMAL) {
            temp_value++;
            if (temp_value > 99) {
                temp_value = 1;
            }
            updateClockBuffer(temp_value, mode);
        }
    }
    else if (btn_index == 2) {
        if (mode == MODE_2_RED) {
            time_red = temp_value;
            if (time_red < 2) {
                time_red = 2;
            }
            if (time_red > time_amber) {
                time_green = time_red - time_amber;
            } else {
                time_green = 1;
                time_amber = time_red - time_green;
            }
            temp_value = time_red;
            updateClockBuffer(temp_value, MODE_2_RED);
        } else if (mode == MODE_3_AMBER) {
            time_amber = temp_value;
            if (time_amber >= 99) {
                time_amber = 98;
            }
            if (time_green + time_amber > 99) {
                time_red = 99;
                time_green = time_red - time_amber;
            } else {
                time_red = time_green + time_amber;
            }
            temp_value = time_amber;
            updateClockBuffer(temp_value, MODE_3_AMBER);
        } else if (mode == MODE_4_GREEN) {
            time_green = temp_value;
            if (time_green >= 99) {
                time_green = 98;
            }
            if (time_green + time_amber > 99) {
                time_red = 99;
                time_amber = time_red - time_green;
            } else {
                time_red = time_green + time_amber;
            }
            temp_value = time_green;
            updateClockBuffer(temp_value, MODE_4_GREEN);
        }
    }
}

static void fsm_traffic_normal(void) {
    switch (trafficState) {
        case INIT_TRAFFIC:
            count_road1 = time_red;
            count_road2 = time_green;
            setTrafficLightRoad1(1, 0, 0);
            setTrafficLightRoad2(0, 0, 1);
            updateClockBuffer(count_road1, count_road2);
            setTimer(0, 1000);
            trafficState = RED1_GREEN2;
            break;

        case RED1_GREEN2:
            if (timer_flag[0] == 1) {
                setTimer(0, 1000);
                count_road1--;
                count_road2--;
                if (count_road2 <= 0) {
                    count_road2 = time_amber;
                    setTrafficLightRoad1(1, 0, 0);
                    setTrafficLightRoad2(0, 1, 0);
                    trafficState = RED1_AMBER2;
                }
                updateClockBuffer(count_road1, count_road2);
            }
            break;

        case RED1_AMBER2:
            if (timer_flag[0] == 1) {
                setTimer(0, 1000);
                count_road1--;
                count_road2--;
                if (count_road1 <= 0 || count_road2 <= 0) {
                    count_road1 = time_green;
                    count_road2 = time_red;
                    setTrafficLightRoad1(0, 0, 1);
                    setTrafficLightRoad2(1, 0, 0);
                    trafficState = GREEN1_RED2;
                }
                updateClockBuffer(count_road1, count_road2);
            }
            break;

        case GREEN1_RED2:
            if (timer_flag[0] == 1) {
                setTimer(0, 1000);
                count_road1--;
                count_road2--;
                if (count_road1 <= 0) {
                    count_road1 = time_amber;
                    setTrafficLightRoad1(0, 1, 0);
                    setTrafficLightRoad2(1, 0, 0);
                    trafficState = AMBER1_RED2;
                }
                updateClockBuffer(count_road1, count_road2);
            }
            break;

        case AMBER1_RED2:
            if (timer_flag[0] == 1) {
                setTimer(0, 1000);
                count_road1--;
                count_road2--;
                if (count_road1 <= 0 || count_road2 <= 0) {
                    count_road1 = time_red;
                    count_road2 = time_green;
                    setTrafficLightRoad1(1, 0, 0);
                    setTrafficLightRoad2(0, 0, 1);
                    trafficState = RED1_GREEN2;
                }
                updateClockBuffer(count_road1, count_road2);
            }
            break;

        default:
            trafficState = INIT_TRAFFIC;
            break;
    }
}

void traffic_light_init(void) {
    turnOffAllLeds();
    setTimer(1, 100);
    setTimer(2, 250);
    mode = MODE_1_NORMAL;
    trafficState = INIT_TRAFFIC;
}

void fsm_for_input_processing(void) {
    for (int i = 0; i < 3; i++) {
        switch (buttonState[i]) {
            case BUTTON_RELEASED:
                if (is_button_pressed(i)) {
                    buttonState[i] = BUTTON_PRESSED;
                    handle_button_press(i);
                }
                break;

            case BUTTON_PRESSED:
                if (!is_button_pressed(i)) {
                    buttonState[i] = BUTTON_RELEASED;
                } else {
                    if (is_button_pressed_1s(i)) {
                        buttonState[i] = BUTTON_PRESSED_MORE_THAN_1_SECOND;
                        if (i == 1) {
                            setTimer(3, 500);
                        }
                    }
                }
                break;

            case BUTTON_PRESSED_MORE_THAN_1_SECOND:
                if (!is_button_pressed(i)) {
                    buttonState[i] = BUTTON_RELEASED;
                } else {
                    if (i == 1 && timer_flag[3] == 1) {
                        setTimer(3, 500);
                        handle_button_press(1);
                    }
                }
                break;
        }
    }

    if (mode == MODE_1_NORMAL) {
        fsm_traffic_normal();
    } else {
        blinkLedsMode(mode);
    }

    scan7SEG_run();
}
