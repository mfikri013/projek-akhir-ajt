int PulseSensorPurplePin = 15;        

int LED21 = 15;   

int Signal;                

int Threshold = 3450;            

void setup() {

pinMode(15,OUTPUT);         
Serial.begin(115200);         
}


void loop() {

Signal = analogRead(PulseSensorPurplePin);  

Serial.println(Signal);                    
if(Signal > Threshold){                          

digitalWrite(15,HIGH);

} else {

digitalWrite(15,LOW);                
}

delay(10);

}
