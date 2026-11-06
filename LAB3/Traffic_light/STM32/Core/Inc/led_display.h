#ifndef INC_LED_DISPLAY_H_
#define INC_LED_DISPLAY_H_

void display7SEG(int num);
void update7SEG(int index);
void updateClockBuffer(int val1, int val2);
void scan7SEG_run(void);

void turnOffAllLeds(void);
void setTrafficLightRoad1(int red, int yellow, int green);
void setTrafficLightRoad2(int red, int yellow, int green);
void blinkLedsMode(int current_mode);
#endif /* INC_LED_DISPLAY_H_ */
