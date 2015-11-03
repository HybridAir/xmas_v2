//uses about 1.5 microamps when sleeping, it should theoretically stay in standby for 3,888 days on a 200 mah cr2032 cell (the battery will self-discharge way sooner though)

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define BUTTON				PB3
#define ANIM_DELAY			25						//min time the button needs to be held to change animations (mainly for debouncing)
#define SLEEP_DELAY			1000					//min time the button needs to be held to enable sleep mode
#define MAXLEDS 			18

byte currentLed = 0;
//byte LED = PB0;
byte prevBtn = 0;
bool btnTimerFlag = false;
long btnTimer = 0;


void setup() {
	// ADCSRA &= ~_BV(ADEN);               //just leave the ADC turned off, we don't need it
	// ACSR |= _BV(ACD);                   //disable the analog comparator
	// DDRB &= ~(1<<BUTTON);					//make pb3 an input
	
	// sleep();							//go to sleep immediately until woken up by the button interrupt
	

}


void loop() {
	//animation();			//run the led animation
	//checkBtn();				//check the button state
	//check the sleep timer
	
	for(byte i=0;i < MAXLEDS;i++) {
		charmander(i);
		delay(100);
	}
}



void animation() {
	switch(currentLed) {				//"animation" placeholder test stuff
		case 0:
			//LED = PB0;
			PORTB |= (1<<PB0);
			PORTB &= ~(1<<PB1);
			PORTB &= ~(1<<PB2);
			break;
		case 1:
			//LED = PB1;
			PORTB |= (1<<PB1);
			PORTB &= ~(1<<PB0);
			PORTB &= ~(1<<PB2);
			break;
		case 2:
			//LED = PB2;
			PORTB |= (1<<PB2);
			PORTB &= ~(1<<PB1);
			PORTB &= ~(1<<PB0);
			break;
	}
	
	// PORTB |= (1<<LED);
	// delay(1000);
	// PORTB &= ~(1<<LED);
}


void switchAnimation() {
	currentLed++;
	if(currentLed > 2) {
		currentLed = 0;
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
	PORTB &= ~(1<<PB0);
	PORTB &= ~(1<<PB1);
	PORTB &= ~(1<<PB2);
	
	
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