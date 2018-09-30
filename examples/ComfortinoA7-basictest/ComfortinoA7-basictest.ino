// SIMPLE EXAMPLE LIBRARY FOR COMFORTINO A7 GSM+GPS MODULE
// USE SERIAL COMMANDS TO CONTROL:
// USE 115200 COM PORT SPEED FOR SERIAL MONITOR
// DO NOT SEND SERIAL COMMANDS WHEN TURNING ON THE MODEM

// D:0-3 will set debug mode
// D:0 - debug off
// D:1 - show only feedback from basic loop
// D:2 - show feedback from basic loop + gps parse data + some basic functional msgs
// D:3 - show all information with full GPS/GSM parser

// M:0-4 will set modem operation mode
// M:0 - modem off
// M:1 - modem on, do not connect GPRS (good for SMS)
// M:2 - connect to GPRS
// M:3 - connect to server 
// M:4 - post HTTP data to server 

// S:text - set message to transmit via HTTP

// V:0-1  will set verbose mode
// V:0 be quiet
// V:1 beep buzzer if communication with modem is active - good for debugging
 


#include <Comfortinoa7.h>

Comfortinoa7 comf;

void setup() {
comf.initalize(23,2,0,1);     // THIS WILL INITIALIZE THE MODEM, TRANSISTOR PIN, MODEM POWER PIN, LEDPIN, BUZZERPIN
}

void loop() {
comf.cA7basicloop(1000);  // IF DEBUGMODE IS ENABLED THIS WILL PRINT BASIC OPERATIONAL INFORMATION EVERY 1000ms
}

ISR(TIMER1_OVF_vect) {
comf.cA7interruptcall();      // THIS IS NECESSARY FOR PSEUDO-PARRALEL PROCESSING OF MODEM DATA AS WE ARE USING INTERRUPTS
}


