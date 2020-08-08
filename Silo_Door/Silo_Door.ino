
// Define Arduino Pins
const int pinButton = 8;
const int pinLockButton = 11;
const int pinLockLED = 12;
const int pinOpenActuator = 3;
const int pinCloseActuator = 2;
const int pinOpenEndstop = 6;
const int pinClosedEndstop = 7;

// Define timers
unsigned long currentTimeButton = 0;
unsigned long previousTimeButton = 0;
unsigned long currentTimeLockButton = 0;
unsigned long previousTimeLockButton = 0;

// Define variables
boolean stateButton = false;           // State of the open/close button
boolean stateOpenEndstop = false;      // State of the "fully open" endstop
boolean stateClosedEndstop = false;    // State of the "fully closed" endstop
boolean stateOpening = false;          // Door is currently opening
boolean stateClosing = false;          // Door is currently closing
boolean stateWasOpening = false;       // Door was previously opening
boolean stateWasClosing = false;       // Door was previously closing
int buttonDelay = 500;                 // Button debounce variable

void setup() {
  
  // Define pin modes
  pinMode(pinButton, INPUT);
  pinMode(pinLockButton, INPUT);
  pinMode(pinLockLED, OUTPUT);
  pinMode(pinOpenActuator, OUTPUT);
  pinMode(pinCloseActuator, OUTPUT);
  pinMode(pinOpenEndstop, INPUT);
  pinMode(pinClosedEndstop, INPUT); 

  // Test LED's
  digitalWrite(pinOpenActuator, HIGH);
  delay(200);
  
  digitalWrite(pinCloseActuator, HIGH);
  delay(200);

  digitalWrite(pinOpenActuator, LOW);
  delay(200);
  
  digitalWrite(pinCloseActuator, LOW);
  delay(200);  

  // Move the door to the open position to start the logic
  stateOpening = true;
  digitalWrite(pinOpenActuator, HIGH);
  
}

void loop() {
  
  // Update timers
  unsigned long currentTimeButton = millis();
  
  // Detect buttons and endstop states
  stateClosedEndstop = digitalRead(pinClosedEndstop);
  stateOpenEndstop = digitalRead(pinOpenEndstop);
  stateButton = digitalRead(pinButton);

  // Stop the door because it's fully open
  if (stateOpenEndstop == true && stateClosing == false) {      // "stateClosing" protects door from being stuck in this condition
    digitalWrite(pinOpenActuator, LOW);
    digitalWrite(pinCloseActuator, LOW);
    stateOpening = false;                                       // Set a flag, saying that the door has finished opening
    buttonDelay = 300;                                          // Very short button delay to make it responsive
  }

  // Stop the door because it's fully closed
  if (stateClosedEndstop == true && stateOpening == false) {    // "stateOpening" protects door from being stuck in this condition
    digitalWrite(pinCloseActuator, LOW);
    digitalWrite(pinOpenActuator, LOW);
    stateClosing = false;                                       // Set a flag, saying that the door has finished closing
    buttonDelay = 300;                                          // Very short button delay to make it responsive
  }

  // Button is pressed
  if ((stateButton == HIGH) && (currentTimeButton - previousTimeButton >= buttonDelay)) { 
    buttonDelay = 500;
    
    // Door is closing, stop the door
    if (stateClosing == true) {
      digitalWrite(pinCloseActuator, LOW);
      stateClosing = false;
      stateWasClosing = true;
    }

    // Door is opening, stop the door
    else if (stateOpening == true) {
      digitalWrite(pinOpenActuator, LOW);
      stateOpening = false;
      stateWasOpening = true;
    }
    
    // Door is not moving
    else if (stateClosedEndstop == false && stateOpenEndstop == false) {        // Door is somewhere in the middle, based on the endstops
       
       if (stateWasOpening == true) {                                           // If it was opening, close it
        stateWasOpening = false;
        stateOpening = false;
        stateClosing = true;
        digitalWrite(pinCloseActuator, HIGH);
       }
       
       else if (stateWasClosing == true) {                                      // If it's closing, open it
        stateWasClosing = false;
        stateOpening = true;
        stateClosing = false;
        digitalWrite(pinOpenActuator, HIGH);
       }
       
    }
    
    // Door is fully closed, open it
    else if (stateClosedEndstop == true) {
        stateOpening = true;
        stateClosing = false;
        digitalWrite(pinOpenActuator, HIGH);
    }
    
    // Door is fully open, close it
    else if (stateOpenEndstop == true) {
        stateClosing = true;
        stateOpening = false;
        digitalWrite(pinCloseActuator, HIGH);
    }

  // Button was pressed, so reset the timer to avoid button bounces
  previousTimeButton = currentTimeButton;
  
  }

}
