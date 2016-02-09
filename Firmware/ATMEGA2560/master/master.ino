//LV Master reader/writer
//Author: Samer Kilani
//Crate version
//09/02/2016

#include <Wire.h>

//---------------- IMPORTANT ----------------------------//
//Wire.h library needs to be modified
//in Wire.h #define BUFFER_LENGTH 32 >>> #define BUFFER_LENGTH 64
//in twi.h   #define TWI_BUFFER_LENGTH 32 >>>   #define TWI_BUFFER_LENGTH 64
//The above two lines increase the i2c buffer from 32bytes to 64bytes (which matches the serial buffer size)
//in twi.h   #define TWI_FREQ 100000L >>>   #define TWI_FREQ 400000L.
//The above line increases the frequency of i2c from 100kHz to 400kHz.
//---------------- IMPORTANT ----------------------------//

#include <math.h>
#include "commands.h"
//#include <avr/pgmspace.h>

#define ver "2.0"
#define packetsize 64

String wireString="";


char inputString[packetsize];  

//const char MEM_adrs_rsp[] PROGMEM  = {"Enter LV board address"};

//const PROGMEM  String menu[6]  = {"on\r","off\r","status\r","sc\r","version\r","help\r"};

String menu[9] = {"ona","onb","offa","offb","status","sca","scb","brdsca","brdscb","version","help"};
//String adrs_rsp = "Enter LV board address";
String reply[2] = {"OK","Error"};
int selected_menu=-1;
int arg=0;
int sc_mode = 0; //0 normal mode, 1 SC mode, 2 Broadcast SC mode
int brdcst_size=0;
int brdcst_count=0;
char i2cpacket[packetsize]; //defined by the serial and wire buffer sizes
bool stringComplete = false;  // whether the string is complete
byte x=0;
int pointer=0;
int scpointer=0;
void setup()
{
  Wire.begin(1); // join i2c bus (address optional for master but it is assigned as 1 to allow slaves to send back data when they receive it through the SC)
  //Wire.setClock(400000L); //setting the i2c to run at 400kHz
  Serial.begin(230400);  // start serial for output
  Serial.print('>');
}


void loop()
{
  
  //serialEvent(); //This is only needed for the Arduino micro
    // print the string when a newline arrives:
    
  if (stringComplete) {
  if (pointer>1) { //to check if the command is not empty
      selected_menu=-1;
      for (int i=0;i<10;i++){
        if (((String)inputString).startsWith(menu[i])) selected_menu=i;
      }
      arg=((String)inputString).substring((menu[selected_menu].length())+1,((String)inputString).length()).toInt();
      //Serial.println(inputString);
      //Serial.println( menu[0]);



      switch (selected_menu){
        case 0:
          Wire.beginTransmission(arg); // transmit to device #
          Wire.write(CMD_ONA);              // sends one byte
          if (Wire.endTransmission()==0) Serial.println(reply[0]);  // stop transmitting
          else Serial.println(reply[1]);
        break;
        case 1:
          Wire.beginTransmission(arg); // transmit to device #
          Wire.write(CMD_ONB);              // sends one byte
          if (Wire.endTransmission()==0) Serial.println(reply[0]);  // stop transmitting
          else Serial.println(reply[1]);
        break;
        case 2:
          Wire.beginTransmission(arg); // transmit to device #
          Wire.write(CMD_OFFA);              // sends one byte
          if (Wire.endTransmission()==0) Serial.println(reply[0]);  // stop transmitting
          else Serial.println(reply[1]);
        break;
        case 3:
          Wire.beginTransmission(arg); // transmit to device #
          Wire.write(CMD_OFFB);              // sends one byte
          if (Wire.endTransmission()==0) Serial.println(reply[0]);  // stop transmitting
          else Serial.println(reply[1]);
        break;
        case 4:
        {
            byte datain[12];
            int j=0;
            
            if (Wire.requestFrom(arg, 12)<12) Serial.println("Error");    // request 3 byte from slave device #
            else {
              delay(100);
              while (Wire.available())   // slave may send less than requested
              {
                datain[j] = Wire.read(); // receive a byte as character
                //Serial.println(datain[i],HEX);
                //Serial.print("Byte ");
               // Serial.print(i);
                //Serial.print(" ");
                //Serial.println(datain[i]);         // print the character
                j++;
              }
               
              int pinvalue1 = joinbytes(datain[0],datain[1]);
              int pinvalue2 = joinbytes(datain[2],datain[3]);
              int pinvalue3 = joinbytes(datain[4],datain[5]);
              int pinvalue4 = joinbytes(datain[6],datain[7]);
              int pinvalue5 = joinbytes(datain[8],datain[9]);
             
              double temp=0;



                Serial.println("LV A");
                if ((datain[10]) == 0xaa) Serial.println("ON");
                else if ((datain[10]) == 0xcc) Serial.println("OFF");
                Serial.print("+5V -> ");
                Serial.print(5.0*((float)pinvalue1/1023));
                Serial.println("A");
                //int pinvalue2 = ((datain[1]<<8) & 0x300) | datain[2];
                Serial.print("-5V -> ");
                Serial.print(5.0*((float)pinvalue2/1023));
                Serial.println("A");
                
                Serial.println("LV B");
                if ((datain[11]) == 0xaa) Serial.println("ON");
                else if ((datain[11]) == 0xcc) Serial.println("OFF");
                Serial.print("+5V -> ");
                Serial.print(5.0*((float)pinvalue3/1023));
                Serial.println("A");
                //int pinvalue2 = ((datain[1]<<8) & 0x300) | datain[2];
                Serial.print("-5V -> ");
                Serial.print(5.0*((float)pinvalue4/1023));
                Serial.println("A");
                
                Serial.print("Temp -> ");
                temp=(double)pinvalue5/1023.0;
                temp=(temp*9000.0)/(1.0-temp);
                temp=(3455/log(temp/0.102756))-273.15;
                //Serial.print(pinvalue3);
                Serial.print(temp);
                Serial.println("C");
              }
        
        
        }
        break;
        case 5:
          Wire.beginTransmission(arg); // transmit to device #
          Wire.write(CMD_SCA);              // sends one byte
          if (Wire.endTransmission()==0) {
            Serial.print("SC on device ");
            Serial.print(arg);
            Serial.println(" A");
            sc_mode=1;    // stop transmitting
			Wire.onReceive(WireEvent); //Attach the wire to an onreceive function
          }
          else {
            Serial.println("Error");
            sc_mode=0; 
          }
        break;
        case 6:
          Wire.beginTransmission(arg); // transmit to device #
          Wire.write(CMD_SCB);              // sends one byte
          if (Wire.endTransmission()==0) {
            Serial.print("SC on device ");
            Serial.print(arg);
            Serial.println(" B");
            sc_mode=1;    // stop transmitting
			Wire.onReceive(WireEvent); //Attach the wire to an onreceive function
          }
          else {
            Serial.println("Error");
            sc_mode=0; 
          }
        break;
		case 7:
          Wire.beginTransmission(0); // transmit to device #
          Wire.write(CMD_BRDSCA);              // sends one byte
		  brdcst_size=arg;
          if (Wire.endTransmission()==0) {
            Serial.print("Broadcast SC mode on channel A");
            sc_mode=2;   
          }
          else {
            Serial.println("Error");
            sc_mode=0; 
          }
        break;
		case 8:
          Wire.beginTransmission(0); // transmit to device #
          Wire.write(CMD_BRDSCB);              // sends one byte
		  brdcst_size=arg;
          if (Wire.endTransmission()==0) {
            Serial.print("Broadcast SC mode on channel B");
            sc_mode=2;  
          }
          else {
            Serial.println("Error");
            sc_mode=0; 
          }
        break;
        case 9:
        Serial.println(ver);
        break;
        case 10:
        for (int i = 0;i<9;i++) Serial.println(menu[i]);
        break;
        default:
        Serial.println("Bad command");   
      }

    }
    
    
    
    // clear the string:
    for( int i = 0; i < packetsize;  ++i )
    inputString[i] = (char)0;
    pointer=0;
    stringComplete = false;
    if (sc_mode==0) Serial.print('>');
  }
}

void WireEvent(int numBytes) {
  
	if (sc_mode==1){	
	//Serial.println("testtt");
  	  while (Wire.available()) {  // slave may send less than requested
            char c = Wire.read(); // receive a byte as character
            if (c<=0) break;
            Serial.print(c);
          }
	}
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    
    
    char inChar = (char)Serial.read();
	
	if (sc_mode==1)
	{
		if (inChar!=0x03){//improve this to send packets **************
			if (scpointer<packetsize)
			{
				i2cpacket[scpointer]=inChar;
				scpointer++;
			}else{
				Wire.beginTransmission(arg);
				Wire.write(i2cpacket);
				Wire.endTransmission();	
				scpointer=0;
				for( int i = 0; i < packetsize;  ++i )
				i2cpacket[i] = (char)0;  
			}

		}
		else{
			//break @ CTRL+C
			sc_mode=0;
			Serial.print("SC mode ended\n>");
			Wire.onReceive(0); //detach onreceive function
			for( int i = 0; i < packetsize;  ++i )
			inputString[i] = (char)0;
			pointer=0;
			
		}
	}else if (sc_mode==2){
		//improve this to send packets ************** This will not completely work
		//if the data is less than 64 nothing will be sent!!!
			if (scpointer<packetsize)
			{
				i2cpacket[scpointer]=inChar;
				scpointer++;
				brdcst_count++;
			}else{
				Wire.beginTransmission(0);
				Wire.write(i2cpacket);
				Wire.endTransmission();	
				scpointer=0;
				for( int i = 0; i < packetsize;  ++i )
				i2cpacket[i] = (char)0;  
			}
		if (brdcst_count==brdcst_size) sc_mode=0;
	} else {
		//Serial.print(y);
		inputString[pointer] = inChar;
		if (inChar==0x08 & pointer>0) { 
		Serial.write(inChar); 
		pointer--; 
		}
		else {
		Serial.write(inChar); 
		pointer++;
		}
		if (inChar == '\r') stringComplete = true;		
	}
  }
}


int joinbytes(byte b1, byte b2)
{
  int joinedint = ((b2<<8) & 0x0f00) | (0xff & b1);
  return joinedint;
}
