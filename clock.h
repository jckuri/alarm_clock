#ifndef CLOCK_H
#define CLOCK_H

#include <cstdint>
#include <Arduino.h>
#include "tm1637.h"
#include "alarm_tone.h"

enum ButtonType
{
  BUTTON_MENU,
  BUTTON_PLUS,
  BUTTON_MINUS,
  BUTTON_OK,
};

enum SystemStates {
  SHOW_CLOCK,
  SET_HOUR,
  SET_MINUTE,
  SET_HOUR_ALARM,
  SET_MINUTE_ALARM,
  ALARMING,
  MENU_SET,
  MENU_ALARM,
  ALARM_OFF,
};

class Clock
{
public:
    // 7-segment Display
    TM1637* display = NULL;

    // Timer variable to count time
    hw_timer_t *timer = NULL;

    // TODO: Add other variables here
    uint8_t middle_colon = 0;
    uint8_t alarm_activated = 1;
    uint8_t system_state = SHOW_CLOCK;
    uint8_t alarm_off_counter = 0;
    uint8_t alarming_counter = 0;
    uint32_t time, alarm;
    AlarmTone *alarm_tone;
    //int hour, minute, second;
    //int alarm_hour, alarm_minute;

    // Constructor
    Clock();

    // Init function
    void init(TM1637* display, uint8_t buzzer_pin);
    
    // Set time and alarm time
    void set_time(uint8_t hour, uint8_t minutes, uint8_t seconds);
    void set_alarm(uint8_t hour, uint8_t minutes);
    
    // Button pressed function
    void button_pressed(ButtonType type);
    
    // Alarm functions
    void turn_alarm(bool on_off);
    void check_alarm();

    // Clock functions 
    void show();
    void run();

    // TODO: Add other public variables/functions here
    void setup_timer_interrupt();   
    void count_second();
    void show_internal_time(uint32_t internal_time, uint8_t internal_middle_colon, uint8_t show_hour, uint8_t show_minute);
    void change_time(int8_t delta_hour, int8_t delta_minute);
    void change_alarm(int8_t delta_hour, int8_t delta_minute);
};

#endif