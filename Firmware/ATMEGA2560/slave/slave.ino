//LV Slave reader/writer
//Crate version
//Author: Samer Kilani
//09/02/2016




#include <Wire.h>
#include "commands.h"

//---------------- IMPORTANT ----------------------------//
//Wire.h library needs to be modified
//in Wire.h #define BUFFER_LENGTH 32 >>> #define BUFFER_LENGTH 64
//in twi.h   #define TWI_BUFFER_LENGTH 32 >>>   #define TWI_BUFFER_LENGTH 64
//The above two lines increase the i2c buffer from 32bytes to 64bytes (which matches the serial buffer size)
//in twi.h   #define TWI_FREQ 100000L >>>   #define TWI_FREQ 400000L.
//The above line increases the frequency of i2c from 100kHz to 400kHz.
//---------------- IMPORTANT ----------------------------//

#define packetsize 64

byte x =0;
int y=0;
bool SCA=false;
bool SCB=false;
bool BRDCSTSCA=false;
bool BRDCSTSCB=false;
bool statusA=false;
bool statusB=false;

int BRDCST_size=0;
int BRDCST_pointer=0;
char inputString[packetsize]; //this is equal to the i2c buffer size
char receivedData[packetsize];
//-----------------------------
//This has changed for v2.0 of the RACK LV Board
//const int ENApin = 22; 
//const int ENBpin = 23; 
//const int posA_ADC = A2; 
//const int negA_ADC = A1; 
//const int posB_ADC = A3; 
//const int negB_ADC = A4; 
const int ENApin = 23; 
const int ENBpin = 22; 
const int posA_ADC = A3; 
const int negA_ADC = A1; 
const int posB_ADC = A2; 
const int negB_ADC = A4; 
const int NTC_ADC = A0;
const int ADRS0 = 24;
const int ADRS1 = 25;
const int ADRS2 = 26;
const int ADRS3 = 27;
//------------------------------
int ADRS=0;
void setup()
{
  //calculate adrs
  ADRS=(digitalRead(ADRS0)*8)+(digitalRead(ADRS1)*4)+(digitalRead(ADRS2)*2)+(digitalRead(ADRS3)*1);
  Wire.begin(ADRS);                // join i2c bus with address #2
  
  TWAR = (ADRS << 1) | 1;
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event
  Serial1.begin(115200);
  Serial.begin(115200); 
  
  pinMode(ENApin, OUTPUT); //to control the output
  pinMode(ENBpin, OUTPUT); //to control the output
  digitalWrite(ENApin, HIGH);
  digitalWrite(ENBpin, HIGH);
  
  for( int i = 0; i < packetsize;  ++i ){
  inputString[i] = (char)0;
  receivedData[i] = (char)0;}
  //Serial.begin(9600);  // start serial for output
}

void loop()
{
  //delay(100);
}

void serialEvent(){
 // Serial.println("requested");
    if (SCA) {
      while (Serial.available()) { 
        char inChar = Serial.read();
        inputString[y] = inChar;
        y++;
        if (y==64){
    	  Wire.beginTransmission(1); //talk to the master as address 0x1.
          Wire.write(inputString);
    	  Wire.endTransmission();
		  for( int i = 0; i < packetsize;  ++i )
		  inputString[i] = (char)0;  
		  y=0;        
        }
      }
      Wire.beginTransmission(1); //talk to the master as address 0x1.
      Wire.write(inputString);
      Wire.endTransmission();
      y=0;
      for( int i = 0; i < packetsize;  ++i )
      inputString[i] = (char)0;  
    }
}


void serialEvent1(){
  //Serial1.println("requested1");
	if (SCB) {
		
      while (Serial1.available()) { 
        char inChar = Serial1.read();
        inputString[y] = inChar;
		
        y++;
        if (y==64){
			
  	      Wire.beginTransmission(1); //talk to the master as address 0x1.
          Wire.write(inputString);
  	      Wire.endTransmission();
		  for( int i = 0; i < packetsize;  ++i )
		  inputString[i] = (char)0;  
		  y=0;    
        }
      }
	  //Serial1.println("requested2");
      Wire.beginTransmission(1); //talk to the master as address 0x1.
      Wire.write(inputString);
      Wire.endTransmission();
      y=0;
      for( int i = 0; i < packetsize;  ++i )
      inputString[i] = (char)0;  
    }
    
  

}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
	
	int data_pointer=0;

	bool SCterminate=false;
	Serial1.println(howMany);
  while (Wire.available()) // loop through all but the last
  {
Serial1.print("read");
 x = Wire.read();
Serial1.print("wire:");
Serial1.println((char)x);
      if (SCA) {
            //char c = Wire.read(); // receive byte as a character
			if (x>0) {
            if (x==0x03) SCterminate=true; 
			receivedData[data_pointer]=(char)x;
			data_pointer++;     
			}
				//Serial.println(data_pointer);
      } else if (SCB) {
            
            //char c = Wire.read(); // receive byte as a character
            if (x==0x03) SCterminate=true; 
			receivedData[data_pointer]=(char)x;
			data_pointer++;   
      } else if (BRDCSTSCA) {
	
            //char c = Wire.read(); // receive byte as a character 
			
			receivedData[data_pointer]=(char)x;
			
			BRDCST_pointer++;
			data_pointer++;  
			if (BRDCST_pointer==BRDCST_size) SCterminate=true;
			//Serial.println(BRDCST_pointer);
			
			
			
      } else if (BRDCSTSCB) {
		
            //char c = Wire.read(); // receive byte as a character
			receivedData[data_pointer]=(char)x;
			
			BRDCST_pointer++;
			data_pointer++;  
			if (BRDCST_pointer==BRDCST_size) SCterminate=true;
					
      }else {
		for( int i = 0; i < packetsize;  ++i ) receivedData[i] = (char)0;
        //x = Wire.read();    // receive byte as an integer
		//Serial.println(x);
        if (x==CMD_OFFA) digitalWrite(ENApin, HIGH);
        else if (x==CMD_ONA) digitalWrite(ENApin, LOW);
        else if (x==CMD_OFFB) digitalWrite(ENBpin, HIGH);
        else if (x==CMD_ONB) digitalWrite(ENBpin, LOW);
        else if (x==CMD_SCA)  SCA=true;
        else if (x==CMD_SCB)  SCB=true;
		else if (x==CMD_BRDSCA) {
			byte b2=Wire.read();
			//Serial.println(b2);
			byte b1=Wire.read();
		
			//Serial.println(b1);
			BRDCST_size= ((b2<<8) & 0xff00) | (0xff & b1); 
			//Serial.println(BRDCST_size);
			BRDCSTSCA=true;
			BRDCST_pointer=0;
			}//the size of the broadcast data is send immediately after the broadcast command
		else if (x==CMD_BRDSCB) {
			byte b2=Wire.read();
			byte b1=Wire.read();
			
			BRDCST_size= ((b2<<8) & 0xff00) | (0xff & b1); 
			BRDCSTSCB=true;
			BRDCST_pointer=0;
			}//the size of the broadcast data is send immediately after the broadcast command
        //else if (x==CMD_statusA) statusA=true;
        //else if (x==CMD_statusB) statusB=true;
      }   
  }
  if (data_pointer>0){
	if (SCA || BRDCSTSCA) {
		Serial.print(receivedData);
		Serial1.print("sent:");
		Serial1.println(receivedData);
		for( int i = 0; i < packetsize;  ++i ) receivedData[i] = (char)0; 
		if (SCterminate) {SCA=false; BRDCSTSCA=false; SCterminate=false;}
	} else if (SCB || BRDCSTSCB) {
		Serial1.print(receivedData);
		for( int i = 0; i < packetsize;  ++i ) receivedData[i] = (char)0;
		if (SCterminate) {SCB=false; BRDCSTSCB=false; SCterminate=false;}
	} 
	
	data_pointer=0;
  }
}

void requestEvent()
{
  //Serial.println("requested");
  if (!SCA && !SCB){
	 // Serial.println("hello");
    byte data[12];

    boolean lvstatusA = digitalRead(ENApin);
    boolean lvstatusB = digitalRead(ENBpin);
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
    
    
    if (lvstatusA==true) data[10] = 0xcc;
    else data[10] = 0xaa;
    if (lvstatusB==true) data[11] = 0xcc;
    else data[11] = 0xaa;
    Wire.write(data,12);
    
    
  } else {
	  
	/*
	  
    //Serial.println("requested2");
    if (SCA) {
      while (Serial.available()) { 
      char inChar = Serial.read();
      inputString[y] = inChar;
      y++;
    }
    }
    else if (SCB) {
      while (Serial1.available()) { 
      char inChar = Serial1.read();
      inputString[y] = inChar;
      y++;
    }
    }
    y=0;
    Wire.write(inputString);
    for( int i = 0; i < sizeof(inputString);  ++i )
    inputString[i] = (char)0;
*/
  }
}





