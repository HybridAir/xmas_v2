//uses about 1.5 microamps when sleeping, it should theoretically stay in standby for 3,888 days on a 200 mah cr2032 cell (the battery will self-discharge way sooner though)


#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <util/delay.h>


#define BUTTON				PB5						//used to wake up/go to sleep and switch animations
#define ANIM_DELAY			25						//min ms the button needs to be held to change animations (mainly for debouncing)
#define SLEEP_DELAY			1000					//min ms the button needs to be held to enable sleep mode
#define MAXLEDS 			18						//the max number of LEDs in the entire project
#define MAXANIMS			6						//the max number of programmed animations
#define FRAME_DELAY			125						//base animation refresh rate in ms
#define SLEEPTIME			3600000					//time in ms to wait until auto sleeping (1 hour, 4 minutes slow)
//#define SLEEPTIME			60000					//time in ms to wait until auto sleeping (1 hour, 4 minutes slow)


byte prevBtn = 0;									//last button state
bool btnTimerFlag = false;							//lets the program know if the user is holding the button down
unsigned long btnTimer = 0;							//time the button has been held down
byte currentAnim = 0;								//the currently displayed animation
unsigned long lastFrameTime = 0;					//the last time the animation changed frames
byte currentFrame = 0;								//basic frame counter, used by the animations


void setup() {
	ACSR |= _BV(ACD);                   			//disable the analog comparator
}


void loop() {
	animation();
	checkBtn();
	checkFrame();
	sleepTimer();
}


//checks if it's time to auto sleep
void sleepTimer() {
	if(millis() >= SLEEPTIME) {						//if the current time is greater than the sleeptime value
		sleep();									//go to sleep
	}
}


//gets the current button state, does not work when sleeping
void checkBtn() {
	byte newBtn = (PINB & (1<<BUTTON));								//get the current/new button state (1 or 0)
	
	if(newBtn != prevBtn) {											//if this new state is different from the last one
		prevBtn = newBtn;											//record the new button state as the new previous one
	
		if(newBtn) {												//if the button just went from LOW to HIGH
			btnTimer = millis();									//get the new start time for the button timer
			btnTimerFlag = true;									//start the button timer
		}
		else {														//if the button just went from HIGH to LOW
			//the button has not been held long enough to go to sleep
			//but it may also not have been held long enough to pass the debouncing test and switch the animation
			
			if(btnTimerFlag) {										//make sure the button timer is even running before we check that
				//the above is needed for a dumb edge case when the button is released when waking up	
					
				if(millis() >= (btnTimer + SLEEP_DELAY)) {			//if the button has been held for at least SLEEP_DELAY ms
					btnTimerFlag = false;							//stop the button timer
					sleep();										//go to sleep
				}
				else if(millis() >= (btnTimer + ANIM_DELAY)) {		//if the button has been HIGH for at least ANIM_DELAY ms
					btnTimerFlag = false;							//stop the button timer
					switchAnimation();								//switch the animation
				}
				else {												//the button press failed the debounce test (not held long enough)
					btnTimerFlag = false;							//stop the button timer
				}
			}
		}
	}
}



void sleep() {
	allLedsOff();

    GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
    PCMSK |= _BV(PCINT5);                   // Use PB5 as interrupt pin
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    
    sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    sei();                                  // Enable interrupts
    sleep_cpu();                            // sleep
	
	//wake up here
	resetMillis();
	
    cli();                                  // Disable interrupts
	resetMillis();							//reset the millis value since the device is essentially turning off
    PCMSK &= ~_BV(PCINT5);                  // Turn off PB5 as interrupt pin
    sleep_disable();                        // Clear SE bit
    sei();                                  // Enable interrupts
	
	switchAnimation();
}



ISR(PCINT0_vect) {
}