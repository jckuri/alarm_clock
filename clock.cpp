#include "clock.h"
#include "stdio.h"


Clock *clock0;


// Static function: Update time, show things on display
//                  and check alarm trigger
static void update_time(void* clock) {
  // I used the function onTimer() instead.
}


void ARDUINO_ISR_ATTR onTimer() {
  clock0->middle_colon = !clock0->middle_colon;
  if(clock0->middle_colon) {
    clock0->count_second();    
    clock0->check_alarm();      
  } else {
  }
  clock0->show();
}


// Empty constructor
Clock::Clock() {}


// Clock::init(): Initialize internal variables,
//                set display to use and buzzer pin
void Clock::init(TM1637* display, uint8_t buzzer_pin)
{ 
  this->display = display;
  this->alarm_tone = new AlarmTone();
  this->alarm_tone->init(buzzer_pin);
}


// Clock::set_time(): Set the time hour, minutes and seconds
//                    to internal binary representation
void Clock::set_time(uint8_t hour, uint8_t minutes, uint8_t seconds)
{
  time = ((0b11111 & hour) << 12) + ((0b111111 & minutes) << 6) + ((0b111111 & seconds));
}


void Clock::count_second() {
  uint8_t hour = (time >> 12) & 0b11111, minute = (time >> 6) & 0b111111, second = time & 0b111111;
  if(second < 59) {
    second++;
  } else {
    second = 0;
    if(minute < 59) {
      minute++;
    } else {
      minute = 0;
      if(hour < 23) hour++; else hour = 0;
    }
  }
  set_time(hour, minute, second);
}


// Clock::set_alarm(): Set the alarm hour, minutes and seconds
//                     to internal binary representation
void Clock::set_alarm(uint8_t hour, uint8_t minutes)
{
  alarm = ((0b11111 & hour) << (6 + 6)) + ((0b111111 & minutes) << 6);
}


void Clock::change_time(int8_t delta_hour, int8_t delta_minute) {
  int8_t hour = (time >> 12) & 0b11111, minute = (time >> 6) & 0b111111;
  hour += delta_hour;
  if(hour < 0) hour = 23;
  if(hour > 23) hour = 0;
  minute += delta_minute;
  if(minute < 0) minute = 59;
  if(minute > 59) minute = 0;
  set_time(hour, minute, 0);
}


void Clock::change_alarm(int8_t delta_hour, int8_t delta_minute) {
  int8_t hour = (alarm >> 12) & 0b11111, minute = (alarm >> 6) & 0b111111;
  hour += delta_hour;
  if(hour < 0) hour = 23;
  if(hour > 23) hour = 0;
  minute += delta_minute;
  if(minute < 0) minute = 59;
  if(minute > 59) minute = 0;
  set_alarm(hour, minute);
}


// Clock::menu_pressed(): Tell the internal clock that a button was pressed
//                        type: The button that was pressed
void Clock::button_pressed(ButtonType type) 
{
  switch(system_state) {
    case SHOW_CLOCK:
      switch(type) {
        case BUTTON_MENU:
          system_state = MENU_SET;
          break;
        case BUTTON_OK:
          break;
        case BUTTON_PLUS:
          break;
        case BUTTON_MINUS:
          break;
      }
      break;
    case SET_HOUR:
      switch(type) {
        case BUTTON_MENU:
          break;
        case BUTTON_OK:
          system_state = SET_MINUTE;
          break;
        case BUTTON_PLUS:
          change_time(1, 0);
          break;
        case BUTTON_MINUS:
          change_time(-1, 0);
          break;
      }
      break;
    case SET_MINUTE:
      switch(type) {
        case BUTTON_MENU:
          break;
        case BUTTON_OK:
          system_state = SHOW_CLOCK;
          break;
        case BUTTON_PLUS:
          change_time(0, 1);
          break;
        case BUTTON_MINUS:
          change_time(0, -1);
          break;
      }
      break;
    case SET_HOUR_ALARM:
      switch(type) {
        case BUTTON_MENU:
          break;
        case BUTTON_OK:
          system_state = SET_MINUTE_ALARM;
          break;
        case BUTTON_PLUS:
          change_alarm(1, 0);
          break;
        case BUTTON_MINUS:
          change_alarm(-1, 0);
          break;
      }
      break;
    case SET_MINUTE_ALARM:
      switch(type) {
        case BUTTON_MENU:
          break;
        case BUTTON_OK:
          system_state = SHOW_CLOCK;
          break;
        case BUTTON_PLUS:
          change_alarm(0, 1);
          break;
        case BUTTON_MINUS:
          change_alarm(0, -1);
          break;
      }
      break;
    case ALARMING:
      switch(type) {
        case BUTTON_MENU:
          break;
        case BUTTON_OK:
          system_state = SHOW_CLOCK;
          alarming_counter = 0;
          break;
        case BUTTON_PLUS:
          break;
        case BUTTON_MINUS:
          break;
      }
      break;
    case MENU_SET:
      switch(type) {
        case BUTTON_MENU:
          system_state = MENU_ALARM;
          break;
        case BUTTON_OK:
          system_state = SET_HOUR;
          break;
        case BUTTON_PLUS:
          break;
        case BUTTON_MINUS:
          break;
      }
      break;
    case MENU_ALARM:
      switch(type) {
        case BUTTON_MENU:
          system_state = SHOW_CLOCK;
          break;
        case BUTTON_OK:
          if(alarm_activated) {
            system_state = SET_HOUR_ALARM;
          } else {
            system_state = ALARM_OFF;
            alarm_off_counter = 6;
          }
          break;
        case BUTTON_PLUS:
          break;
        case BUTTON_MINUS:
          break;
      }
      break;
    case ALARM_OFF:
      switch(type) {
        case BUTTON_MENU:
          break;
        case BUTTON_OK:
          break;
        case BUTTON_PLUS:
          break;
        case BUTTON_MINUS:
          break;
      }
      break;
  }
}


// Clock::turn_alarm(): Enable or disable alarm
void Clock::turn_alarm(bool on_off)
{
  alarm_activated = on_off;
}

void Clock::show_internal_time(uint32_t internal_time, uint8_t internal_middle_colon, uint8_t show_hour, uint8_t show_minute) {
  uint8_t hour, minute, hour1, hour2, minute1, minute2;
  hour = (internal_time >> 12) & 0b11111;
  hour1 = hour / 10;
  hour2 = hour % 10;
  minute = (internal_time >> 6) & 0b111111;
  minute1 = minute / 10;
  minute2 = minute % 10;
  if(!show_hour || !show_minute) display->clearDisplay();
  if(show_hour) {
    display->display(0, hour1);
    display->display(1, hour2);
  }
  if(show_minute) {
    display->display(2, minute1);
    display->display(3, minute2);
  }  
  display->point(internal_middle_colon);
}

// Clock::show(): Show time or menu on display
void Clock::show()
{
  //show_internal_time(time, middle_colon);
  //show_internal_time(time, middle_colon, !middle_colon, middle_colon);
    switch(system_state) {
    case SHOW_CLOCK:
      show_internal_time(time, middle_colon, 1, 1);
      break;
    case SET_HOUR:
      show_internal_time(time, 1, middle_colon, 1);
      break;
    case SET_MINUTE:
      show_internal_time(time, 1, 1, middle_colon);
      break;
    case SET_HOUR_ALARM:
      show_internal_time(alarm, 1, middle_colon, 1);
      break;
    case SET_MINUTE_ALARM:
      show_internal_time(alarm, 1, 1, middle_colon);
      break;
    case ALARMING:
      show_internal_time(time, 1, middle_colon, middle_colon);
      //if(middle_colon) 
      alarm_tone->play();
      alarming_counter--;
      if(alarming_counter == 0) system_state = SHOW_CLOCK;      
      break;
    case MENU_SET:
      display->displayStr("SET", 0);
      break;
    case MENU_ALARM:
      display->displayStr("AL", 0);
      break;
    case ALARM_OFF:
      display->displayStr("OFF", 0);
      alarm_off_counter--;
      if(alarm_off_counter == 0) system_state = SHOW_CLOCK;
      break;
  }
}


// Clock::check_alarm(): Check if alarm needs to be triggered
void Clock::check_alarm()
{
  if(system_state == SHOW_CLOCK && time == alarm && alarm_activated) {
    system_state = ALARMING;   
    alarming_counter = 60; 
  }
}


void Clock::setup_timer_interrupt() {
  // Code is based on this link:
  // https://docs.espressif.com/projects/arduino-esp32/en/latest/api/timer.html
  // Set timer frequency to 1Mhz
  timer = timerBegin(1000000);
  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer);
  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter) with unlimited count = 0 (fourth parameter).
  timerAlarm(timer, 500000, true, 0);
}


// Clock::run(): Start running the clock
//               This function MUST not block, everything should be handled
//               by interrupts
void Clock::run()
{
  clock0 = this;
  Serial.begin(115200);
  setup_timer_interrupt();
}
