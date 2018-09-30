# ComfortinoA7
 
**A7 GPS + GSM Arduino Libraries.  
Intended for use with Comfortino modules.  
  
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
  
// S:text - send text via SMS to predefined number  
  
// V:0-1  will set verbose mode  
// V:0 be quiet  
// V:1 beep buzzer if communication with modem is active - good for debugging  
