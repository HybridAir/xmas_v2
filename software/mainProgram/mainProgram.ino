//uses about 1.5 microamps when sleeping, it should theoretically stay in standby for 3,888 days on a 200 mah cr2032 cell (the battery will self-discharge way sooner though)

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>


#define BUTTON				PB5						//this button will only work when the reset functionality is disabled
#define ANIM_DELAY			25						//min ms the button needs to be held to change animations (mainly for debouncing)
#define SLEEP_DELAY			1000					//min ms the button needs to be held to enable sleep mode
#define MAXLEDS 			18
#define FRAME_DELAY			125


byte prevBtn = 0;
bool btnTimerFlag = false;
long btnTimer = 0;
byte currentAnim = 3;						//the currently displayed animation

byte currentColor = 0;
long lastTime = 0;
byte currentLed = 0;
bool increasing = true;

long lastFrameTime = 0;
byte currentFrame = 0;


void setup() {
	ADCSRA &= ~_BV(ADEN);               //just leave the ADC turned off, we don't need it
	ACSR |= _BV(ACD);                   //disable the analog comparator
	
	//sleep();							//go to sleep immediately until woken up by the button interrupt
}


void loop() {
	checkBtn();				//check the button state
	animation();
	checkFrame();
	//check the sleep timer
	

}


//displays the currently selected animation on the charlieplexed LEDs
void animation() {
	
	if(currentAnim <= 3) {					//base animation: leds are lit sequentially, individually
		if(currentFrame >= MAXLEDS) {
			currentFrame = 0;
		}
			byte ledOut = currentFrame;
		if(currentAnim % 2 == 0) {
			ledOut = (MAXLEDS - 1) - currentFrame;
		}
		charmander(ledOut);
		
		//if above 2, add offset
		if(currentAnim > 1) {
			byte offset1 = ledOut + 6;
			if(offset1 >= MAXLEDS) {
				offset1 = offset1 - MAXLEDS;
			}
			
			byte offset2 = offset1 + 6;
			if(offset2 >= MAXLEDS) {
				offset2 = offset2 - MAXLEDS;
			}
			charmander(offset1);
			charmander(offset2);
		}
	}
	else if(currentAnim == 4) {						//blinks each individual LED, per color section (3 colors)
		
		if(currentFrame >= MAXLEDS) {
			currentFrame = 0;
			currentColor++;
			
			if(currentColor > 2) {
				currentColor = 0;
			}
		}
		
		if(currentFrame % 3 == 0) {
			byte outled = currentColor + currentFrame;
			if(outled < MAXLEDS) {
				charmander(outled);					//light the led we got, plus the color offset
			}
		}
		
	}	
	else if(currentAnim == 5) {							//blink each full color section at once
	
		if(currentFrame > 2) {
			currentFrame = 0;
		}

		for(byte x = 0;x < MAXLEDS;x++) {		//go through every led, starting from the color offset
			if(x % 3 == 0) {					//only match every third led
				byte outled = x + currentFrame;
				if(outled < MAXLEDS) {
					charmander(outled);					//light the led we got, plus the color offset
				}
			}
	
		}

	}
}


void checkFrame() {
	// long time = FRAME_DELAY + lastFrameTime;
	// if(currentAnim == 3) {
		// time = (FRAME_DELAY * 2) + lastFrameTime;
	// }
	
	if(millis() > FRAME_DELAY + lastFrameTime) {
		lastFrameTime = millis();
		currentFrame++;
	}
}


void switchAnimation() {
	currentFrame = 0;
	
	currentAnim++;
	if(currentAnim > 4) {
		currentAnim = 0;
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
	// PORTB &= ~(1<<PB0);
	// PORTB &= ~(1<<PB1);
	// PORTB &= ~(1<<PB2);
	
	
    GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
    PCMSK |= _BV(PCINT3);                   // Use PB3 as interrupt pin
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement
    sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    sei();                                  // Enable interrupts
    sleep_cpu();                            // sleep
	
    cli();                                  // Disable interrupts
    PCMSK &= ~_BV(PCINT3);                  // Turn off PB3 as interrupt pin
    sleep_disable();                        // Clear SE bit
    sei();                                  // Enable interrupts
}



ISR(PCINT0_vect) {
}