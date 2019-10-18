/*
  Input: Through rotary encoder (RE) in 3 ways 
  1. rotating (21 positions)
  2. quick press
  3. long press ( > 1.2 seconds)

  Output: Serial Data to MAX MCP
  1. rotating range [0,21], outside of the range (200)
  2. quick press to toggle on/off the game in MAX (100)
  3. long press to trigger the Easter Egg (150): a 5 seconds bullet time (road increases at a slower speed)
  
  References:
  This code is appropriate from 
  1. Arduino official debouncing code http://www.arduino.cc/en/Tutorial/Debounce
  2. rotaryEncoder, Michael Gurevich, Oct 8, 2013
*/

// ROTATING
const int pinA = 2;          // Rotary Encoder (RE) rotating pin
const int pinB = 3;          // RE rotating pin

int encoderPos = 0;          // encoder position
int lastPos = 0;             // keep track of last position   
int stateA=LOW;
int stateB=LOW;
int car_range = 21;          // number of positions for the rotary encoder

// PUSHING
const int buttonPin = 4;             // Rotary Encoder (RE) button pin
int buttonState;                     // the current reading from the input pin
int lastButtonState = LOW;           // the previous reading from the input pin

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 10;    // the debounce time; increase if the output flickers
unsigned long lastPressTime = 0;     // the last time the long push was detected
unsigned long pressDelay = 1200;     // the push debounce time; increase if the button is pushed longer than pressDelay

bool pressed = false;
bool eggShown = false;               // controls the Easter Egg from long press        

void setup() {
  Serial.begin(115200);
  pinMode(pinA, INPUT_PULLUP);   //set encoder pins to inputs with pullups
  pinMode(pinB, INPUT_PULLUP); 
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
   encoderStep();
   displayStep();
   delay(1);
   
  // detect the change
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  // confirm it's a real press
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // when the press is real, detect if it's a long press
    if(pressed && millis() - lastPressTime > pressDelay && !eggShown){
      Serial.write(150); // output the Easter Egg to MAX
      eggShown = true;
    }

    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        pressed = true;
        lastPressTime = millis();
        Serial.write(100); // toggle on/off in MAX 
      }
      else{
        pressed = false;
        eggShown = false;
      }
    }
  }
  lastButtonState = reading;
}

void encoderStep(){

   int readingA,readingB;
   readingA = digitalRead(pinA);
   readingB = digitalRead(pinB);
   if (stateA!=readingA) {        //state has changed->tick
     if (readingA==readingB)  {   //CW
          ++encoderPos;
          if (encoderPos > car_range) {
            encoderPos = car_range;
            Serial.write (200);
            }
     }
      else {
          --encoderPos;  
          if (encoderPos < 0) {
            encoderPos = 0;
            Serial.write (200);
            }
          }             //CCW
    }
   stateA = readingA;   //update state to current reading
   stateB = readingB;
    
} 
  
void displayStep() {
  if (encoderPos != lastPos) {    //position has changed
    Serial.write(encoderPos);     //send new position
  }
  lastPos=encoderPos;             //store current position
}
