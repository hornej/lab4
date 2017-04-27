/*
 * clockControl.c
 *
 *  Created on: Sep 24, 2016
 *      Author: hornej2
 */

#include <stdio.h>
#include <stdbool.h>
#include "string.h"
#include "clockControl.h"
#include "clockDisplay.h"
#include "supportFiles/display.h"
#include "supportFiles/utils.h"

#define ADC_COUNTER_MAX_VALUE 1 //50ms
#define AUTO_COUNTER_MAX_VALUE 10 //500ms
#define RATE_COUNTER_MAX_VALUE 2 //100ms
#define MAX_TIMER_CNT 17 //this is the time of one second
#define RESET 0 //resets counters

uint32_t adcCounter; //keeps track of adc counts
uint32_t autoCounter; //keeps track of auto counts
uint32_t rateCounter; //keeps track of rate counts
uint32_t timer_cnt; //keeps track of the timer count

// States for the controller state machine.
enum clockControl_st_t {
    init_st,                 // Start here, stay in this state for just one tick.
    never_touched_st,        // Wait here until the first touch - clock is disabled until set.
    waiting_for_touch_st,    // waiting for touch, clock is enabled and running.
    ad_Counter_running_st,     // waiting for the touch-controller ADC to settle.
    auto_Counter_running_st,   // waiting for the auto-update delay to expire
    // (user is holding down button for auto-inc/dec)
    rate_Counter_running_st,   // waiting for the rate-timer to expire to know when to perform the auto inc/dec.
    rate_Counter_expired_st,   // when the rate-timer expires, perform the inc/dec function.
    add_second_to_clock_st   // add a second to the clock time and reset the ms counter.
} currentState = init_st;

static clockControl_st_t previousState;
static bool firstPass = true;

// This is a debug state print routine. It will print the names of the states each
// time tick() is called. It only prints states if they are different than the
// previous state.
void debugStatePrint() {
    // Only print the message if:
    // 1. This the first pass and the value for previousState is unknown.
    // 2. previousState != currentState - this prevents reprinting the same state name over and over.
    if (previousState != currentState || firstPass) {
        firstPass = false;                // previousState will be defined, firstPass is false.
        previousState = currentState;     // keep track of the last state that you were in.
        //printf("msCounter:%d\n\r", msCounter);
        switch(currentState) {            // This prints messages based upon the state that you were in.
        case init_st:
            printf("init_st\n\r"); //print init_st to console
            break;
        case never_touched_st:
            printf("never_touched_st\n\r"); //print never_touched_st to console
            break;
        case waiting_for_touch_st:
            printf("waiting_for_touch_st\n\r"); //print waiting_for_touch to console
            break;
        case ad_Counter_running_st:
            printf("ad_Counter_running_st\n\r"); //print ad_Counter_running_st to console
            break;
        case auto_Counter_running_st:
            printf("auto_Counter_running_st\n\r"); //print auto_Counter_running_st to console
            break;
        case rate_Counter_running_st:
            printf("rate_Counter_running_st\n\r"); //print rate_Counter_running_st to console
            break;
        case rate_Counter_expired_st:
            printf("rate_Counter_expired_st\n\r"); //print rate_Counter_expired_st to console
            break;
        }
    }
}

void clockControl_tick() {
    debugStatePrint();

    // Perform state action first
    switch(currentState) {
    case init_st: //do nothing in init state
        break;
    case never_touched_st: //do nothing in never touched
        break;
    case waiting_for_touch_st: //set all counters to 0
        adcCounter = RESET; //set adcCounter to 0
        autoCounter = RESET; //set autoCounter to 0
        rateCounter = RESET; //set rateCounter to 0
        break;
    case ad_Counter_running_st:
        adcCounter++; //increment adcCounter
        break;
    case auto_Counter_running_st:
        autoCounter++; //increment autoCounter
        break;
    case rate_Counter_running_st:
        rateCounter++; //increment rateCounter
        break;
    case rate_Counter_expired_st:
        rateCounter = RESET; //set rateCounter to 0
        break;
    case add_second_to_clock_st:
        clockDisplay_advanceTimeOneSecond(); //run clock
        break;
    default:
        printf("clockControl_tick state update: hit default\n\r");
        break;
    }

    // Perform state update next
    switch(currentState) {
    case init_st:
        currentState = never_touched_st; //move to never_touched_st
        break;
    case never_touched_st:
        if (display_isTouched()){ //if display is touched move to waiting_for_touch_st
            currentState = waiting_for_touch_st;
        }
        break;
    case waiting_for_touch_st:
        if (display_isTouched()){ //if display is touched clear old data and move to ad_Counter_running_st
            display_clearOldTouchData();
            currentState = ad_Counter_running_st;
        }
        else {
            if(timer_cnt > MAX_TIMER_CNT) { //once timer is greater than 1 second
                timer_cnt = RESET; //reset timer_cnt to 0
                currentState = add_second_to_clock_st; //move to add_second_to_clock_st
            }
            else{ //if timer_cnt is less than 1 second
                timer_cnt++; //increment timer_cnt
            }
        }
        break;
    case ad_Counter_running_st:
        //if display is not touched and the adcCounter is greater than or equal to the max value
        if (!display_isTouched()&&adcCounter>=ADC_COUNTER_MAX_VALUE){
            clockDisplay_performIncDec(); //increment/decrement clock
            currentState = waiting_for_touch_st; //move to waiting_for_touch_st
        }
        //if display is touched and the adcCounter is greater than or equal to the max value
        else if (display_isTouched()&&adcCounter>=ADC_COUNTER_MAX_VALUE){
            currentState = auto_Counter_running_st; //move to auto_Counter_running_st
        }
        else {
            currentState = ad_Counter_running_st; //move to ad_Counter_running_st
        }
        break;
    case auto_Counter_running_st:
        if (!display_isTouched()){ //if display is not touched
            clockDisplay_performIncDec(); //increment/decrement clock
            currentState = waiting_for_touch_st; //move to waiting_for_touch_st
        }
        //if display is touched and autoCounter is greater than or equal to the max value
        else if (display_isTouched()&&autoCounter>=AUTO_COUNTER_MAX_VALUE){
            clockDisplay_performIncDec(); //increment/decrement clock
            currentState = rate_Counter_running_st; //move to rate_Counter_running_st
        }
        else {
            currentState = auto_Counter_running_st; //move to auto_Counter_running_st
        }
        break;
    case rate_Counter_running_st:
        if (!display_isTouched()){ //if display is not touched
            currentState = waiting_for_touch_st; //move to waiting_for_touch_st
        }
        //if display is touched and rateCounter is greater than or equal to the max value
        else if (display_isTouched()&&rateCounter>=RATE_COUNTER_MAX_VALUE){
            currentState = rate_Counter_expired_st; //move to rate_Counter_expired_st
        }
        else {
            currentState = rate_Counter_running_st; //move to rate_Counter_running_st
        }
        break;
    case rate_Counter_expired_st:
        if (!display_isTouched()){ //if display is not touched
            currentState = waiting_for_touch_st; //move to waiting_for_touch_st
        }
        else if (display_isTouched()){ //if display is touched
            clockDisplay_performIncDec(); //increment/decrement clock
            currentState = rate_Counter_running_st; //move to rate_Counter_running_st
        }
        break;
    case add_second_to_clock_st:
        if(display_isTouched()){ //if display is touched
            currentState = ad_Counter_running_st; //move to ad_Counter_running_st
        }
        else {
            currentState = waiting_for_touch_st; //move to waiting_for_touch_st
        }
        break;
    default:
        printf("clockControl_tick state action: hit default\n\r");
        break;
    }
}


