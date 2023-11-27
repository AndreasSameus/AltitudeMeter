/* File: AltitudeMeter.ino
 * Author: Andreas Saméus
 * Date: 2023-11-27
 * Description: A simple altitude reading program
 */




#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels 
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //setup for the the display and defines height and width



#define outputA 5 //makes the variable outputA refer to the Arduino digitalPin 5 
#define outputB 6 //makes the variable outputB refer to the Arduino digitalPin 6
#define encoderButton 11 //makes the variable encoderButton refer to the Arduino digitalPin 11


#define SEALEVELPRESSURE_HPA (1013.25) //defines SEALEVELPRESSURE as 1013.25 HPA
Adafruit_BME280 bme; // changes how to refer to the bme module to bme




int humidity; //makes an int as humidity (isnt used but could be if the project was more advanced)
float pressure; // creates a float for pressure which will get a value from a reading later 
int temperature; // creates an int for temperature which will get a value from a reading later 
int numericalValue = 0; // defines the int NumericalValue as 0
int aState; // creates an int for the current aState of the encoder which will get a value from a reading later 
int aLastState; // creates an int for the most recent aState of the encoder which will get a value from a reading later 
float pressureChangePerMeter = 0.11; // defines the float pressureChangePerMeter as 0.11 which refers to the change in HPA
float startPressure; // creates a float for the startPressure which will get the value for the pressure when the arduino starts
float readingPressure; // creates a float for the readingPressure which will get a value for the pressure when the encoderbutton gets pressed 

void setup() {

  Wire.begin(); //starts the I2C bus

  pinMode (outputA, INPUT); // makes the outputA pin as an input pin
  pinMode (outputB, INPUT); // makes the outputB pin as an input pin
  pinMode (encoderButton, INPUT_PULLUP); //makes the encoderButton pin as an input pin and reads the value when the button is pulled up

  Serial.begin(9600);
  aLastState = digitalRead(outputA); //gives the variable aLastState the value of the reading of the outputA pin


  /*
   * Checks so the OLED screen is connected and if the arduino could find it
   */

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();

  bool status;

  /*
   * Checks so the Bme module is connected and if the arduino could find it
   */

  status = bme.begin();
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  startPressure = bme.readPressure() / 100.0F; //gives the float startPressure the first reading the bme module makes
  readingPressure = startPressure; // gives the float readingPressure the value of startpressure which can be changed later 
}

void loop() {
  
  updateBME(); //calls upon the updatBME function


  /* updateOled()
   * calls upon the updateOled function with the different string inputs
   * string 1 Shows the temperature
   * string 2 shows the start height to be calibrated by the encoder
   * string 3 shows the difference in height compared to the readingPressure
   */
  
  updateOled(String(temperature) + char(0xf7),String(((startPressure - pressure)/pressureChangePerMeter)+ numericalValue),String((readingPressure - pressure)/pressureChangePerMeter)); 
  
  
  updateEncoder(); //calls upon the updateEncoder function

}

/* updateBME()
 * Gives the humidity int the bme reading of the humidity
 * Gives the pressure float the bme reading of the pressure
 * Gives the temperature int the bme reading of the temperature
 * 
 * No parameters or returns
 */ 
 
void updateBME() {
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  temperature = bme.readTemperature();  
}

/* updateOLED()
 *  
 *  Writes the different string inputs to the oled
 *  
 *  Parameters
 *  text - the first string
 *  text2 - the second string
 *  text3 - the third string
 *  
 *  No returns
 */


void updateOled(String text, String text2,String text3) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(text);
  display.setCursor(10, 20);
  display.println(text2);
  display.setCursor(10, 40);
  display.println(text3);
  display.display();      // Show initial text
}

/* updateEncoder()
 * 
 * Reads the values of the encoder 
 * changes the int numericalValue depending on the encoder changes
 * Changes the readingpressure if the encoderButton is pushed
 * 
 * No parameters or returns
 * 
 */

void updateEncoder() {
   aState = digitalRead(outputA); //makes the aState the current state of the a pin on the encoder 
   Serial.println(digitalRead(outputA));
  
  if (aState != aLastState) { //checks if the encoder has been turned
    if (digitalRead(outputB) != aState) { //if the outputB is different from the aState then it has been turned clockwise
      numericalValue ++; //increases numerical Value by 1

    } else { //otherwise it has been turned counterclockwise
      numericalValue --; //decreases numerical Value by one

    }

  }
  aLastState = aState; //makes the aLastState the most recent aState 

  int buttonState = digitalRead(encoderButton); // creates the int button state which gets the value of the encoderButton
  
  if (buttonState == LOW) { //if the int buttonState is LOW then the bytton has been pressed
  readingPressure = bme.readPressure() / 100.0F; //changes the readingPressure to the current bme pressure reading
 }

}
