/*
  Comfortinoa7.h - Library for A7 module for Comfortino 
  Created by JMLABS Sp. z o.o., March 3, 2018.
  Property of JMLABS Sp. z o.o.
*/

#ifndef Comfortinoa7_h
#define Comfortinoa7_h

#include "Arduino.h"

class Comfortinoa7{
  public:
void initalize(int ptranpin, int ppwrpin, int pverbosepin, int pledpin);
int cA7sethost(String parameter);
int cA7setport(String parameter);
int cA7setsubfolder(String parameter);
int cA7setuseragent(String parameter);
int cA7setap(String parameter);
int cA7setsmsno(String parameter);
int cA7setmode(int parameter);
int cA7getmode();
int cA7sendSMS(String parameter);
int cA7logdata(String parameter);
int cA7checklaststatus();
String cA7rreturnlastresponse();
int cA7setdebug(int parameter);
int cA7setverbose(int parameter);
int cA7setcmdfwrd(int parameter);
void cA7interruptcall();
void cA7basicloop(int pdelay);
void cA7updategps();
void cA7gpson();
int cA7getgpsalt();
int cA7getgpssatno();
char cA7getgpslonEW();
float cA7getgpslon();
char cA7getgpslatNS();
int cA7getgpstime();
float cA7getgpslat();
String cA7getmcc();
String cA7getmcn();
String cA7getcid();
String cA7getlac();
int cA7getsignal();

  private:
void buildclass();
void modemON();
void modemOFF();
void initTimers();
void verbose();
void checkA7mode();
void printdebug();
void sendsms(String no, String text);
void postHTTP(String content);
void A7updateparameters();
void processA7line(String linebuffer);
void processCMD(String cmdbuffer);
void getgps();
void gpson();
int parseNMEA(String input);
};

#endif
