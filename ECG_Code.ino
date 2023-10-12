#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows//importing library for lcddisplay

void setup() {
  Serial.begin(9600); //baud rate
  pinMode(10, INPUT); // Setup for leads off detection LO +
  pinMode(11, INPUT); // Setup for leads off detection LO -
  lcd.init(); // initialize the lcd
  lcd.backlight();
}
void loop() {
  readHeartbeat(); // call the readHeartbeat function
  delay(10); // add a small delay to avoid overwhelming the serial monitor
  displayBPM();
  //view this code on the serial monitor
  if((digitalRead(10) == 1)||(digitalRead(11) == 1)) {
    Serial.println('!');
  }
  else {
    Serial.print(peakValue);
    Serial.print(' ');
     Serial.println(analogRead(A0));
  }
  delay(10);
}
//writing the function to read the signal from the serial port and start calculations
const int thresholdValue = 400; // set the threshold value for peak detection
int data = 0; //variable to store value from serial plotter
int plotterValue = 0; // variable to store the incoming data
int peakValue = 3000; // variable to store the peak value
int startTime = 0; //variable to store start time
int timeBetween = 0; //variable to get elapsed time
float currentBPM = 0; //variable to store BPM, float since dividing
float newBPM = 0; //variable to store new BPM, used to check if increase in BPM
bool alert = false; //boolean to set alert to false when not indicated
String bpmString; //declaring bpmString as string variable
void readHeartbeat() {
  if (Serial.available()) {
    data = Serial.read(); // read incoming data from serial plotter
    plotterValue = data; //initializing the plotter value to be the data from the serial monitor
    if (plotterValue > peakValue) { // check if incoming data is higher than current peak value
      peakValue = plotterValue; // update peak value
      startTime = millis(); // update start time
    }
    if (peakValue > thresholdValue) { // check if peak value has exceeded threshold value
      timeBetween = millis() - startTime;
      Serial.println("R peak! The time this took is: "); // print message to serial monitor
      Serial.print(timeBetween); //printing the elapsed time, aka the time between two R peaks
      Serial.println("milliseconds"); //finishing the line in serial monitor that shows the unit of time
      currentBPM = 60000 / timeBetween; // convert to beats per minute
      Serial.println("The beats per minute is: ");
      //Serial.print(currentBPM); //displaying the beats per minute
      displayBPM(); //to display BPM on the LCD
      //peakValue = 0; // reset peak value
    }
    if ((newBPM - currentBPM) >= 10) {//checks if bpm has increased by the threshold value of 5
    alert = true; //alert will go off
    }
    newBPM=currentBPM; //updating BPM value
  }
  if (alert) { //checking if alert is reached
    displayAlertMessage(); //calling the function to display the alert on LCD
    alert = false; //
  }
}
void displayBPM(){ //function to display BPM on LCD
  bpmString = String(currentBPM);
  lcd.clear(); // clears display
  lcd.setCursor(0, 0); // move cursor to   (0, 0)
  lcd.print("BPM is : ");
  lcd.print(bpmString);
}

void displayAlertMessage(){
  lcd.clear(); // clear display
  lcd.setCursor(1, 0); // move cursor to (1,0)
  lcd.print("Heart rate increase!");
  delay(2000);
  lcd.setCursor(0, 0); // move cursor back to (0,0)
  lcd.print(bpmString);
  lcd.clear(); //clears display
}