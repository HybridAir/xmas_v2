//LED charlieplexing section

//charlieplexed LED sections, used readable as a key for the charliePin array
#define SECTION_PB3 	0
#define SECTION_PB4 	1
#define SECTION_PB0 	2
#define SECTION_PB1 	3
#define SECTION_PB2 	4


// pin definitions {SECTION_PB3, SECTION_PB4, SECTION_PB0, SECTION_PB1, SECTION_PB2}
const byte charliePin[5] = {PB3, PB4, PB0, PB1, PB2};


// Charlieplexed LED definitions (current flowing from-to pairs)
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



void charmander(byte i) {

	byte led1 = pgm_read_byte(&(LED[i][1]));
	
	DDRB = (1<<charliePin[pgm_read_byte(&(LED[i][0]))])|(1<<charliePin[led1]);
	PORTB = (1<<charliePin[led1]);
  
}
