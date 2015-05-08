//LV Master reader/writer
//Author: Samer Kilani
//19/01/2015

#include <Wire.h>
//#include <avr/pgmspace.h>

#define ver "2.0"

String wireString="";


char inputString[50];  

//const char MEM_adrs_rsp[] PROGMEM  = {"Enter LV board address"};

//const PROGMEM  String menu[6]  = {"on\r","off\r","status\r","sc\r","version\r","help\r"};

String menu[6] = {"on","off","status","sc","version","help"};
//String adrs_rsp = "Enter LV board address";
String reply[2] = {"OK","Error"};
int slave_adrs=0;
bool sc_mode = false;
bool stringComplete = false;  // whether the string is complete
byte x=0;
int y=0;
void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  Serial.print('>');
}


void loop()
{
  
  serialEvent(); //This is only needed for the Arduino micro
    // print the string when a newline arrives:
    
  if (stringComplete) {
  if (y>1) { //to check if the command is not empty
      //Serial.println(inputString);
      //Serial.println( menu[0]);
        if (((String)inputString).startsWith(menu[0])) {
          slave_adrs=((String)inputString).substring(3,((String)inputString).length()).toInt();
          Wire.beginTransmission(slave_adrs); // transmit to device #
          Wire.write(0x02);              // sends one byte
          if (Wire.endTransmission()==0) Serial.println(reply[0]);  // stop transmitting
          else Serial.println(reply[1]);
        }
        else if (((String)inputString).startsWith(menu[1])) {
          slave_adrs=((String)inputString).substring(4,((String)inputString).length()).toInt();
          Wire.beginTransmission(slave_adrs); // transmit to device #
          Wire.write(0x01);              // sends one byte
          if (Wire.endTransmission()==0) Serial.println(reply[0]);  // stop transmitting
          else Serial.println(reply[1]);
        }  
        else if (((String)inputString).startsWith(menu[2])) {
            //Serial.write("\nPolling status\n");
            slave_adrs=((String)inputString).substring(7,((String)inputString).length()).toInt();
            byte datain[5];
            int i=0;
            if (Wire.requestFrom(slave_adrs, 5)<5) Serial.println("Error");    // request 3 byte from slave device #
            else {
              delay(100);
              while (Wire.available())   // slave may send less than requested
              {
                datain[i] = Wire.read(); // receive a byte as character
                //Serial.println(datain[i],HEX);
                //Serial.print("Byte ");
               // Serial.print(i);
                //Serial.print(" ");
                //Serial.println(datain[i]);         // print the character
                i++;
              }
               
              int pinvalue1 = joinbytes(datain[0],datain[1]);
              int pinvalue2 = joinbytes(datain[2],datain[3]);
              if ((datain[4]) == 0xaa) Serial.println("ON");
              else if ((datain[4]) == 0xcc) Serial.println("OFF");
              
              //convert the three bytes to two integers
              //0000 XXXX XXXX XXYY YYYY YYYY
              //int pinvalue1 = (datain[0] << 4) & 0xf0;
              //pinvalue1 = pinvalue1 | (datain[1] >> 4);
              //pinvalue1 = (pinvalue1 << 2) | ((datain[1] >> 2) & 0x03);
              //to find the on/off status
              //the slave board should return 0xc0 or 0x50. If neither were received then the address is wrong.
              //if (((datain[0] & 0xf0) != 0xc0) && ((datain[0] & 0xf0) != 0x50)) Serial.println("Error 52");
              //else
              //{
                //if ((datain[0] & 0xf0) == 0xc0) Serial.println("OFF");
                //else if ((datain[0] & 0xf0) == 0x50) Serial.println("ON");
                Serial.print("-5V -> ");
                Serial.print(((float)pinvalue1/1023));
                Serial.println("A");
                //int pinvalue2 = ((datain[1]<<8) & 0x300) | datain[2];
                Serial.print("+5V -> ");
                Serial.print(((float)pinvalue2/1023));
                Serial.println("A");
              //}
            }
        }
        else if (((String)inputString).startsWith(menu[3])) {
            slave_adrs=((String)inputString).substring(3,((String)inputString).length()).toInt();
            Wire.beginTransmission(slave_adrs); // transmit to device #
            Wire.write(0x03);              // sends one byte
            if (Wire.endTransmission()==0) {
              Serial.print("SC on device ");
              Serial.println(slave_adrs);
              sc_mode=true;    // stop transmitting
            }
            else {
              Serial.println("Error");
              sc_mode=false; 
            }
        }
        else if (((String)inputString).startsWith(menu[4])) {
          Serial.println(ver);
          //Serial.print('>');
        }
        
        else if ((((String)inputString).startsWith(menu[5]))) {
          for (int i = 0;i<6;i++) Serial.println(menu[i]);
          //Serial.print('>');
        }
        else Serial.println("Bad command");
    }
    
    
    
    // clear the string:
    for( int i = 0; i < sizeof(inputString);  ++i )
    inputString[i] = (char)0;
    y=0;
    stringComplete = false;
    if (!sc_mode) Serial.print('>');
  }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    
    
    char inChar = (char)Serial.read();
    if (inChar!=0x03 && sc_mode)
    {
        Wire.beginTransmission(slave_adrs);
        Wire.write(inChar);
        Wire.endTransmission();
        
        delay(10);
        Wire.requestFrom(slave_adrs, 64); 

      while (Wire.available())   // slave may send less than requested
      {
        char c = Wire.read(); // receive a byte as character
        if (c<=0) break;
        Serial.print(c);
      }
    } else if (inChar==0x03 && sc_mode) {
 //break @ CTRL+C
      sc_mode=false;
      Serial.print("SC mode ended\n>");
      for( int i = 0; i < sizeof(inputString);  ++i )
      inputString[i] = (char)0;
      y=0;
    }
    else {
      Serial.write(inChar);
      inputString[y] = inChar;
      y++;
      if (inChar == '\r') stringComplete = true;
    }
  }
}


int joinbytes(byte b1, byte b2)
{
  int joinedint = ((b2<<8) & 0x0f00) | (0xff & b1);
  return joinedint;
}
