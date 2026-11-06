#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#define TIMER_CYCLE 10

extern int timer_flag[];

void setTimer(int index, int duration_ms);
void timerRun(void);

#endif /* INC_TIMER_H_ */
