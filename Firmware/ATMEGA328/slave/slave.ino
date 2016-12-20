//LV Slave reader/writer
//Crate version
//For ATMEGA328P
//Author: Samer Kilani
//29/06/2016




#include <Wire.h>
#include "commands.h"

#define packetsize 64

byte x =0;
int y=0;


bool statusA=false;
bool statusB=false;

char inputString[packetsize]; //this is equal to the i2c buffer size
char receivedData[packetsize];
//-----------------------------
const int ENApin = 3; 
const int ENBpin = 2; 
const int LED1 = 10;
const int LED2 = 9;
const int posA_ADC = A1;
const int negA_ADC = A6;
const int posB_ADC = A0;
const int negB_ADC = A2;
const int NTC_ADC = A3;
const int ADRS0 = 5;
const int ADRS1 = 6;
const int ADRS2 = 7;
const int ADRS3 = 8;
//------------------------------
int ADRS=0;

boolean lvstatusA = false;
boolean lvstatusB = false;
    
    
void setup()
{
  //calculate adrs
  ADRS=(digitalRead(ADRS0)*8)+(digitalRead(ADRS1)*4)+(digitalRead(ADRS2)*2)+(digitalRead(ADRS3)*1);
  Wire.begin(ADRS);                // join i2c bus with address #2
  
  TWAR = (ADRS << 1) | 1;
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event
  //Serial.begin(115200); 
  
  pinMode(ENApin, OUTPUT); //to control the output
  pinMode(ENBpin, OUTPUT); //to control the output

  pinMode(LED1, OUTPUT); //to control the output
  pinMode(LED2, OUTPUT); //to control the output

  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);  

  digitalWrite(ENApin, LOW);
  digitalWrite(ENBpin, LOW);
  

  for( int i = 0; i < packetsize;  ++i ){
  inputString[i] = (char)0;
  receivedData[i] = (char)0;}
  //Serial.begin(9600);  // start serial for output
}

void loop()
{
  //delay(100);
}



// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while (Wire.available()) // loop through all but the last
  {
        x = Wire.read();
	for( int i = 0; i < packetsize;  ++i ) receivedData[i] = (char)0;
        //x = Wire.read();    // receive byte as an integer
		//Serial.println(x);

        if (x==CMD_OFFA) {digitalWrite(ENApin, LOW); lvstatusA = false;}
        else if (x==CMD_ONA) {digitalWrite(ENApin, HIGH); lvstatusA = true;}
        else if (x==CMD_OFFB) {digitalWrite(ENBpin, LOW);lvstatusB = false;}
        else if (x==CMD_ONB) {digitalWrite(ENBpin, HIGH);lvstatusB = true;}
        else if (x==CMD_SCA_ON) {
          digitalWrite(LED1, LOW);
          digitalWrite(LED2, HIGH);
        }
        else if (x==CMD_SCB_ON) {
          digitalWrite(LED1, HIGH);
          digitalWrite(LED2, LOW);
        }
        else if (x==CMD_SCA_OFF) {
          digitalWrite(LED1, HIGH);
        }
        else if (x==CMD_SCB_OFF) {
          digitalWrite(LED2, HIGH);
        }

        
  }
}

void requestEvent()
{
  //Serial.println("requested");
	 // Serial.println("hello");
    byte data[12];

    //boolean lvstatusA = digitalRead(ENApin);
    //boolean lvstatusB = digitalRead(ENBpin);
    unsigned int pinvalue1=0;
    unsigned int pinvalue2=0;
    unsigned int pinvalue3=0;
    unsigned int pinvalue4=0;
    unsigned int pinvalue5=0;
    //AVERAGING THE CURRENT READINGS OVER 100MS
    for (int avg=0;avg<11;avg++)
    {
      pinvalue1 += analogRead(posA_ADC);
      pinvalue2 += analogRead(negA_ADC);
      pinvalue3 += analogRead(posB_ADC);
      pinvalue4 += analogRead(negB_ADC);      
      pinvalue5 += analogRead(NTC_ADC);
      delay(20);
    }
    
    pinvalue1=pinvalue1/10; //current reading +ve
    pinvalue2=pinvalue2/10; //current reading -ve
    pinvalue3=pinvalue3/10; //current reading +ve
    pinvalue4=pinvalue4/10; //current reading -ve
    pinvalue5=pinvalue5/10; //NTC temp reading

    data[0] = pinvalue1&0xff;              // sends one byte
    //Serial.println(pinvalue1&0xff,HEX);
    data[1] = pinvalue1>>8;
    //Serial.println(pinvalue1>>8,HEX);
    
    data[2] = pinvalue2&0xff;              // sends one byte
    //Serial.println(pinvalue2&0xff,HEX); 
    data[3] = pinvalue2>>8;
    //Serial.println(pinvalue2>>8,HEX);
    
    data[4] = pinvalue3&0xff;              // sends one byte
    //Serial.println(pinvalue2&0xff,HEX); 
    data[5] = pinvalue3>>8;
    //Serial.println(pinvalue2>>8,HEX);

    data[6] = pinvalue4&0xff;              // sends one byte
    //Serial.println(pinvalue1&0xff,HEX);
    data[7] = pinvalue4>>8;
    //Serial.println(pinvalue1>>8,HEX);
    
    data[8] = pinvalue5&0xff;              // sends one byte
    //Serial.println(pinvalue2&0xff,HEX); 
    data[9] = pinvalue5>>8;
    //Serial.println(pinvalue2>>8,HEX);
    
    
    if (lvstatusA==false) data[10] = 0xcc;
    else data[10] = 0xaa;
    if (lvstatusB==false) data[11] = 0xcc;
    else data[11] = 0xaa;
    Wire.write(data,12);
    
    

}





