
bool stringComplete=false;
void setup() {
 Serial.begin(9600);

}

// the loop routine runs over and over again forever:
void loop() {
  SerialEvent();
   if (stringComplete)
 {
   float current1 = ((float)analogRead(A5) - 511.0) / 20.0;
   float current2 = ((float)analogRead(A0) - 511.0) / 20.0;
   float voltage1 = ((float)analogRead(A4)*5.0)/1023.0;
   voltage1 = (voltage1-(141.0/53.0))/(94.0/265.0);
   float voltage2 = ((float)analogRead(A3)*5.0)/1023.0; 
   voltage2 = (voltage2-(141.0/53.0))/(94.0/265.0);
   
   Serial.print("+ve supply voltage ");
   Serial.print(voltage1);
   Serial.println("V");
   Serial.print("+ve supply current ");
   Serial.print(current1);
   Serial.println("A");
   
   Serial.print("-ve supply voltage ");
   Serial.print(voltage2);
   Serial.println("V");
   Serial.print("-ve supply current ");
   Serial.print(current2);
   Serial.println("A");   
 }
stringComplete=false;
}


void SerialEvent()
{
//  unsigned char data[5];
//  //the UNO has 10bit ADCs. To send the value of 2 ADCs we need 3 bytes.
//  //the values are read from the ADCs as integers and converted into a single three byte stream of bits.
//  //i.e. 1023 and 1023 becomes 1048575 == 0000 1111 1111 1111 1111 1111 == 0x0fffff
//  //to do this the first integer (X) needs to be shifted by 6 bits (towards the lsb)
//  //XXXX XXXX XX >> 0000 00XX XX
//  //The int is then casted to a char which will get rid of the first two 00.
//  unsigned int pinvalue1 = 1023;//analogRead(A0); //current for negative supply W
//  unsigned int pinvalue2 = 512;//analogRead(A5); //current for positive supply X
//  unsigned int pinvalue3 = 865;//analogRead(A1); //voltage for negative supply Y
//  unsigned int pinvalue4 = 999;//analogRead(A2); //voltage for positive supply Z  
//  
//  //W X Y Z
//  //WWWW WWWW WWXX XXXX XXXX YYYY YYYY YYZZ ZZZZ ZZZZ
//  //DATA[0] = WWWW WWWW
//  //DATA[1] = WWXX XXXX
//  //DATA[2] = XXXX YYYY
//  //DATA[3] = YYYY YYZZ
//  //DATA[4] = ZZZZ ZZZZ
//  
//  data[0] = pinvalue1 >> 2;
//  data[4] = pinvalue4 & 0xff;
//  
//  data[1] = ((pinvalue1 & 0x03) << 6) | ((pinvalue2 >> 4) & 0x3f);
//  
//  data[2] = ((pinvalue2 & 0x0f) << 4) | ((pinvalue3 >> 6) & 0x0f) ;
//
//  data[3] = ((pinvalue3 << 2) & 0xfc) | (pinvalue4 & 0x03);
//
//  Wire.write(data,5);

  
  
  
  
  
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:

    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\r') {
      stringComplete = true;
    }//else inputString += inChar;
  }
}
