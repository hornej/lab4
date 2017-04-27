/*
 * clockDisplay.c
 *
 *  Created on: Sep 24, 2016
 *      Author: hornej2
 */

#include <stdio.h>
#include <stdbool.h>
#include "string.h"
#include "clockDisplay.h"
#include "supportFiles/display.h"
#include "supportFiles/utils.h"

#define CLOCK_TEXT_SIZE 6 //set font size multiplier
#define BASE_TEXT_WIDTH 6 //width of the text in pixels
#define BASE_TEXT_HEIGHT 8 //height of the text in pixels
#define TEXT_SIZE_CHAR_WIDTH CLOCK_TEXT_SIZE * BASE_TEXT_WIDTH //text size width
#define TEXT_SIZE_CHAR_HEIGHT CLOCK_TEXT_SIZE * BASE_TEXT_HEIGHT //text size height
//y coordinate for base of upper triangle
#define TRIANGLE_BASE_TOP_Y DISPLAY_HEIGHT/2 - TEXT_SIZE_CHAR_HEIGHT
//y coordinate for point of upper triangle
#define TRIANGLE_TOP_TOP_Y DISPLAY_HEIGHT/2 - TEXT_SIZE_CHAR_HEIGHT*2
//y coordinate for base of lower triangle
#define TRIANGLE_BASE_BOTTOM_Y DISPLAY_HEIGHT/2 + TEXT_SIZE_CHAR_HEIGHT
//y coordinate for point of lower triangle
#define TRIANGLE_TOP_BOTTOM_Y DISPLAY_HEIGHT/2 + TEXT_SIZE_CHAR_HEIGHT*2
//x coordinate for middle of left triangles
#define TRIANGLE_ONE_X0 DISPLAY_WIDTH/2 - 4 * TEXT_SIZE_CHAR_WIDTH + TEXT_SIZE_CHAR_WIDTH
//x coordinate for left side of left triangles
#define TRIANGLE_ONE_X1 DISPLAY_WIDTH/2 - 5 * TEXT_SIZE_CHAR_WIDTH + TEXT_SIZE_CHAR_WIDTH
//x coordinate for right side of left triangles
#define TRIANGLE_ONE_X2 DISPLAY_WIDTH/2 - 3 * TEXT_SIZE_CHAR_WIDTH + TEXT_SIZE_CHAR_WIDTH
#define TRIANGLE_TWO_X0 DISPLAY_WIDTH/2 //x coordinate for middle of middle triangles
#define TRIANGLE_TWO_X1 DISPLAY_WIDTH/2 - TEXT_SIZE_CHAR_WIDTH //x coordinate for left of middle triangles
#define TRIANGLE_TWO_X2 DISPLAY_WIDTH/2 + TEXT_SIZE_CHAR_WIDTH //x coordinate for right of middle triangles
//x coordinate for middle of right triangles
#define TRIANGLE_THREE_X0 DISPLAY_WIDTH/2 + 4 * TEXT_SIZE_CHAR_WIDTH - TEXT_SIZE_CHAR_WIDTH
//x coordinate for right side of right triangles
#define TRIANGLE_THREE_X1 DISPLAY_WIDTH/2 + 5 * TEXT_SIZE_CHAR_WIDTH - TEXT_SIZE_CHAR_WIDTH
//x coordinate for left side of right triangles
#define TRIANGLE_THREE_X2 DISPLAY_WIDTH/2 + 3 * TEXT_SIZE_CHAR_WIDTH - TEXT_SIZE_CHAR_WIDTH
#define CHAR_HEIGHT DISPLAY_HEIGHT/2 - TEXT_SIZE_CHAR_HEIGHT/2.2 //sets clock text in middle of display
#define DISPLAY_MID DISPLAY_HEIGHT/2 //half the display height
#define DISPLAY_ONE_THIRD DISPLAY_WIDTH/3 //one-third display width
#define DISPLAY_TWO_THIRD DISPLAY_WIDTH/3*2 //two-thirds display width
#define TOP_LEFT 1 //quadrant 1
#define BOTTOM_LEFT 2 //quadrant 2
#define TOP_MID 3 //quadrant 3
#define BOTTOM_MID 4 //quadrant 4
#define TOP_RIGHT 5 //quadrant 5
#define BOTTOM_RIGHT 6 //quadrant 6
#define MINUTES_MAX 59 //maximum number of minutes to display on clock
#define SECONDS_MAX 59 //maximum number of seconds to display on clock
#define ZERO 0 //the number 0
#define HOURS_MIN 1 //minimum number of minutes to display on clock
#define HOURS_MAX 12 //maximum number of minutes to display on clock
#define CHAR_LENGTH 9 //length of the char arrays that hold the time
#define HOURS 12 //initialized hour value
#define MINUTES 59 //initialized minute value
#define SECONDS 59 //initialized seconds value
#define ONE_SECOND 1000 //1000ms
#define ONE_TENTH_SECOND 100 //100ms
#define COUNT_TO_FIVE 5 //this is for running loop to 5
#define COUNT_TO_HUNDRED 100 //this is for running loop to 100

uint32_t hours = HOURS; //initialize hours
uint32_t minutes = MINUTES; //initialize minutes
uint32_t seconds = SECONDS; //initialize seconds

char current[CHAR_LENGTH]; //char array that holds current time
char previous[CHAR_LENGTH]; //char array that holds previous time

// Called only once - performs any necessary inits.
// This is a good place to draw the triangles and any other
// parts of the clock display that will never change.
void clockDisplay_init(){
    display_init();
    display_fillScreen(DISPLAY_BLACK); //fill screen black
    display_setTextColor(DISPLAY_GREEN,DISPLAY_BLACK);//green text black background
    // top left triangle
    display_fillTriangle(TRIANGLE_ONE_X0,TRIANGLE_TOP_TOP_Y,TRIANGLE_ONE_X1,
            TRIANGLE_BASE_TOP_Y,TRIANGLE_ONE_X2,TRIANGLE_BASE_TOP_Y,DISPLAY_GREEN);
    // top middle triangle
    display_fillTriangle(TRIANGLE_TWO_X0,TRIANGLE_TOP_TOP_Y,TRIANGLE_TWO_X1,
            TRIANGLE_BASE_TOP_Y,TRIANGLE_TWO_X2,TRIANGLE_BASE_TOP_Y,DISPLAY_GREEN);
    // top right triangle
    display_fillTriangle(TRIANGLE_THREE_X0,TRIANGLE_TOP_TOP_Y,TRIANGLE_THREE_X1,
            TRIANGLE_BASE_TOP_Y,TRIANGLE_THREE_X2,TRIANGLE_BASE_TOP_Y,DISPLAY_GREEN);
    // bottom left triangle
    display_fillTriangle(TRIANGLE_ONE_X0,TRIANGLE_TOP_BOTTOM_Y,TRIANGLE_ONE_X1,
            TRIANGLE_BASE_BOTTOM_Y,TRIANGLE_ONE_X2,TRIANGLE_BASE_BOTTOM_Y,DISPLAY_GREEN);
    // bottom middle triangle
    display_fillTriangle(TRIANGLE_TWO_X0,TRIANGLE_TOP_BOTTOM_Y,TRIANGLE_TWO_X1,
            TRIANGLE_BASE_BOTTOM_Y,TRIANGLE_TWO_X2,TRIANGLE_BASE_BOTTOM_Y,DISPLAY_GREEN);
    // bottom right triangle
    display_fillTriangle(TRIANGLE_THREE_X0,TRIANGLE_TOP_BOTTOM_Y,TRIANGLE_THREE_X1,
            TRIANGLE_BASE_BOTTOM_Y,TRIANGLE_THREE_X2,TRIANGLE_BASE_BOTTOM_Y,DISPLAY_GREEN);
    // clock text
    display_setCursor(TRIANGLE_ONE_X1,CHAR_HEIGHT); //sets cursor where text should start
    //display_setTextColor(DISPLAY_GREEN); //green text!
    display_setTextSize(CLOCK_TEXT_SIZE); //sets the clock text size
    sprintf(current,"%2hd:%02hd:%02hd",hours,minutes,seconds);
    display_println(current);
}


void incHour(){ //increments hours
    if (hours < 12){ //if hours is less than 12, continue increasing
        hours++;
    }
    else {
        hours = 1; //if hours goes past 12 roll over to 1
    }
    clockDisplay_updateTimeDisplay(false); //update time display
}
void incMin(){ //increments minutes
    if (minutes < MINUTES_MAX){ //if minutes is less than 59, continue increasing
        minutes++;
    }
    else {
        minutes = ZERO; //if minutes goes past 59 roll over to 0
        incHour(); //call increment hours function
    }
    clockDisplay_updateTimeDisplay(false); //update time display
}
void incSec(){ //increments seconds
    if (seconds < SECONDS_MAX){ //if seconds is less than 59, continue increasing
        seconds++;
    }
    else {
        seconds = ZERO; //if seconds goes past 59 roll over to 0
        incMin(); //call increment hours function
    }
    clockDisplay_updateTimeDisplay(false); //update time display
}
void decSec(){ //decrements seconds
    if (seconds > ZERO){ //if seconds is greater than 0, continue decreasing
        seconds--;
    }
    else {
        seconds = SECONDS_MAX; //if seconds goes below 0, roll over to 59
        //decMin();
    }
    clockDisplay_updateTimeDisplay(false); //update time display
}

void decMin(){ //decrements minutes
    if (minutes > ZERO){ //if minutes is greater than 0, continue decreasing
        minutes--;
    }
    else {
        minutes = MINUTES_MAX; //if minutes goes below 0, roll over to 59
        //decHour();
    }
    clockDisplay_updateTimeDisplay(false); //update time display
}

void decHour(){ //decrements hours
    if (hours > HOURS_MIN){ //if hours is greater than 1, continue decreasing
        hours--;
    }
    else {
        hours = HOURS_MAX; //if hours goes below 1, roll over to 59
    }
    clockDisplay_updateTimeDisplay(false); //update time display
}

int16_t getCoordinate(int16_t x, int16_t y){ //passes in x and y coordinates from touch screen
    if (y < DISPLAY_MID){ //is the touch on the top half of the screen?
        if (x < DISPLAY_ONE_THIRD){ //check top left quadrant
            return 1;
        }
        else if (x > DISPLAY_ONE_THIRD && x < DISPLAY_TWO_THIRD){ //check top middle quadrant
            return TOP_MID;
        }
        else if (x > DISPLAY_TWO_THIRD){ //check top right quadrant
            return TOP_RIGHT;
        }
    }
    else if (y > DISPLAY_MID){ //is the touch on the bottom half of the screen?
        if (x < DISPLAY_ONE_THIRD){ //check bottom left quadrant
            return BOTTOM_LEFT;
        }
        else if (x > DISPLAY_ONE_THIRD && x < DISPLAY_TWO_THIRD){ //check bottom middle quadrant
            return BOTTOM_MID;
        }
        else if (x > DISPLAY_TWO_THIRD){ //check bottom right quadrant
            return BOTTOM_RIGHT;
        }
    }
}

void writeTime(){
    display_setCursor(TRIANGLE_ONE_X1,CHAR_HEIGHT); //set cursor at initial digit
    display_println(current); //print time
}

void writeDigit(uint32_t index){
    //draw a black box at index where the new digit will be drawn
    display_fillRect(TEXT_SIZE_CHAR_WIDTH*index+TRIANGLE_ONE_X1,CHAR_HEIGHT,
            TEXT_SIZE_CHAR_WIDTH,TEXT_SIZE_CHAR_HEIGHT,DISPLAY_BLACK);
    display_setCursor(TEXT_SIZE_CHAR_WIDTH*index+TRIANGLE_ONE_X1,CHAR_HEIGHT); //sets cursor at respective digit
    display_setTextColor(DISPLAY_GREEN); //green text!
    display_println(current[index]); //write new digit
}

// Updates the time display with latest time, making sure to update only those digits that
// have changed since the last update.
// if forceUpdateAll is true, update all digits.
void clockDisplay_updateTimeDisplay(bool forceUpdateAll){
    if(forceUpdateAll){ //if forceUpdateAll is true, no need to update time display
        sprintf(current,"%2hd:%02hd:%02hd",hours,minutes,seconds);
        writeTime();
    }
    else{ //when forceUpdateAll is false, update time display
        sprintf(current,"%2hd:%02hd:%02hd",hours,minutes,seconds);
        //run through char array and compare every value. if there is a difference, write the new digit
        for (uint32_t i=ZERO;i<CHAR_LENGTH;i++){
            if (previous[i]!=current[i]){ //compare char array
                writeDigit(i); //call writeDigit
            }
            previous[i]=current[i]; //update the digit in the char array
        }
    }
}

//Reads the touched coordinates and performs the increment or decrement,
//depending upon the touched region.
void clockDisplay_performIncDec(){
    int16_t x, y; //x and y coordinates of touchscreen
    uint8_t z; //pressure of touch screen
    //display_clearOldTouchData(); // Throws away all previous touch data.
    display_getTouchedPoint(&x, &y, &z); // Returns the x-y coordinate point and the pressure (z).
    int16_t region = getCoordinate(x,y); //finds the respective quadrant and saves it to int region
    if (region == TOP_LEFT){ //if top left quadrant is pressed increment hours
        incHour();
    }
    else if (region == BOTTOM_LEFT){ //if bottom left quadrant is pressed decrement hours
        decHour();
    }
    else if (region == TOP_MID){ //if top middle quadrant is pressed increment minutes
        incMin();
    }
    else if (region == BOTTOM_MID){ //if bottom middle quadrant is pressed decrement minutes
        decMin();
    }
    else if (region == TOP_RIGHT){ //if top right quadrant is pressed increment seconds
        incSec();
    }
    else if (region == BOTTOM_RIGHT){ //if bottom right quadrant is pressed decrement seconds
        decSec();
    }
    //clockDisplay_updateTimeDisplay(false);
}

// Advances the time forward by 1 second and update the display.
void clockDisplay_advanceTimeOneSecond(){
    incSec();
}

// Run a test of clock-display functions.
void clockDisplay_runTest(){
    clockDisplay_init(); //initialize clock display
    utils_msDelay(ONE_TENTH_SECOND); //wait for 100ms
    for (uint32_t i=ZERO;i<COUNT_TO_FIVE;i++){ //increment hours 5 times
        incHour();
        utils_msDelay(ONE_SECOND); //wait for one second
    }
    for (uint32_t i=ZERO;i<COUNT_TO_FIVE;i++){ //decrement hours 5 times
        decHour();
        utils_msDelay(ONE_SECOND); //wait for one second
    }
    for (uint32_t i=ZERO;i<COUNT_TO_FIVE;i++){ //increment minutes 5 times
        incMin();
        utils_msDelay(ONE_SECOND); //wait for one second
    }
    for (uint32_t i=ZERO;i<COUNT_TO_FIVE;i++){ //decrement minutes 5 times
        decMin();
        utils_msDelay(ONE_SECOND); //wait for one second
    }
    for (uint32_t i=ZERO;i<COUNT_TO_FIVE;i++){ //increment seconds 5 times
        incSec();
        utils_msDelay(ONE_SECOND); //wait for one second
    }
    for (uint32_t i=ZERO;i<COUNT_TO_FIVE;i++){ //decrement seconds 5 times
        decSec();
        utils_msDelay(ONE_SECOND); //wait for one second
    }
    for (uint32_t i=ZERO;i<COUNT_TO_HUNDRED;i++){ //increment seconds at 10x, 100 times
        incSec();
        utils_msDelay(ONE_TENTH_SECOND); //wait for 100ms
    }
}


