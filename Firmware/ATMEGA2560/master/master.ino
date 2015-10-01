//LV Master reader/writer
//Author: Samer Kilani
//Crate version
//30/09/2015

#include <Wire.h>
#include <math.h>
//#include <avr/pgmspace.h>

#define ver "1.0"

String wireString="";


char inputString[50];  

//const char MEM_adrs_rsp[] PROGMEM  = {"Enter LV board address"};

//const PROGMEM  String menu[6]  = {"on\r","off\r","status\r","sc\r","version\r","help\r"};

String menu[10] = {"ona","onb","offa","offb","statusa","statusb","sca","scb","version","help"};
//String adrs_rsp = "Enter LV board address";
String reply[2] = {"OK","Error"};
int selected_menu=-1;
int slave_adrs=0;
bool sc_mode = false;;
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
  
  //serialEvent(); //This is only needed for the Arduino micro
    // print the string when a newline arrives:
    
  if (stringComplete) {
  if (y>1) { //to check if the command is not empty
      selected_menu=-1;
      for (int i=0;i<10;i++){
        if (((String)inputString).startsWith(menu[i])) selected_menu=i;
      }
      slave_adrs=((String)inputString).substring((menu[selected_menu].length())+1,((String)inputString).length()).toInt();
      //Serial.println(inputString);
      //Serial.println( menu[0]);



      switch (selected_menu){
        case 0:
          Wire.beginTransmission(slave_adrs); // transmit to device #
          Wire.write(0x02);              // sends one byte
          if (Wire.endTransmission()==0) Serial.println(reply[0]);  // stop transmitting
          else Serial.println(reply[1]);
        break;
        case 1:
          Wire.beginTransmission(slave_adrs); // transmit to device #
          Wire.write(0x02);              // sends one byte
          if (Wire.endTransmission()==0) Serial.println(reply[0]);  // stop transmitting
          else Serial.println(reply[1]);
        break;
        case 2:
          Wire.beginTransmission(slave_adrs); // transmit to device #
          Wire.write(0x01);              // sends one byte
          if (Wire.endTransmission()==0) Serial.println(reply[0]);  // stop transmitting
          else Serial.println(reply[1]);
        break;
        case 3:
          Wire.beginTransmission(slave_adrs); // transmit to device #
          Wire.write(0x01);              // sends one byte
          if (Wire.endTransmission()==0) Serial.println(reply[0]);  // stop transmitting
          else Serial.println(reply[1]);
        break;
        case 4:
        break;
        case 5:
        break;
        case 6:
          Wire.beginTransmission(slave_adrs); // transmit to device #
          Wire.write(0x03);              // sends one byte
          if (Wire.endTransmission()==0) {
            Serial.print("SC on device ");
            Serial.print(slave_adrs);
            Serial.println(" A");
            sc_mode=true;    // stop transmitting
          }
          else {
            Serial.println("Error");
            sc_mode=false; 
          }
        break;
        case 7:
          Wire.beginTransmission(slave_adrs); // transmit to device #
          Wire.write(0x03);              // sends one byte
          if (Wire.endTransmission()==0) {
            Serial.print("SC on device ");
            Serial.print(slave_adrs);
            Serial.println(" B");
            sc_mode=true;    // stop transmitting
          }
          else {
            Serial.println("Error");
            sc_mode=false; 
          }
        break;
        case 8:
        Serial.println(ver);
        break;
        case 9:
        for (int i = 0;i<10;i++) Serial.println(menu[i]);
        break;
        default: 
        Serial.println("Bad command");   
      }
      /*
        if (((String)inputString).startsWith(menu[0])) {
          //slave_adrs=((String)inputString).substring((menu[0].length())+1,((String)inputString).length()).toInt(); //generalize this
          Wire.beginTransmission(slave_adrs); // transmit to device #
          Wire.write(0x02);              // sends one byte
          if (Wire.endTransmission()==0) Serial.println(reply[0]);  // stop transmitting
          else Serial.println(reply[1]);
        }
        else if (((String)inputString).startsWith(menu[1])) {
          //slave_adrs=((String)inputString).substring(4,((String)inputString).length()).toInt();
          Wire.beginTransmission(slave_adrs); // transmit to device #
          Wire.write(0x01);              // sends one byte
          if (Wire.endTransmission()==0) Serial.println(reply[0]);  // stop transmitting
          else Serial.println(reply[1]);
        }  
        else if (((String)inputString).startsWith(menu[2])) {
            //Serial.write("\nPolling status\n");
            //slave_adrs=((String)inputString).substring(7,((String)inputString).length()).toInt();
            byte datain[7];
            int i=0;
            if (Wire.requestFrom(slave_adrs, 7)<7) Serial.println("Error");    // request 3 byte from slave device #
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
              int pinvalue3 = joinbytes(datain[4],datain[5]);
              double temp=0;
              if ((datain[6]) == 0xaa) Serial.println("ON");
              else if ((datain[6]) == 0xcc) Serial.println("OFF");
              
                Serial.print("+5V -> ");
                Serial.print(5.0*((float)pinvalue1/1023));
                Serial.println("A");
                //int pinvalue2 = ((datain[1]<<8) & 0x300) | datain[2];
                Serial.print("-5V -> ");
                Serial.print(5.0*((float)pinvalue2/1023));
                Serial.println("A");
                
                
                Serial.print("Temp -> ");
                temp=(double)pinvalue3/1023.0;
                temp=(temp*9000.0)/(1.0-temp);
                temp=(3455/log(temp/0.102756))-273.15;
                //Serial.print(pinvalue3);
                Serial.print(temp);
                Serial.println("C");
              //}
            }
        }
        else if (((String)inputString).startsWith(menu[3])) {
            //slave_adrs=((String)inputString).substring(3,((String)inputString).length()).toInt();
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
        else {
          Serial.println("Bad command");
        }
    */
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
      
      //Serial.print(y);
      inputString[y] = inChar;
      if (inChar==0x08 & y>0) { 
        Serial.write(inChar); 
        y--; 
      }
      else {
        Serial.write(inChar); 
        y++;
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
