//LV Slave reader/writer
//Author: Samer Kilani
//19/01/2015

##############################################
#Uncomment the I2C address below
#Board A
#define i2cadrs 1
#Board B
##define i2cadrs 2
##############################################


#include <Wire.h>
byte x =0;
int y=0;
bool SC=false;
char inputString[64]; 
void setup()
{
  Wire.begin(i2cadrs);                // join i2c bus with address #2
  TWAR = (i2cadrs << 1) | 1;
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event
  //Serial1.begin(115200); //for micro
  Serial.begin(115200); //for uno
  pinMode(A2, OUTPUT); //to control the output
  digitalWrite(A2, HIGH);
  
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
        if (x==1) digitalWrite(A2, HIGH);
        else if (x==2) digitalWrite(A2, LOW);
        else if (x==3) SC=true;
      }   
  }
}

void requestEvent()
{
  //Serial.println("requested");
  if (!SC){
    byte data[5];
    //the UNO has 10bit ADCs. To send the value of 2 ADCs we need 3 bytes.
    //the values are read from the ADCs as integers and converted into a single three byte stream of bits.
    //i.e. 1023 and 1023 becomes 1048575 == 0000 1111 1111 1111 1111 1111 == 0x0fffff
    //to do this the first integer (X) needs to be shifted by 6 bits (towards the lsb)
    //XXXX XXXX XX >> 0000 00XX XX
    //The int is then casted to a char which will get rid of the first two 00.
    boolean lvstatus = digitalRead(A2);
    unsigned int pinvalue1=0;
    unsigned int pinvalue2=0;
    //AVERAGING THE CURRENT READINGS OVER 100MS
    for (int avg=0;avg<11;avg++)
    {
      pinvalue1 += analogRead(A0);
      pinvalue2 += analogRead(A1);
      delay(20);
    }
    
    pinvalue1=pinvalue1/10;
    pinvalue2=pinvalue2/10;
    
//    
//    data[0] = pinvalue1 >> 6;
//    //now we have the first 8 bits
//    //[0000 XXXX] XXXX XXYY YYYY YYYY
//    //To obtain the next byte, the integer X needs to be shifted by 2 bits (towards the msb)
//    //This is then ANDed with 1111 1100 to obtaine XXXX XX00 which will be ORed with our next byte
//    data[1] = pinvalue1 << 2;
//    data[1] = data[1] & 0xfc;
//  
//    //The next byte is prepared by shifting the Y integer 8 bits (towards the lsb)
//    //this is then ORed with the previous byte
//    //This way we obtain the 2nd byte
//    //0000 XXXX [XXXX XXYY] YYYY YYYY
//    unsigned char tmp = pinvalue2 >> 8;
//    data[1] = data[1] | tmp;
//  
//    //the third and final byte is simply the integer Y as is casted into a char
//    data[2] = pinvalue2;
//    
//    if (lvstatus==true) data[0] = (data[0] & 0x0f) | 0xc0; //lvstatus is inverted true is OFF and vice versa
//    else data[0] = (data[0] & 0x0f) | 0x50;
//    //this will include the on/off status of the LV board. where ZZZZ is either 0xc or 0x5 (on or off, respectively).
//    //[ZZZZ] XXXX XXXX XXYY YYYY YYYY
//    
//    Wire.write(data,3); 

    data[0] = pinvalue1&0xff;              // sends one byte
    //Serial.println(pinvalue1&0xff,HEX);
    data[1] = pinvalue1>>8;
    //Serial.println(pinvalue1>>8,HEX);
    
    data[2] = pinvalue2&0xff;              // sends one byte
    //Serial.println(pinvalue2&0xff,HEX); 
    data[3] = pinvalue2>>8;
    //Serial.println(pinvalue2>>8,HEX);
    
    if (lvstatus==true) data[4] = 0xcc;
    else data[4] = 0xaa;
    
    Wire.write(data,5);
    
    
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





