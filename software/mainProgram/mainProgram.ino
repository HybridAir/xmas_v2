//I want to blink on pb4


void setup() {
  // initialize digital pin 13 as an output.
  DDRB |= (1<<PB5);			//make pb4 an output
}

// the loop function runs over and over again forever
void loop() {
  PORTB |= (1<<PB5);
  delay(1000);              // wait for a second
  PORTB &= ~(1<<PB5);
  delay(1000);              // wait for a second
}
