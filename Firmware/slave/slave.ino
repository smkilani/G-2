//LV Slave reader/writer
//Author: Samer Kilani
//19/01/2015

//##############################################
//#Uncomment the I2C address below
//#Board A
//#define i2cadrs 1
//#Board B
#define i2cadrs 2
//##############################################


#include <Wire.h>
byte x =0;
int y=0;
bool SC=false;
char inputString[64]; 
const int ENpin = A0; //A2 for Rev 1.0
const int pos_ADC = A1; //A0 for Rev 1.0
const int neg_ADC = A2; //A1 for Rev 1.0
const int NTC_ADC = A3;


void setup()
{
  Wire.begin(i2cadrs);                // join i2c bus with address #2
  TWAR = (i2cadrs << 1) | 1;
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event
  //Serial1.begin(115200); //for micro
  Serial.begin(115200); //for uno
  //Serial.println("Welcome");
  pinMode(ENpin, OUTPUT); //to control the output
  digitalWrite(ENpin, LOW);
  
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
      if (SC) {
            char c = Wire.read(); // receive byte as a character
            Serial.print(c);         // print the character CHANGE BACK TO SERIAL1 FOR MICRO
      }else {

        x = Wire.read();    // receive byte as an integer
        if (x==1) digitalWrite(ENpin, HIGH);
        else if (x==2) digitalWrite(ENpin, LOW);
        else if (x==3) SC=true;
      }   
  }
}

void requestEvent()
{
  //Serial.println("requested");
  if (!SC){
    byte data[7];

    boolean lvstatus = digitalRead(ENpin);
    unsigned int pinvalue1=0;
    unsigned int pinvalue2=0;
    unsigned int pinvalue3=0;
    //AVERAGING THE CURRENT READINGS OVER 100MS
    for (int avg=0;avg<11;avg++)
    {
      pinvalue1 += analogRead(pos_ADC);
      pinvalue2 += analogRead(neg_ADC);
      pinvalue3 += analogRead(NTC_ADC);
      delay(20);
    }
    
    pinvalue1=pinvalue1/10; //current reading +ve
    pinvalue2=pinvalue2/10; //current reading -ve
    pinvalue3=pinvalue3/10; //NTC temp reading

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
    
    if (lvstatus==true) data[6] = 0xcc;
    else data[6] = 0xaa;
    
    Wire.write(data,7);
    
    
  } else {
    //Serial.println("requested2");
      while (Serial.available()) { //CHANGE BACK TO SERIAL1 FOR MICRO
      char inChar = Serial.read();
      inputString[y] = inChar;
      y++;
    }
    y=0;
    Wire.write(inputString);
    for( int i = 0; i < sizeof(inputString);  ++i )
    inputString[i] = (char)0;
  }
}





