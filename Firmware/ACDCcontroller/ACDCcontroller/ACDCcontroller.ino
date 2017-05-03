#include <Wire.h>
#include <TFT.h>  // Arduino LCD library
#include <SPI.h>
#include <assert.h>

// pin definition for the Uno
#define cs   10
#define dc   9
#define rst  8

#define INH 3 //inhibit signal

TFT TFTscreen = TFT(cs, dc, rst);
//char sensorPrintout[4];
float old_current1=-2;
float old_voltage1=-2;
float new_voltage=0;
float old_current2=-2;
float old_voltage2=-2;
float new_current=0;
bool i2cstatus1=false;
bool i2cstatus2=false;

char sensorPrintout1[5];
char sensorPrintout2[5];




void setup() {
  Wire.begin();                // join i2c bus with address #8
  //Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output

  // Put this line at the beginning of every sketch that uses the GLCD:
  TFTscreen.begin();

  // clear the screen with a black background
  TFTscreen.background(0, 0, 0);

  TFTscreen.stroke(255, 255, 255);
  TFTscreen.line(75, 0, 75, TFTscreen.height());



  // write the static text to the screen
  // set the font color to white
  TFTscreen.stroke(255, 255, 255);
  // set the font size
  TFTscreen.setTextSize(2);
  // write the text to the top left corner of the screen
  TFTscreen.setCursor(10,10);
  TFTscreen.print("V1");
  TFTscreen.setCursor(90,10);
  TFTscreen.print("V2");
  TFTscreen.setCursor(10,70);
  TFTscreen.print("I1");
  TFTscreen.setCursor(90,70);
  TFTscreen.print("I2");
  // ste the font size very large for the loop
  TFTscreen.setTextSize(2.5);

  //pinMode(inh,OUTPUT);
  //digitalWrite(inh,LOW);

}

void loop() {
  //delay(2000);
//Serial.println(linear(0xba,0x00));
readPSUout();

delay(1000);


}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
//void receiveEvent(int howMany) {
//  while (1 < Wire.available()) { // loop through all but the last
//    char c = Wire.read(); // receive byte as a character
//    Serial.print(c);         // print the character
//  }
//  //int x = Wire.read();    // receive byte as an integer
//  //Serial.println(x);         // print the integer
//}


void readPSUout()
{
    
    bool alive=false;
  
    
    
    
    readVoltage(0x59,&alive,&new_voltage);
    
    delay(50);
    
  if (new_voltage!=old_voltage1) {
    // erase the text you just wrote
    TFTscreen.stroke(0, 0, 0);
    TFTscreen.setCursor(10,30);
    if (alive==true) TFTscreen.print(old_voltage1); else TFTscreen.print("Error");
    
    //Serial.println();
    TFTscreen.stroke(255, 255, 255);
    // print the sensor value
    TFTscreen.setCursor(10,30);
    if (alive==true) TFTscreen.print(new_voltage); else TFTscreen.print("Error");
    old_voltage1=new_voltage;
  }

  readCurrent(0x59,&alive,&new_current);
  delay(50);
  
  if (new_current!=old_current1) {
    //Serial.println();
    // erase the text you just wrote
    TFTscreen.stroke(0, 0, 0);
    TFTscreen.setCursor(10,90);
    if (alive==true) TFTscreen.print(old_current1); else TFTscreen.print("Error");
    //TFTscreen.print(old_current1);
    TFTscreen.stroke(255, 255, 255);
    // print the sensor value
    TFTscreen.setCursor(10,90);
    if (alive==true) TFTscreen.print(new_current); else TFTscreen.print("Error");
    //TFTscreen.print(new_current);
    old_current1=new_current;
  }




    readVoltage(0x58,&alive,&new_voltage);
    delay(50);
    
  if (new_voltage!=old_voltage2) {
    // erase the text you just wrote
    TFTscreen.stroke(0, 0, 0);
    TFTscreen.setCursor(90,30);
    if (alive==true) TFTscreen.print(old_voltage2); else TFTscreen.print("Error");
    
    //Serial.println();
    TFTscreen.stroke(255, 255, 255);
    // print the sensor value
    TFTscreen.setCursor(90,30);
    if (alive==true) TFTscreen.print(new_voltage); else TFTscreen.print("Error");
    old_voltage2=new_voltage;
  }

    readCurrent(0x58,&alive,&new_current);
    delay(50);
      
  if (new_current!=old_current2) {
    //Serial.println();
    // erase the text you just wrote
    TFTscreen.stroke(0, 0, 0);
    TFTscreen.setCursor(90,90);
    if (alive==true) TFTscreen.print(old_current2); else TFTscreen.print("Error");
    //TFTscreen.print(old_current2);
    TFTscreen.stroke(255, 255, 255);
    // print the sensor value
    TFTscreen.setCursor(90,90);
    if (alive==true) TFTscreen.print(new_current); else TFTscreen.print("Error");
    //TFTscreen.print(new_current);
    old_current2=new_current;
  }

    
}


void readVoltage(byte adrs, bool* alive, float* value)
{
  int data=0;

  assert(alive);
  assert(value);


  
  Wire.beginTransmission(adrs); // start transmission
  Wire.write(0x8B); // send command
  if (Wire.endTransmission(false)>0) {
    Serial.println("Error"); 
    *alive=false;
    return;
  }
  *alive=true;
  
  int l=Wire.requestFrom(adrs, (uint8_t)2); // request expected number of bytes to be returned
  //delay(500);
  Serial.println("Voltage:");
  int i=0;
  while(l>0 && Wire.available()) // loop all bytes if any are available
  {
    Serial.println("in the loop!");
    int temp= Wire.read();
    Serial.print(temp,HEX); // print em out
    Serial.print(" ");
    data = (temp<<(8*i));
    i++;
  }
  Serial.println();
  //out=(28.58/14638)*data;
  *value=((float)data/512.0);
  //return out;
}

float readCurrent(byte adrs, bool* alive, float* value)
{
  char data[2]={0,0};


  
  assert(alive);
  assert(value);

  
  Wire.beginTransmission(adrs); // start transmission
  Wire.write(0x8C); // send command
  if (Wire.endTransmission(false)>0) {
    Serial.println("Error"); // don't send a stop condition
    *alive=false;
    return;//change back to -1 if needed
  }
  *alive=true;
  
  int l=Wire.requestFrom(adrs, (uint8_t)2); // request expected number of bytes to be returned
  //delay(500);
  Serial.println("Current:");
  int i=0;
  while(l>0 && Wire.available()) // loop all bytes if any are available
  {
    //Serial.println("in the loop!");
    int temp= Wire.read();
    Serial.print(temp,HEX); // print em out
    Serial.print(" ");
    data[i] = temp;
    i++;
  }
  Serial.println();
  *value=linear(data[1],data[0]);
}

float linear(unsigned char byte1, unsigned char byte2)
{
  int exponent=((byte1)&0xf8)>>3;

  if ((exponent&0x10)==0x10) {
    exponent=(~exponent&0x1f);
    //printf("1=%d\n",exponent);
    //two's complement to one's complement
    exponent++;
    //check if negative
    exponent=-1*(exponent);
  }
  

  
  //joing the 11bit mantissa
  int mantissa=byte2+(((byte1)&0x07)<<8);
  //printf("1=%d\n",mantissa);
  //two's complement to one's complement
  if ((mantissa&0x400)==0x400) {
    mantissa=(mantissa)&0x7ff;
    mantissa--;
    mantissa=(~mantissa)&0x3ff;
    //printf("3=%d\n",mantissa);
    mantissa=-1*(mantissa&0x7ff);
  }


  //printf("exponent = %d\n",exponent);
  //printf("mantissa = %d\n",mantissa);

  return mantissa*pow(2,exponent);
  
}



void test()
{
  
}



