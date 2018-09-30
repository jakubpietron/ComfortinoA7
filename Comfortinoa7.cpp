/*
  Comfortinoa7.cpp - Library for A7 module for Comfortino 
  Created by JMLABS Sp. z o.o., March 3, 2018.
  Property of JMLABS Sp. z o.o.
*/

#include "Arduino.h"
#include "Comfortinoa7.h"

// BASIC SETTINGS FOR COMFORTINO A7 MODULE
int tranpin = 23;
int pwrpin = 2;
int verbosepin = 1;
int ledpin = 0;

// OPERATIONAL VARIABLES
int debugmode = 0;
int cmdfwrd = 1; 
int verbosemode = 0;
int A7setmode = 0;
int A7currentmode = -99;
int gpstime = 0;
float gpslat =0;
char gpslatNS = '0';
float gpslon =0;
char gpslonEW ='0';
int gpssatno = 0;
int gpsalt = 0;
String lastGPGAA = "";
int GPSFWDSTATE = 0;
int GPSSTATE = 0;
String host = "google.com";
String port = "80";
String subfolder = "";
String useragent = "Default";
String ap = "internet";
String smsno = "+55555555555";
String httpheader = "";
String httpmsg = "HELLOTHERE";
int lastresponsecode = -1;
int waslastsent = -1;
String cipstartcnt = "";
String cgdcont = "";
String fullresp = "";
int M6counter = 0;
int ATEset = 0;
int CSQcounter = 99;
int CREGcounter = 0;
int COPScounter = 0;
int CGATTcounter=0;
int CGACTcounter=0;
int CIPSTcounter=0;
int CIPSTRTcounter=0;
int CIPSNDcounter=0;
int INCHTTPcounter=0;
int TIMER = 0;
bool A7incoming = false; 
bool A7incominghttp = false; 
int A7insetmode = 1; 
int A7insetmode_prev = 1;
int flag = 0;
int updatecounter = 0;
int A7setmode_prev=0;
String A7currentCREG = "";
String A7currentnettime = "";
int A7currentsignal = 0;
String A7currentcell = "";
String A7currentarea = "";
String A7currentnetwork = "";

void Comfortinoa7::initalize(int ptranpin, int ppwrpin, int pverbosepin, int pledpin)
{
tranpin = ptranpin;
pwrpin = ppwrpin;
verbosepin = pverbosepin;
ledpin = pledpin;
pinMode(tranpin,OUTPUT);
pinMode(pwrpin,OUTPUT);
pinMode(verbosepin,OUTPUT);
pinMode(ledpin,OUTPUT);
Serial.begin(115200);
Serial1.begin(115200);
Serial.flush();
Serial1.flush();
delay(100);
initTimers(); 
buildclass();
}

void Comfortinoa7::cA7basicloop(int pdelay)
{
checkA7mode();
if (debugmode>=1) printdebug();
delay(pdelay);
}

void Comfortinoa7::buildclass()
{
httpheader = "POST ";
httpheader += subfolder;
httpheader += " HTTP/1.1\r\nHost:  ";
httpheader += host;
httpheader += "\r\nUser-Agent: ";
httpheader += useragent;
httpheader += "\r\nConnection: Close\r\nContent-Type: application/json;\r\nContent-Length: ";
cipstartcnt = "\"TCP\",\"";
cipstartcnt += host;
cipstartcnt += "\",";
cipstartcnt += port;
cgdcont = "1,\"IP\",\"";
cgdcont += ap;
cgdcont += "\"";
}

void Comfortinoa7::modemON()
{
cmdfwrd=0;
  if (debugmode>=2) Serial.print(F("A7 MODEM:\tINITIALIZE"));
  delay(100);
  digitalWrite(tranpin,LOW);
  if (debugmode>=2) Serial.print(F("."));
  delay(500);
  digitalWrite(pwrpin,HIGH);
  if (debugmode>=2) Serial.print(F("."));
  delay(2000);
  digitalWrite(pwrpin,LOW);
  if (debugmode>=2) Serial.print(F("."));
  delay(1000);
  Serial1.flush();
  Serial1.end();
  if (debugmode>=2) Serial.print(F("."));
  delay(1000);
  Serial1.begin(115200);
  if (debugmode>=2) Serial.print(F("."));
  delay(100);
  A7currentmode=-1;
  if (debugmode>=2) Serial.println(F("OK"));
cmdfwrd=1;
}

  void Comfortinoa7::modemOFF()
  {
  if (debugmode>=2) Serial.println(F("A7 MODEM:\tSHUTDOWN"));
  digitalWrite(tranpin,HIGH);
  A7currentmode=-99;
  A7insetmode_prev=0;
  A7currentsignal = 0;
  A7currentcell = "";
  A7currentarea = "";
  A7currentnetwork = "";
  Serial1.flush();
  Serial1.end();
  }

void Comfortinoa7::initTimers() {       // TIMERS INITIALIZATION
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  int timer1_value = 59000;
  TCNT1 = timer1_value;
  TCCR1B |= (1 << CS12);
  TIMSK1 |= (1 << TOIE1);
  interrupts();
}

void Comfortinoa7::verbose(){             // BEEPS AND LIGHTS ON SERIAL MSG
	  digitalWrite(ledpin,HIGH);
      if (verbosemode>=1) tone(verbosepin, 4000);
      delay(1);
      if (verbosemode>=1) noTone(verbosepin);
      digitalWrite(ledpin, LOW);
  }

void Comfortinoa7::checkA7mode()
{
A7insetmode = 0;
if ((A7currentmode>=3) and (A7setmode==1)) A7currentmode=1;
if ((A7currentmode>=5) and (A7setmode==2)) A7currentmode=2;
if ((A7currentmode>=6) and (A7setmode==4)) A7insetmode = 1;  // for 3
if ((A7currentmode>=5) and (A7setmode==4) and (httpmsg=="")) A7insetmode = 1;  // for 3
if ((A7currentmode>=5) and (A7setmode==3)) A7insetmode = 1;  // for 3
if ((A7currentmode>=4) and (A7setmode==2)) A7insetmode = 1;  //for 2
if ((A7currentmode>=1) and (A7setmode==1)) A7insetmode = 1; //for 1
if ((A7currentmode==-99) and (A7setmode==0)) A7insetmode = 1; //for 0
if ((debugmode>=0) and (A7insetmode_prev!=A7insetmode) and (A7insetmode == 1)) Serial.println("::::::MODEM READY");
A7insetmode_prev = A7insetmode;
}

void Comfortinoa7::printdebug()
{
Serial.print("TIME:"); 
Serial.print(millis()/1000); 
Serial.print("\t\tMODEM MODE:"); 
Serial.print(A7currentmode); 
Serial.print("("); 
Serial.print(A7setmode); 
Serial.print(") "); 
if (A7insetmode==1) Serial.print("OK");
Serial.print("\t\tNETWORK:"); 
Serial.print(A7currentnetwork); 
Serial.print("\tCELL:"); 
Serial.print(A7currentcell); 
Serial.print("\tAREA:"); 
Serial.print(A7currentarea); 
Serial.print("\tSIGNAL:"); 
Serial.print(A7currentsignal); 
Serial.print("\tLASTRSP:"); 
Serial.println(lastresponsecode); 
}

void Comfortinoa7::A7updateparameters(){        // ENSURE THAT A7CURRENTMODE IS A7TARGETMODE

  if ((A7currentmode==-99) and (A7setmode!=0)) modemON();
  if ((A7currentmode!=-99) and (A7setmode==0)) modemOFF();

if (A7incoming==false){

// SMS 
  if ((A7currentmode != 6) and (A7setmode>=1)) {
    if ((A7currentmode>0) and (A7currentmode<4)) { if (CSQcounter>120) {Serial1.println("AT+CSQ"); CSQcounter=0;} else CSQcounter++;}
    
    if (A7currentmode==0 ) { if (CREGcounter>10) {Serial1.println("AT+CREG?"); CREGcounter=0;} else CREGcounter++; }
    if (A7currentmode==1 or A7currentmode==2) { if (CREGcounter>80) {Serial1.println("AT+CREG?"); CREGcounter=0;} else CREGcounter++; }
    if (A7currentmode==1 or A7currentmode==2) { if (COPScounter>100) {Serial1.println("AT+COPS?"); COPScounter=0;} else COPScounter++;  }
  }

//GPRS ON
  if ((A7currentmode != 6) and (A7setmode>=2)) {
    if (A7currentmode==1 or A7currentmode==2 or A7currentmode==3) { if (CGATTcounter>27) {Serial1.println("AT+CGATT?"); CGATTcounter=0;} else CGATTcounter++;  }
    if ((A7currentmode==3) or (A7currentmode==4))  { if (CGACTcounter>47) {Serial1.println("AT+CGACT?"); CGACTcounter=0;} else CGACTcounter++;  }
   }

//HTTP CONNECT
if ((A7currentmode != 6) and (A7setmode>=3)) {

   if (A7currentmode==4) { if (CIPSTcounter>100) {Serial1.println("AT+CIPSTATUS"); CIPSTcounter=0;} else CIPSTcounter++;  }
   if (A7currentmode==4) { if (CIPSTRTcounter>30) {Serial1.print("AT+CIPSTART="); Serial1.println(cipstartcnt); CIPSTRTcounter=0;} else CIPSTRTcounter++;  }
   if (A7currentmode==5) { if (CIPSTcounter>40) {Serial1.println("AT+CIPSTATUS"); CIPSTcounter=0;} else CIPSTcounter++;  }

if ((A7currentmode !=6) and (A7setmode>=4))
{
   if ((A7currentmode==5) and (httpmsg!="")) { if (CIPSNDcounter>10) {Serial1.println("AT+CIPSEND"); CIPSNDcounter=0; } else CIPSNDcounter++;  }
}
}
}
}

void Comfortinoa7::processA7line(String linebuffer){        // HANDLE RESPONSES FROM A7 MODEM
  
    // UPDATE PARAMS
  if (linebuffer.indexOf("^CINIT:") != -1) { A7currentmode = 0; Serial1.println("AT&F"); delay(500); Serial1.println("AT&F0"); delay(500); }
  if (linebuffer.indexOf("+CSQ:") != -1) {
    if (linebuffer.substring(6,linebuffer.indexOf(",")).toInt() == 0)
    {
    A7currentsignal = 0;
    }
    else 
    {
    A7currentsignal = linebuffer.substring(6,linebuffer.indexOf(",")).toInt();
    }
  }
 if ((linebuffer.indexOf("+CREG: 2,1,") != -1) and (A7currentmode < 3)) {
    A7currentmode = 1;
    A7currentcell = linebuffer.substring(12, 16);
    A7currentarea = linebuffer.substring(19, 23);
    COPScounter=999;
  }
  if ((linebuffer.indexOf("+CREG: 2,5,") != -1) and (A7currentmode < 3)) {
    A7currentmode = 2;
    A7currentcell = linebuffer.substring(12, 16);
    A7currentarea = linebuffer.substring(19, 23);
    COPScounter=999;
  }
  if ((linebuffer.indexOf("+CGATT:1") != -1) and (A7currentmode < 4)) {A7currentmode = 3; CGACTcounter=999;}
  if (linebuffer.indexOf("+COPS: 1,2,") != -1) {A7currentnetwork = linebuffer.substring(12,17); CSQcounter=999;}
  if (linebuffer.indexOf("+CTZV:") != -1) A7currentnettime = linebuffer;
  if (linebuffer.indexOf("0,CONNECT OK") != -1) {A7currentmode = 5; CIPSNDcounter=999; }
  if ((linebuffer.indexOf("0,IP INITIAL") != -1) or (linebuffer.indexOf("0,IP GPRSACT") != -1)) A7currentmode = 4;
  if ((linebuffer.indexOf("+CGACT: 1, 1") != -1) and (A7currentmode < 5)) {A7currentmode = 4; CIPSTRTcounter=999;}
  if (linebuffer.indexOf("+TCPCLOSED:0") != -1)  { A7currentmode = 4; A7incominghttp = false;}
  if ((linebuffer.indexOf("</html>") != -1) and (A7currentmode == 6))  { A7currentmode = 4; A7incominghttp = false; Serial1.write(0x1A); Serial1.write(0x1A);} 
  if (linebuffer.indexOf("$GPGGA") != -1) { lastGPGAA = linebuffer.substring(linebuffer.indexOf("$GPGGA"), linebuffer.length()); parseNMEA(lastGPGAA); } 
  if ((linebuffer.indexOf("0,CONNECT OK") == -1) and (linebuffer.indexOf("CONNECT OK") != -1)) {CIPSTcounter=999; }
  if (linebuffer.indexOf("+CREG: 1/r") != -1) { A7currentmode = 1; CREGcounter=999; }
  if (linebuffer.indexOf("+CREG: 0") != -1) { A7currentmode = 0; }
  if (linebuffer.indexOf("+CIPRCV") != -1) lastresponsecode = linebuffer.substring(linebuffer.indexOf("+CIPRCV")+21,linebuffer.indexOf("+CIPRCV")+24).toInt();
  if ((linebuffer.indexOf(">") != -1) and (A7setmode>=4))  {A7currentmode = 6;}
  if (linebuffer.indexOf("+CREG: 1,") != -1) Serial1.println("AT+CREG=2");
  if (linebuffer.indexOf("+CGATT:0") != -1) Serial1.println("AT+CGATT=1");
  if (linebuffer.indexOf("+CGACT: 0,0") != -1) {
    Serial1.print("AT+CGDCONT=");Serial1.println(cgdcont);
    delay(100);
    Serial1.println("AT+CGACT=1,1");
  }
  if (debugmode>=3) Serial.println(linebuffer);
}

void Comfortinoa7::processCMD(String cmdbuffer) {               // THIS WILL PROCESS EVENTS FROM SERIAL PORT
  // PROCESS CMDBUFFER & RESTART APP
  
if ((cmdbuffer.indexOf("C:")==-1) and (cmdbuffer.indexOf("S:")==-1) and (cmdbuffer.indexOf("M:")==-1) and (cmdbuffer.indexOf("V:")==-1) and (cmdfwrd==1) and (cmdbuffer.indexOf("GPSON")==-1) and (cmdbuffer.indexOf("GPSFWD")==-1)) Serial1.print(cmdbuffer);

//C HANDLER
  if (cmdbuffer.indexOf("C:")!=-1) { 
String newcalib = cmdbuffer.substring(cmdbuffer.indexOf("C:")+2,cmdbuffer.length());
newcalib.trim();
if (newcalib.toInt()!=-1) 
{
if (debugmode>=0) Serial.println("::::::::::::C:");
}
}

//S HANDLER
  if (cmdbuffer.indexOf("S:")!=-1) { 
String newserial = cmdbuffer.substring(cmdbuffer.indexOf("S:")+2,cmdbuffer.length());
newserial.trim();
{
if (debugmode>=0) Serial.println("::::::::::::sending sms");
sendsms(smsno,newserial);
}
}

//M HANDLER
  if (cmdbuffer.indexOf("M:")!=-1) { 
String newmode = cmdbuffer.substring(cmdbuffer.indexOf("M:")+2,cmdbuffer.length());
newmode.trim();
if (newmode.toInt()!=-1) 
{
if (debugmode>=0) Serial.println("::::::::::::NEW MODE REQUESTED, SETTING");
A7setmode = newmode.toInt();
}
}

//D HANDLER
  if (cmdbuffer.indexOf("D:")!=-1) { 
String dbgmode = cmdbuffer.substring(cmdbuffer.indexOf("D:")+2,cmdbuffer.length());
dbgmode.trim();
if (dbgmode.toInt()!=-1) 
{
if (debugmode>=0) Serial.println("::::::::::::DEBUG MODE CHANGE");
debugmode=dbgmode.toInt();
}
}

//V HANDLER
  if (cmdbuffer.indexOf("V:")!=-1) { 
String vermode = cmdbuffer.substring(cmdbuffer.indexOf("V:")+2,cmdbuffer.length());
vermode.trim();
if (vermode.toInt()!=-1) 
{
if (debugmode>=0) Serial.println("::::::::::::VERBOSE MODE CHANGE");
verbosemode=vermode.toInt();
}
}
//GPSON HANDLER
  if (cmdbuffer.indexOf("GPSON")!=-1) { 
{
if (debugmode>=0) Serial.println("::::::::::::GPSON");
gpson();
}
}

//GPSFWD HANDLER
  if (cmdbuffer.indexOf("GPSFWD")!=-1) { 
{
if (debugmode>=0) Serial.println("::::::::::::GETGPS");
getgps();
}
}
}


void Comfortinoa7::sendsms (String no, String text)
{
    A7incominghttp=true;
Serial1.print("AT+CMGF=1\r");
delay(1000);
Serial1.print("AT+CMGS=\"");
Serial1.print(no);
Serial1.print("\"\r");
delay(500);

Serial1.print(text);
Serial1.print("\r");
delay(500);
Serial1.write(0x1A);
delay(500);
A7incominghttp=false;
}


// NOT WORKING = WHY? 

void Comfortinoa7::postHTTP(String content)
{
A7incominghttp=true;
fullresp="";
Serial1.print(httpheader);
Serial1.print(content.length()); Serial1.write("\r\n\r\n");
Serial1.println(content);
Serial1.write("\r\n");
delay(10);
Serial1.write(0x1A);
delay(10);
httpmsg="";
lastresponsecode=0;
M6counter = TIMER;
A7currentmode = 4;
}

  int Comfortinoa7::cA7sethost(String parameter)
  {
    host = parameter;
    buildclass();
    return 1; 
  }

  int Comfortinoa7::cA7setport(String parameter)
  {
    port = parameter;
    buildclass();
    return 1; 
  }

int Comfortinoa7::cA7setsubfolder(String parameter)
  {
    subfolder = parameter;
    buildclass();
    return 1; 
  }

int Comfortinoa7::cA7setuseragent(String parameter)
  {
    useragent = parameter;
    buildclass();
    return 1; 
  }

int Comfortinoa7::cA7setap(String parameter)
  {
    ap = parameter;
    buildclass();
    return 1; 
  }

  int Comfortinoa7::cA7setsmsno(String parameter)
  {
    smsno = parameter;
    return 1; 
  }

  int Comfortinoa7::cA7setmode(int parameter)
  {
    A7setmode = parameter;
    return A7insetmode;
  }

int Comfortinoa7::cA7getmode()
{
  return A7currentmode;
}

int Comfortinoa7::cA7sendSMS(String parameter)
{
  sendsms (smsno, parameter);
  return 1;
}

int Comfortinoa7::cA7logdata(String parameter)
{
  if (A7currentmode==6) {postHTTP(parameter); return 1;} 
  else return 0;
  }

 int Comfortinoa7::cA7checklaststatus(){
  int replastresponsecode=lastresponsecode;
  lastresponsecode=-1;
  return replastresponsecode;
  
 }

 String Comfortinoa7::cA7getmcc()
 {
 return A7currentnetwork.substring(3,10);
}

 String Comfortinoa7::cA7getmcn()
 {
 return A7currentnetwork.substring(0,3);
}

 String Comfortinoa7::cA7getcid()
 {
 return A7currentcell;
}

 String Comfortinoa7::cA7getlac()
 {
 return A7currentarea;
}

 int Comfortinoa7::cA7getsignal()
 {
 return A7currentsignal;
}

 
 String Comfortinoa7::cA7rreturnlastresponse()
 {
  String repfullresp = fullresp;
  fullresp="";
  return repfullresp;  
 }

int Comfortinoa7::cA7setdebug(int parameter)
{
  debugmode=parameter;
  return 1;
}

int Comfortinoa7::cA7setverbose(int parameter)
{
  verbosemode=parameter;
  return 1;
}

int Comfortinoa7::cA7setcmdfwrd(int parameter)
{
  cmdfwrd=parameter;
  return 1;   
}

void Comfortinoa7::cA7interruptcall() {       
if (TIMER>1000) TIMER=0;
TIMER++;    
   if (flag == 0)
  {
    interrupts();
    flag = 1;
    while ((Serial1.available() or (A7incominghttp == true)) )
    {
      TIMER++; 
if ((A7incominghttp==true) and (TIMER-M6counter>15)) {if (debugmode>=1) A7incominghttp=false; A7currentmode=4; TIMER=0; if (GPSFWDSTATE==1) {delay(50); Serial1.println("AT+GPSRD=0"); delay(50);  GPSFWDSTATE=0;}}
      if (ATEset!=1) {Serial1.println("ATE0"); ATEset=1;}  
      A7incoming=true;
      String linebuffer = "";
verbose();
      linebuffer = Serial1.readStringUntil('\n');
      if (A7incominghttp==true) fullresp+=linebuffer; 
      if ((linebuffer.length() != 1) and (linebuffer!="OK\r")) processA7line(linebuffer);
    }
     A7incoming=false;
    
    while (Serial.available())
    {
      
    String cmdbuffer = "";
    verbose();
      cmdbuffer = Serial.readStringUntil('\r\n');
      if (cmdbuffer.length() != 1) processCMD(cmdbuffer);
      while (Serial.available()) Serial.read();
     }
    A7updateparameters();
    TCNT1 = 60000; //was 60000
    flag = 0;
  }
}


int Comfortinoa7::cA7getgpstime()
{
  return gpstime;
}

float Comfortinoa7::cA7getgpslat(){
  return gpslat;
}

char Comfortinoa7::cA7getgpslatNS(){
  return gpslatNS;
}

float Comfortinoa7::cA7getgpslon(){
  return gpslon;
}

char Comfortinoa7::cA7getgpslonEW(){
  return gpslonEW;
}

int Comfortinoa7::cA7getgpssatno(){
  return gpssatno;
}

int Comfortinoa7::cA7getgpsalt(){
  return gpsalt;
}

void Comfortinoa7::cA7gpson()
{
   gpson();
}

void Comfortinoa7::cA7updategps()
{
  getgps();
}

void Comfortinoa7::gpson()
{
  Serial1.println("AT+GPS=1");
  GPSSTATE=1;
  }

void Comfortinoa7::getgps()
{
Serial1.println("AT+GPSRD=1");
GPSFWDSTATE=1;
A7incominghttp=true;
M6counter = TIMER;
}

int Comfortinoa7::parseNMEA(String input)
{
  if (input.substring(0,input.indexOf(","))=="$GPGGA") input.remove(0,input.indexOf(",")+1); else return 0;
  if (debugmode>=2) Serial.print("TIME :");
  gpstime = input.substring(0,input.indexOf(",")).toInt();
  if (debugmode>=2) Serial.print(gpstime);
  input.remove(0,input.indexOf(",")+1); 
  if (debugmode>=2) Serial.print("\tLAT :");
  gpslat = input.substring(0,input.indexOf(",")).toFloat();
  if (debugmode>=2) Serial.print(gpslat);
  input.remove(0,input.indexOf(",")+1);
  gpslatNS = input[0]; 
  if (debugmode>=2) Serial.print(gpslatNS); 
  input.remove(0,input.indexOf(",")+1);
  if (debugmode>=2) Serial.print("\tLON :");
  gpslon = input.substring(0,input.indexOf(",")).toFloat(); 
  if (debugmode>=2) Serial.print(gpslon); 
  input.remove(0,input.indexOf(",")+1);
  gpslonEW= input[0]; 
  if (debugmode>=2) Serial.print(gpslonEW); 
  input.remove(0,input.indexOf(",")+1);
  if (debugmode>=2) Serial.print("\tFIX QUAL :"); 
  if (debugmode>=2) Serial.print(input.substring(0,input.indexOf(",")));  
  input.remove(0,input.indexOf(",")+1);
  if (debugmode>=2) Serial.print("\tSAT NO:"); 
  gpssatno =input.substring(0,input.indexOf(",")).toInt(); 
  if (debugmode>=2) Serial.print(gpssatno); 
  input.remove(0,input.indexOf(",")+1);
  if (debugmode>=2) Serial.print("\tHOR DIL:");
  if (debugmode>=2) Serial.print(input.substring(0,input.indexOf(",")));
  input.remove(0,input.indexOf(",")+1);
  if (debugmode>=2) Serial.print("\tALT:");
  gpsalt = input.substring(0,input.indexOf(",")).toInt();
  if (debugmode>=2) Serial.print(gpsalt);
  input.remove(0,input.indexOf(",")+1);
  if (debugmode>=2) Serial.print(input.substring(0,input.indexOf(","))); 
  input.remove(0,input.indexOf(",")+1);
  if (debugmode>=2) Serial.print("\tHGEOID:");
  if (debugmode>=2) Serial.print(input.substring(0,input.indexOf(","))); 
  input.remove(0,input.indexOf(",")+1);
  if (debugmode>=2) Serial.print(input.substring(0,input.indexOf(",")));
  input.remove(0,input.indexOf(",")+1);
  if (debugmode>=2) Serial.print("\tLSTUPD:"); 
  if (debugmode>=2) Serial.print(input.substring(0,input.indexOf(","))); 
  input.remove(0,input.indexOf(",")+1);
  if (debugmode>=2) Serial.print("\tREFID:"); 
  if (debugmode>=2) Serial.print(input.substring(0,input.indexOf("*")));
  input.remove(0,input.indexOf("*")+1);
  if (debugmode>=2) Serial.print("\tCHKSM:");
  if (debugmode>=2) Serial.println(input.substring(0,input.length()));  
  input.remove(0,input.length()); 
}

