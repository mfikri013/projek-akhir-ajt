#include <SoftwareSerial.h>
#include <stdlib.h>
#define DEBUG true
SoftwareSerial ser(2,13); 
#define SSID "Ddio"     
#define PASS "serangsaya"       
#define IP "api.thingspeak.com"
String msg = "GET /update?key=4722S7JT0GI4CZJ8"; 
//Variables
int error;
int sensorPin = 0;         
volatile int BPM;           
volatile int Signal;              
volatile int IBI = 600;      
volatile boolean Pulse = false;     
volatile boolean QS = false;        
static boolean serialVisual = true;    
volatile int rate[10];                    
volatile unsigned long sampleCounter = 0;          
volatile unsigned long lastBeatTime = 0;          
volatile int P =512;                 
volatile int T = 512;               
volatile int thresh = 525; 
volatile int amp = 100;     
volatile boolean firstBeat = true;       
volatile boolean secondBeat = false;      
void setup()  {
  Serial.begin(500000); 
  ser.begin(500000);
  Serial.println("AT");
  ser.println("AT");
  delay(3000);
  if(ser.find("OK"))  {
    connectWiFi();
  }
  interruptSetup(); 
}
void loop() {
  start:  
  error=0;
  updatebeat();
  
  if (error==1) {
    goto start; 
  }
  
  delay(1000); 
}
void updatebeat() {
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  Serial.println(cmd);
  ser.println(cmd);
  delay(2000);
  if(ser.find("Error")) {
    return;
  }
  cmd = msg ;
  cmd += "&field1=";   
  cmd += BPM;
  cmd += "\r\n";
  Serial.print("AT+CIPSEND=");
  ser.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  ser.println(cmd.length());
  if(ser.find(">")) {
    Serial.print(cmd);
    ser.print(cmd);
  }
  else{
   Serial.println("AT+CIPCLOSE");
   ser.println("AT+CIPCLOSE");
    //Resend...
    error=1;
  }
}
boolean connectWiFi() {
  Serial.println("AT+CWMODE=1");
  ser.println("AT+CWMODE=1");
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial.println(cmd);
  ser.println(cmd);
  delay(5000);
  if(ser.find("OK"))  {
    Serial.println("OK");
    return true;    
  }else {
    return false;
  }
}
void interruptSetup() {     
         
} 
ISR (TIMER2_COMPA_vect){                  
  cli();                                 
  Signal = analogRead(sensorPin);               
  sampleCounter += 2;                         
  int N = sampleCounter - lastBeatTime;       
    
  if(Signal < thresh && N > (IBI/5)*3){      
    if (Signal < T){                         
      T = Signal;                            
    }
  }
  if(Signal > thresh && Signal > P){        
    P = Signal;                             
  }                                         
  
  
  if (N > 250){                                   
    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) ){        
      Pulse = true;                               
                    // turn on pin 13 LED
      IBI = sampleCounter - lastBeatTime;         
      lastBeatTime = sampleCounter;               
      if(secondBeat){                        
        secondBeat = false;                  
        for(int i=0; i<=9; i++){            
          rate[i] = IBI;                      
        }
      }
      if(firstBeat){                         
        firstBeat = false;                  
        secondBeat = true;                   
        sei();                               
        return;                              
      }   
      word runningTotal = 0;                     
      for(int i=0; i<=8; i++){                
        rate[i] = rate[i+1];                  
        runningTotal += rate[i];             
      }
      rate[9] = IBI;                          
      runningTotal += rate[9];                
      runningTotal /= 10;                      
      BPM = 60000/runningTotal;               
      QS = true;                               
      
    }                       
  }
  if (Signal < thresh && Pulse == true){   
                // turn off pin 13 LED
    Pulse = false;                         
    amp = P - T;                           
    thresh = amp/2 + T;                    
    P = thresh;                            
    T = thresh;
  }
  if (N > 2500){                          
    thresh = 512;                         
    P = 512;                               
    T = 512;                               
    lastBeatTime = sampleCounter;                
    firstBeat = true;                     
    secondBeat = false;                    
  }
  sei();     
  
}
