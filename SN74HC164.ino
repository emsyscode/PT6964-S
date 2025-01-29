/****************************************************/
/* This is only one example of code structure       */
/* OFFCOURSE this code can be optimized, but        */
/* the idea is let it so simple to be easy catch    */
/* where can do changes and look to the results     */
/****************************************************/
#define VFD_in 8// If 0 write LCD, if 1 read of LCD
#define VFD_clk 9 // if 0 is a command, if 1 is a data0
#define VFD_stb 10 // Must be pulsed to LCD fetch data of bus
#define VFD_onGreen 11 // Led on/off Green
#define VFD_onRed 12 // Led on/off Red
//
#define bright 0x03 // This is to be usded at command CM4 to define intensity of segments. Varie between 0 to 7.

unsigned char grid;
unsigned char gridSegments = 0b00000011; // Here I define the number of GRIDs and Segments I'm using
//This panel use 7 digits and 10 segments, it is max value to the PT6964. See page 5 "display mode settings"

unsigned int k=0;
boolean flag=true;

unsigned int numbers0to6[] ={
  //This respect the table for 7 segm like "abcdefgh", but at vertical// 
  //Dig:S6543210              //S means symbols
      0b01101101, 0b00000000, //seg a   
      0b00011111, 0b00000000, //seg b     
      0b01111011, 0b00000000, //seg c   
      0b01101101, 0b00000000, //seg d    
      0b01000101, 0b00000000, //seg e   
      0b01110001, 0b00000000, //seg f   
      0b01111100, 0b00000000, //seg g   
      0b00000000, 0b00000000, //seg h   
      0b00000000, 0b00000000, //symb   
      0b00000000, 0b00000000 //symb   
  };
unsigned int numbers7to9[] ={
  //This respect the table for 7 segm like "abcdefgh", but at vertical// 
  //Dig:S6543210              //S means symbols
      0b00000111, 0b00000000, //seg a   
      0b00000111, 0b00000000, //seg b     
      0b00000111, 0b00000000, //seg c   
      0b00000010, 0b00000000, //seg d    
      0b00000010, 0b00000000, //seg e   
      0b00000110, 0b00000000, //seg f   
      0b00000110, 0b00000000, //seg g   
      0b00000000, 0b00000000, //seg h   
      0b00000000, 0b00000000, //symb   
      0b00000000, 0b00000000 //symb   
  };
unsigned int msgHello[] ={
  //This respect the table for 7 segm like "abcdefgh", but at vertical// 
  //Dig:S6543210              //S means symbols. The digit "0" is firts digit at left!!!
      0b00010010, 0b00000000, //seg a   
      0b00010001, 0b00000000, //seg b     
      0b00010001, 0b00000000, //seg c   
      0b00011110, 0b00000000, //seg d    
      0b00011111, 0b00000000, //seg e   
      0b00011111, 0b00000000, //seg f   
      0b00000011, 0b00000000, //seg g   
      0b00000000, 0b00000000, //seg h   
      0b00000000, 0b00000000, //symb   
      0b00000000, 0b00000000 //symb   
  };
unsigned int msgPeople[] ={
  //This respect the table for 7 segm like "abcdefgh", but at vertical// 
  //Dig:S6543210              //S means symbols. The digit "0" is firts digit at left!!!
      0b00101111, 0b00000000, //seg a   
      0b00001101, 0b00000000, //seg b     
      0b00000100, 0b00000000, //seg c   
      0b00110110, 0b00000000, //seg d    
      0b00111111, 0b00000000, //seg e   
      0b00111111, 0b00000000, //seg f   
      0b00101011, 0b00000000, //seg g   
      0b00000000, 0b00000000, //seg h   
      0b00000000, 0b00000000, //symb   
      0b00000000, 0b00000000 //symb   
  };

//******************* Init and send ZONE ************************/
void pt6964_init(void){
  delayMicroseconds(200); //power_up delay
  // Note: Allways the first byte in the input data after the STB go to LOW is interpret as command!!!

  // Configure VFD display (grids)
  cmd_with_stb(gridSegments); // cmd 1// Command to define the number of grids and segments
  delayMicroseconds(1);
  // Write to memory display, increment address, normal operation
  cmd_with_stb(0b01000000);//(BIN(01000000));
  delayMicroseconds(1);
  cmd_with_stb(0b11000000);//(BIN(01100110)); 
  delayMicroseconds(1);
  // set DIMM/PWM to value
  cmd_with_stb((0b10001000) | bright);//0 min - 7 max  )(0b01010000)
  delayMicroseconds(1);
}
void cmd_without_stb(unsigned char a){
  // send without stb
  unsigned char data = 170; //value to transmit, binary 10101010
  unsigned char mask = 1; //our bitmask
  
  data=a;
  //This don't send the strobe signal, to be used in burst data send
   for (mask = 0b00000001; mask>0; mask <<= 1) { //iterate through bit mask
     digitalWrite(VFD_clk, LOW);
     if (data & mask){ // if bitwise AND resolves to true
        digitalWrite(VFD_in, HIGH);
     }
     else{ //if bitwise and resolves to false
       digitalWrite(VFD_in, LOW);
     }
    delayMicroseconds(5);
    digitalWrite(VFD_clk, HIGH);
    delayMicroseconds(5);
   }
   //digitalWrite(VFD_clk, LOW);
}
void cmd_with_stb(unsigned char a){
  // send with stb
  unsigned char data = 170; //value to transmit, binary 10101010
  unsigned char mask = 1; //our bitmask
  data=a;
  
  //This send the strobe signal
  //Note: The first byte input at in after the STB go LOW is interpreted as a command!!!
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(1);
   for (mask = 0b00000001; mask>0; mask <<= 1) { //iterate through bit mask
     digitalWrite(VFD_clk, LOW);
     delayMicroseconds(1);
     if (data & mask){ // if bitwise AND resolves to true
        digitalWrite(VFD_in, HIGH);
     }
     else{ //if bitwise and resolves to false
       digitalWrite(VFD_in, LOW);
     }
    digitalWrite(VFD_clk, HIGH);
    delayMicroseconds(1);
   }
   digitalWrite(VFD_stb, HIGH);
   delayMicroseconds(1);
}
/******************* End ZONE of Init Send***********************/
void test_LED(void){
  //This function is only to teste the led's ON green ON red.
   digitalWrite(VFD_onGreen, HIGH);
   delay(1000);
   digitalWrite(VFD_onGreen, LOW);
   digitalWrite(VFD_onRed, HIGH);
   delay(1000);
   digitalWrite(VFD_onRed, LOW);
   delay(1000);
}
void test_LED_panel_Philips(void){
  clearDisplay();
      
      digitalWrite(VFD_stb, LOW);
      delayMicroseconds(1);
      cmd_with_stb(gridSegments); // cmd 1 // 7 digits & 10 Segments on this panel
      cmd_with_stb(0b01000000); // cmd 2 //Normal operation; Set pulse as 1/16
            for (int n=0; n < 8; n++){
                       for (int i = 0; i < 8 ; i++){ // test base to 16 segm and 6 grids
                          digitalWrite(VFD_stb, LOW);
                          delayMicroseconds(1);
                            cmd_without_stb(0b11000000 | n); //cmd 3 wich define the start address (00H to 15H)
                            cmd_without_stb(0b00000001 << i); // 
                            cmd_without_stb(0b00000001 << i); // 
                            digitalWrite(VFD_stb, HIGH);
                           cmd_with_stb((0b10001000) | bright); //cmd 4
                          delay(400);
                       }
             }
}
void allSegmentsOn(void){
  /* 
  Here do a test for all segments of 7 grids
  each grid is controlled by a group of 2 bytes
  by these reason I'm send a burst of 2 bytes of
  data. The cycle for do a increment of 3 bytes on 
  the variable "i" on each test cycle of FOR.
  */
  // to test 6 grids is 6*3=18, the 8 grid result in 8*3=24.
  clearDisplay();
      digitalWrite(VFD_stb, LOW);
      delayMicroseconds(1);
      cmd_with_stb(gridSegments); // cmd 1// Command to define the number of grids and segments
      cmd_with_stb(0b01000000); // cmd 2 //Normal operation; Set pulse as 1/16
      
        digitalWrite(VFD_stb, LOW);
        delayMicroseconds(1);
        cmd_without_stb((0b11000000)); //cmd 3 wich define the start address (00H to 15H)
             for (uint8_t i = 0; i < 7 ; i++){ // Pay attention are 7 digits, each digit use 2 bytes (0~6 * 16 bits)
             cmd_without_stb(0b11111111); // Data to fill table 7*16 = 112 bits
             cmd_without_stb(0b11111111); // Data to fill table 7*16 = 112 bits
             delayMicroseconds(2);
             }
          digitalWrite(VFD_stb, HIGH); //Display memory is updated by incrementing addresses. Please refer to the following diagram. , see page 10.
          delayMicroseconds(1);
      cmd_with_stb(gridSegments); // cmd 1// Command to define the number of grids and segments
      cmd_with_stb((0b10001000) | bright); //cmd 4
      delay(1);
      delay(500);
}
void testNumbers0to6(void){
  clearDisplay();
      //
      digitalWrite(VFD_stb, LOW);
      delayMicroseconds(1);
      cmd_with_stb(gridSegments); // cmd 1// Command to define the number of grids and segments
      cmd_with_stb(0b01000000); // cmd 2 //Normal operation; Set pulse as 1/16
      //
      cmd_with_stb((0b11000000)); //cmd 3 wich define the start address (00H to 15H)
      for (int i = 0; i < 14 ; i=i+2){ // test base to 15 segm and 6 grids
        digitalWrite(VFD_stb, LOW);
        delayMicroseconds(1);
        cmd_without_stb((0b11000000) | i);
      
             cmd_without_stb(numbers0to6[i]); // Data to fill table 6*16 = 96 bits
             cmd_without_stb(numbers0to6[i+1]); // Data to fill table 6*16 = 96 bits
             digitalWrite(VFD_stb, HIGH);
             cmd_with_stb((0b10001000) | bright); //cmd 4
        delay(5); 
      }
}
void testNumbers7to9(void){
  clearDisplay();
      //
      digitalWrite(VFD_stb, LOW);
      delayMicroseconds(1);
      cmd_with_stb(gridSegments); // cmd 1// Command to define the number of grids and segments
      cmd_with_stb(0b01000000); // cmd 2 //Normal operation; Set pulse as 1/16
      //
      cmd_with_stb((0b11000000)); //cmd 3 wich define the start address (00H to 15H)
      for (int i = 0; i < 14 ; i=i+2){ // test base to 15 segm and 6 grids
        digitalWrite(VFD_stb, LOW);
        delayMicroseconds(1);
        cmd_without_stb((0b11000000) | i); // This is the address of memory
      
             cmd_without_stb(numbers7to9[i]); // Data to fill table 7*16
             cmd_without_stb(numbers7to9[i+1]); // Data to fill table 7*16
             digitalWrite(VFD_stb, HIGH);
             cmd_with_stb((0b10001000) | bright); //cmd 4
        delay(5); 
      }
}
void messageHello(void){
  clearDisplay();
      //
      digitalWrite(VFD_stb, LOW);
      delayMicroseconds(1);
      cmd_with_stb(gridSegments); // cmd 1// Command to define the number of grids and segments
      cmd_with_stb(0b01000000); // cmd 2 //Normal operation; Set pulse as 1/16
      //
      cmd_with_stb((0b11000000)); //cmd 3 wich define the start address (00H to 15H)
      for (int i = 0; i < 14 ; i=i+2){ // test base to 15 segm and 6 grids
        digitalWrite(VFD_stb, LOW);
        delayMicroseconds(1);
        cmd_without_stb((0b11000000) | i);
      
             cmd_without_stb(msgHello[i]); // Data to fill table 7*16
             cmd_without_stb(msgHello[i+1]); // Data to fill table 7*16
             digitalWrite(VFD_stb, HIGH);
             cmd_with_stb((0b10001000) | bright); //cmd 4
        delay(5); 
      }
}
void messagePeople(void){
  clearDisplay();
      //
      digitalWrite(VFD_stb, LOW);
      delayMicroseconds(1);
      cmd_with_stb(gridSegments); // cmd 1// Command to define the number of grids and segments
      cmd_with_stb(0b01000000); // cmd 2 //Normal operation; Set pulse as 1/16
      //
      cmd_with_stb((0b11000000)); //cmd 3 wich define the start address (00H to 15H)
      for (int i = 0; i < 14 ; i=i+2){ // test base to 15 segm and 6 grids
        digitalWrite(VFD_stb, LOW);
        delayMicroseconds(1);
        cmd_without_stb((0b11000000) | i);
      
             cmd_without_stb(msgPeople[i]); // Data to fill table 7*16
             cmd_without_stb(msgPeople[i+1]); // Data to fill table 7*16
             digitalWrite(VFD_stb, HIGH);
             cmd_with_stb((0b10001000) | bright); //cmd 4
        delay(5); 
      }
}
void clearDisplay(void){
  /*
  Here I clean all registers 
  Could be done only on the number of grid
  to be more fast. The 12 * 3 bytes = 36 registers
  */
      for (int n=0; n < 14; n++){  // important be 14
        cmd_with_stb(gridSegments); // cmd 1// Command to define the number of grids and segments
        cmd_with_stb(0b01000000); //   cmd 2 //Normal operation; Set pulse as 1/16
        digitalWrite(VFD_stb, LOW);
        delayMicroseconds(1);
            cmd_without_stb((0b11000000) | n); // cmd 3 //wich define the start address (00H to 15H)
            cmd_without_stb(0b00000000); // Data to fill table of 7 grids * 14 segm
            //
            //cmd_with_stb((0b10001000) | bright); //cmd 4
            digitalWrite(VFD_stb, HIGH);
            delayMicroseconds(100);
     }
}
void readButtons(){
  //Take special attention to the initialize digital pin LED_BUILTIN as an output.
  //
  int ledPin = 13;   // LED connected to digital pin 13
  int inPin = 8;     // pushbutton connected to digital pin 7
  int val = 0;       // variable to store the read value
  int dataIn=0;

  byte array[8] = {0,0,0,0,0,0,0,0};
  byte together = 0;

  unsigned char data = 0; //value to transmit, binary 10101010
  unsigned char mask = 1; //our bitmask

  array[0] = 1;

  unsigned char btn1 = 0x41;

      digitalWrite(VFD_stb, LOW);
        delayMicroseconds(2);
      cmd_without_stb(0b01000010); // cmd 2 //Read Keys;Normal operation; Set pulse as 1/16
       // cmd_without_stb((0b11000000)); //cmd 3 wich define the start address (00H to 15H)
     // send without stb
  
  pinMode(8, INPUT_PULLUP);  // Important this point! Here I'm changing the direction of the pin to INPUT data.
  delayMicroseconds(2);
  //PORTD != B01010100; // this will set only the pins you want and leave the rest alone at
  //their current value (0 or 1), be careful setting an input pin though as you may turn 
  //on or off the pull up resistor  
  //This don't send the strobe signal, to be used in burst data send
         for (int z = 0; z < 5; z++){
             //for (mask=00000001; mask > 0; mask <<= 1) { //iterate through bit mask
                   for (int h =8; h > 0; h--) {
                      digitalWrite(VFD_clk, HIGH);  // Remember wich the read data happen when the clk go from LOW to HIGH! Reverse from write data to out.
                      delayMicroseconds(2);
                     val = digitalRead(inPin);
                      //digitalWrite(ledPin, val);    // sets the LED to the button's value
                           if (val & mask){ // if bitwise AND resolves to true
                             //Serial.print(val);
                            //data =data | (1 << mask);
                            array[h] = 1;
                           }
                           else{ //if bitwise and resolves to false
                            //Serial.print(val);
                           // data = data | (1 << mask);
                           array[h] = 0;
                           }
                    digitalWrite(VFD_clk, LOW);
                    delayMicroseconds(2);
                   } 
             
              Serial.print(z);  // All the lines of print is only used to debug, comment it, please!
              Serial.print(" - " );
                        
                                  for (int bits = 7 ; bits > -1; bits--) {
                                      Serial.print(array[bits]);
                                   }
                        
                        if (z==0){
                          if(array[7] == 1){
                           //hours++;
                          }
                        }
                          if (z==0){
                            if(array[4] == 1){
                             //hours--;
                            }
                          }
                          if (z==1){
                            if(array[7] == 1){
                             //minutes++;
                            }
                        }
                        if (z==1){
                          if(array[4] == 1){
                           //minutes--;
                          }
                        }
                        if (z==2){
                          if(array[7] == 1){
                          //  hours = 0;
                          //  minutes = 0;
                          }
                        }
                         
                        if (z==2){
                            if(array[4] == 1){
                              //digitalWrite(VFD_net, !digitalRead(VFD_net));
                          }
                        }
                        
                        if (z==3){
                           if(array[7] == 1){
                             digitalWrite(VFD_onRed, !digitalRead(VFD_onRed));
                             digitalWrite(VFD_onGreen, !digitalRead(VFD_onGreen));
                            }
                          }                        
                  Serial.println();
          }  // End of "for" of "z"
      Serial.println();  // This line is only used to debug, please comment it!

 digitalWrite(VFD_stb, HIGH);
 delayMicroseconds(2);
 cmd_with_stb((0b10001000) | bright); //cmd 4
 delayMicroseconds(2);
 pinMode(8, OUTPUT);  // Important this point!  // Important this point! Here I'm changing the direction of the pin to OUTPUT data.
 delay(1); 
}
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  //
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  //
  pt6964_init();
  clearDisplay();
}
void loop() {
  test_LED_panel_Philips();
  allSegmentsOn();
  delay(1000);
  messageHello();
  delay(800);
  messagePeople();
  delay(1200);
  testNumbers0to6();
  delay(1000);
  testNumbers7to9();
  delay(1000);
  clearDisplay();
  test_LED();
  //
       readButtons();
}
