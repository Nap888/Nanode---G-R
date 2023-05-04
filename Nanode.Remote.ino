// Start of Nanode Remote code

// Based on the pollee code from <jc@wippler.nl>
// 2011-11-23 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php
//
#include <JeeLib.h>
// #include <rf12.h>

/*
Status LED
*/
#define BLINK_DELAY 1000 // milliseconds
#define LED_PIN 5  // Status LED

/*
RF12 Communications
*/
#define RF12_GROUPID 212    // all nodes must be a member of the same group
                            // to communicate with each other
#define RF12_NODEID  2         // Each node within a group must have a unique ID

/*
Sensor Specific Code
*/

#define TEMPERATURE_PIN A0

// Definition of Data Structure used to send information from Remote to Gateway

typedef struct {
byte node;
long time;
float temp;
} Payload;

// Create an instance of this data structure
Payload mypayload;

//Arduino Setup code, run once after reset
void setup () {
int i;
float temperature;
float volts;
int analogData;

Serial.begin(57600);
Serial.print("Remote Sensorn");

// Initialize RF12 Radio
mypayload.node = rf12_initialize(RF12_NODEID, RF12_433MHZ, RF12_GROUPID);

// Set status LED pin as output
pinMode(LED_PIN,OUTPUT);

// Print temperature to monitor
/* LM34 CODE */
//temperature = analogRead(TEMPERATURE_PIN);
//temperature = (5.0 * temperature * 100.0) / 1024.0;

/* TMP36 CODE */
analogData = analogRead(TEMPERATURE_PIN);
volts = analogData * (5.0 / 1024.0);
temperature = (volts - 0.5) * 100;  //Celsius
temperature = (temperature * 9.0 / 5.0) + 32.0;
Serial.println(temperature);

// Blink LED 3 times at end of initialize sequence
for(i = 0; i < 6; i ++){
toggleLed();
delay(BLINK_DELAY);
}
}

// Each call to toggleLed, switches the state of Led from on to off or off to on
int ledState = LOW;

void toggleLed() {

if (ledState == LOW)
ledState = HIGH;
else
ledState = LOW;
digitalWrite(LED_PIN,ledState);
}

// main loop
void loop () {
float temperature;
float volts;
int analogData;

// wait to be polled by Gateway
if (rf12_recvDone() && rf12_crc == 0 && rf12_len == 0 && RF12_WANTS_ACK) {

// Fill payload with current time in milliseconds
mypayload.time = millis();

/* LM34 CODE */
//temperature = analogRead(TEMPERATURE_PIN);
//temperature = (5.0 * temperature * 100.0) / 1024.0;

/* TMP36 CODE */
analogData = analogRead(TEMPERATURE_PIN);
volts = analogData * (5.0 / 1024.0);
temperature = (volts - 0.5) * 100;  //Celsius
temperature = (temperature * 9.0 / 5.0) + 32.0;

// Fill payload with temperature info
mypayload.temp = temperature;

Serial.println(temperature);

// start transmission
rf12_sendStart(RF12_ACK_REPLY, &mypayload, sizeof mypayload);

toggleLed();
}
}
// End of Nanode Remote Code