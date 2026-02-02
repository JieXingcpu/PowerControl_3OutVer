#ifndef __BUZZER_H
#define __BUZZER_H

#define tone1_2kHz_arr 35999
#define tone1_2kHz_ccr 18000
#define tone2_2_2kHz_arr 32070
#define tone2_2_2kHz_ccr 16035
#define tone3_2_4kHz_arr 24031
#define tone3_2_4kHz_ccr 12016

typedef enum 
{
  BUZZER_OFF,
  BUZZER_SYSTEM_INIT,
  BUZZER_WARNING,
}Buzzer_State;

void Buzzer_Init(void);
void Buzzer_Switch(Buzzer_State state);

#endif
