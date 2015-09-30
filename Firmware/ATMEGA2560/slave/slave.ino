//LV Slave reader/writer
//Crate version
//Author: Samer Kilani
//30/09/2015




#include <Wire.h>
byte x =0;
int y=0;
bool SC1=false;
bool SC2=false;
char inputString[64]; 
//-----------------------------
const int EN1pin = 22; 
const int EN2pin = 23; 
const int pos1_ADC = A2; 
const int neg1_ADC = A1; 
const int pos2_ADC = A3; 
const int neg2_ADC = A4; 
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
  //Serial.println("Welcome");
  pinMode(EN1pin, OUTPUT); //to control the output
  pinMode(EN2pin, OUTPUT); //to control the output
  digitalWrite(EN1pin, HIGH);
  digitalWrite(EN2pin, HIGH);
  
  for( int i = 0; i < sizeof(inputString);  ++i )
  inputString[i] = (char)0;
  //Serial.begin(9600);  // start serial for output
}

void loop()
{
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while (Wire.available()) // loop through all but the last
  {
      if (SC1) {
            char c = Wire.read(); // receive byte as a character
            Serial.print(c);         // print the character CHANGE BACK TO SERIAL1 FOR MICRO
      } else if (SC2) {
            char c = Wire.read(); // receive byte as a character
            Serial1.print(c);         // print the character CHANGE BACK TO SERIAL1 FOR MICRO
      }else {

        x = Wire.read();    // receive byte as an integer
        if (x==1) digitalWrite(EN1pin, HIGH);
        else if (x==2) digitalWrite(EN1pin, LOW);
        else if (x==3) digitalWrite(EN2pin, HIGH);
        else if (x==4) digitalWrite(EN2pin, LOW);
        else if (x==5) SC1=true;
        else if (x==6) SC2=true;
      }   
  }
}

void requestEvent()
{
  //Serial.println("requested");
  if (!SC1 && !SC2){
    byte data[12];

    boolean lvstatus1 = digitalRead(EN1pin);
    boolean lvstatus2 = digitalRead(EN2pin);
    unsigned int pinvalue1=0;
    unsigned int pinvalue2=0;
    unsigned int pinvalue3=0;
    unsigned int pinvalue4=0;
    unsigned int pinvalue5=0;
    //AVERAGING THE CURRENT READINGS OVER 100MS
    for (int avg=0;avg<11;avg++)
    {
      pinvalue1 += analogRead(pos1_ADC);
      pinvalue2 += analogRead(neg1_ADC);
      pinvalue3 += analogRead(pos2_ADC);
      pinvalue4 += analogRead(neg2_ADC);      
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
    
    
    if (lvstatus1==true) data[10] = 0xcc;
    else data[10] = 0xaa;
    if (lvstatus2==true) data[11] = 0xcc;
    else data[11] = 0xaa;
    Wire.write(data,12);
    
    
  } else {
    //Serial.println("requested2");
    if (SC1) {
      while (Serial.available()) { 
      char inChar = Serial.read();
      inputString[y] = inChar;
      y++;
    }
    }
    else if (SC2) {
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
  }
}






