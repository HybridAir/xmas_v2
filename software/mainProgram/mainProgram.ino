//uses about 1.5 microamps when sleeping, it should theoretically stay in standby for 3,888 days on a 200 mah cr2032 cell (the battery will self-discharge way sooner though)

/* ANIMATIONS: 
0: single led sequential
1: single led sequential REVERSED
2: tri led sequential
3: tri led sequential REVERSED



*/

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>


#define BUTTON				PB5						//this button will only work when the reset functionality is disabled
#define ANIM_DELAY			25						//min ms the button needs to be held to change animations (mainly for debouncing)
#define SLEEP_DELAY			1000					//min ms the button needs to be held to enable sleep mode
#define MAXLEDS 			18
#define FRAME_DELAY			125
#define SLEEPTIME			60000


byte prevBtn = 0;
bool btnTimerFlag = false;
unsigned long btnTimer = 0;
byte currentAnim = 5;						//the currently displayed animation
unsigned long lastSleepTime = 0;

byte currentColor = 0;
unsigned long lastTime = 0;
byte currentLed = 0;
bool increasing = true;

unsigned long lastFrameTime = 0;
byte currentFrame = 0;
byte prevFrame = 0;


void setup() {
	ADCSRA &= ~_BV(ADEN);               //just leave the ADC turned off, we don't need it
	ACSR |= _BV(ACD);                   //disable the analog comparator
	//sleep();							//go to sleep immediately until woken up by the button interrupt
}


void loop() {
	checkBtn();				//check the button state
	animation();
	checkFrame();
	sleepTimer();
}


//checks if it has been 
void sleepTimer() {
	if(millis() >= SLEEPTIME) {
		//lastSleepTime = millis();
		sleep();
	}
}





//gets the current button state, this only works while the device is not sleeping
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
	
	DDRB = 0;
	PORTB = 0;
	
	
    //GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
    //PCMSK |= _BV(PCINT3);                   // Use PB3 as interrupt pin
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement
    sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    //sei();                                  // Enable interrupts
    sleep_cpu();                            // sleep
	
    cli();                                  // Disable interrupts
    PCMSK &= ~_BV(PCINT3);                  // Turn off PB3 as interrupt pin
    sleep_disable();                        // Clear SE bit
    sei();                                  // Enable interrupts
}



ISR(PCINT0_vect) {
}