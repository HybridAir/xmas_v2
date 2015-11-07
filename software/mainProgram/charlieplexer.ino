//LED charlieplexing section


/* ANIMATIONS: 
0: single led sequential
1: single led sequential REVERSED
2: tri led sequential
3: tri led sequential REVERSED
4: 6 led scan
5: 6 led scan REVERSED
6: full color flashing	*/


//charlieplexed LED sections
#define SECTION_PB3 	0
#define SECTION_PB4 	1
#define SECTION_PB0 	2
#define SECTION_PB1 	3
#define SECTION_PB2 	4


//pins used for charlieplexing {SECTION_PB3, SECTION_PB4, SECTION_PB0, SECTION_PB1, SECTION_PB2}
const byte charliePin[5] = {PB3, PB4, PB0, PB1, PB2};


//charlieplexed LEDs, from 0 to 17
const byte LED[MAXLEDS][2] PROGMEM = {
{SECTION_PB4, SECTION_PB3},
{SECTION_PB0, SECTION_PB3},
{SECTION_PB1, SECTION_PB3},
{SECTION_PB2, SECTION_PB3},
{SECTION_PB3, SECTION_PB4},
{SECTION_PB0, SECTION_PB4},
{SECTION_PB1, SECTION_PB4},
{SECTION_PB2, SECTION_PB4},
{SECTION_PB3, SECTION_PB0},
{SECTION_PB4, SECTION_PB0},
{SECTION_PB1, SECTION_PB0},
{SECTION_PB2, SECTION_PB0},
{SECTION_PB3, SECTION_PB1},
{SECTION_PB4, SECTION_PB1},
{SECTION_PB0, SECTION_PB1},
{SECTION_PB2, SECTION_PB1},
{SECTION_PB3, SECTION_PB2},
{SECTION_PB4, SECTION_PB2}};


//turns the specified LED on, and then off after 1 ms
void ledOn(byte i) {

	byte led1 = pgm_read_byte(&(LED[i][1]));		//get the LED the user wants to be lit out of PROGMEM
	
	//set port and output states as required by the specific LED
	DDRB = (1<<charliePin[pgm_read_byte(&(LED[i][0]))])|(1<<charliePin[led1]);
	PORTB = (1<<charliePin[led1]);

	_delay_ms(1);									//wait 1 ms to get a consistent brightness
	allLedsOff();									//turn all LEDs back off
}


//turns all LEDs off
void allLedsOff() {
	DDRB = 0;
	PORTB = 0;
}


//displays the currently selected animation on the charlieplexed LEDs
void animation() {
	
	if(currentAnim <= 5) {									//if animations 0-5 are selected
	
		if(currentFrame >= MAXLEDS) {						//used for looping the animation
			currentFrame = 0;
		}
		
		byte ledOut = currentFrame;							//get the LED that needs to be lit from the current animation frame
		
		if(currentAnim % 2 == 0) {							//if it's an even numbered animation, reverse it
			ledOut = MAXLEDS - 1 - currentFrame;
		}
		
		if(currentAnim > 1) {								//if animations 2-5 (4 total) are selected
		
			byte offset = 6;								//default offset of 6 LEDs apart
			byte amount = 3;								//default of 3 LEDs on at once
		
			if(currentAnim > 3) {							//if animations 4 or 5 are selected
				//display 6 LEDs directly next to eachother
				offset = 1;
				amount = 6;
			}
		
		
			for(byte i = 0;i<amount;i++) {					//for each LED that needs to be lit
				byte finalOffset = offset*i;				//calculate the sequential offset
				finalOffset = ledOut + finalOffset;			//calculate the LED number from the above
				if(finalOffset >= MAXLEDS) {				//check for overflows
					finalOffset = finalOffset - MAXLEDS;	//loop the LED to the beginning if needed
				}
				ledOn(finalOffset);							//light the LED
			}
		}
		else {												//else, just light the current LED
			ledOn(ledOut);
		}
	}
	else {													//blink each full color section at once
	
		if(currentFrame > 2) {								//only 3 color sections to loop through
			currentFrame = 0;
		}

		for(byte x = 0;x < MAXLEDS;x++) {					//go through every LED
			if(x % 3 == 0) {								//only match every third led, since there are 3 color sections
				byte outled = x + currentFrame;				//calculate which color section needs to be lit to get the LED number
				if(outled < MAXLEDS) {						//don't overflow
					ledOn(outled);							//light the offset color sectioned LED we got
				}
			}
		}
	}
}


//checks if it's time to go to the next animation frame
void checkFrame() {
	if(millis() > FRAME_DELAY + lastFrameTime) {			//if it has been long enough since the last frame change
		lastFrameTime = millis();							//update this to the current time
		currentFrame++;										//increment the fram value
	}
}


//switches the animation and loops to the first when needed
void switchAnimation() {
	//reset animation timing variable
	currentFrame = 0;
	lastFrameTime = 0;
	
	currentAnim++;											//increment the current animation value
	if(currentAnim > MAXANIMS) {							//loop to the beginning when needed
		currentAnim = 0;
	}
}