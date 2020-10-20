#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#include <HX711.h>
#include <EEPROM.h>

#define DOUT  6
#define CLK  5

HX711 scale;

float calibration_factor = 795;
int offset = 0;

int but1 = 2;
int but2 = 3;
int but3 = 4;
int but4 = 7;
int lastState1 = LOW;
int lastState2 = LOW;
int lastState3 = LOW;
int lastState4 = LOW;
int val = 0;
int setWeight = 0;
int error = 0;
int Button = 0;
int OldButton = 0;

volatile byte state = LOW;

int a = 0; //EEPROM Address 1
int b = 1; //EEPROM Address 2


void setup() {

Serial.begin(9600);

  scale.begin(DOUT, CLK);
  scale.set_scale();

  long zero_factor = scale.read_average();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  
  pinMode(but1, INPUT);
  pinMode(but2, INPUT);
  pinMode(but3, INPUT);
  pinMode(but4, INPUT);

  attachInterrupt(digitalPinToInterrupt(2), butstate, RISING);

  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();



    // eeprom_address += EEPROM_writeAnything(eeprom_address, error); //write a to memory and then updates eeprom address

}

void loop() {

 do{
  calibrate();

 }
 while(state == 1); 


 do{
  test();
  
 }
 while(state == 0); 
   
 
 OldButton = Button; //The data is now old
}

void test() {

  scale.set_scale(calibration_factor); //Adjust to this calibration factor

error = EEPROM.read(a);
offset = EEPROM.read(b);

 int reading = (scale.get_units()+ offset);

int actual = reading - error;

//   Serial.println(actual); //Uncomment for Debugging

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.clearDisplay();
  display.print(F("Weight:"));
  display.setTextSize(2);
  display.setCursor(50,10);
  display.print(actual);
  display.setCursor(100,10);
  display.print("g");
  display.display();

delay (50);

}  

void calibrate() {

scale.set_scale(calibration_factor);

int buttonState1 = digitalRead(but1);
int buttonState2 = digitalRead(but2);
int buttonState3 = digitalRead(but3);
int buttonState4 = digitalRead(but4);

  if (buttonState3 == HIGH && lastState3 == LOW){
    error = setWeight;
    EEPROM.write(a,error);
    Serial.print("Error: ");
    Serial.println(error);
    
      display.setTextSize(1); // Draw 2X-scale text
      display.setTextColor(SSD1306_WHITE);
      display.clearDisplay();
      display.setCursor(0,0);
      display.print("Spool Weight Saved!");
      display.display();
      
delay(3000);
  }

  
  if (buttonState2 == HIGH && lastState2 == LOW){
    setWeight = setWeight + 10;
    Serial.println(setWeight);
  }

  if (buttonState4 == HIGH && lastState4 == LOW){
  offset = scale.get_units();
  offset = -offset;
  EEPROM.write(b, offset);
  Serial.println(scale.get_units());
  Serial.println(EEPROM.read(b));
  Serial.println(offset);
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Scale Tare!");
  display.display();
  delay(3000);
  
  }
  
  if (setWeight >= 260){
    setWeight = 0;
  }
lastState1 = buttonState1;
lastState2 = buttonState2;
lastState3 = buttonState3;
lastState4 = buttonState4;

display.setTextSize(1); // Draw 2X-scale text
display.setTextColor(SSD1306_WHITE);
display.clearDisplay();
display.setCursor(0,0);
display.print("Spool Weight:");
display.setCursor(80,0);
display.print(setWeight);
display.setCursor(0,10);
display.print("Weight:");
display.setCursor(80,10);
display.print(scale.get_units() + EEPROM.read(b));
display.setCursor(0,20);
display.print("Error:");
display.setCursor(80,20);
display.print(error);
display.display();
delay(100);
} 



void butstate(){

  state = !state;
}
