#include "pitches.h"

// Arduino pin numbers

#define GREEN 8
#define RED 9
#define BLUE 10
#define WHITE 5

// define the LED digit patterns, from 0 - 9
// 1 = LED on, 0 = LED off, in this order:
//                74HC595 pin     Q0,Q1,Q2,Q3,Q4,Q5,Q6,Q7 
//                Mapping to      a,b,c,d,e,f,g of Seven-Segment LED
byte seven_seg_digits[10] = { B11111100,  // = 0
                              B01100000,  // = 1
                              B11011010,  // = 2
                              B11110010,  // = 3
                              B01100110,  // = 4
                              B10110110,  // = 5
                              B10111110,  // = 6
                              B11100000,  // = 7
                              B11111110,  // = 8
                              B11100110   // = 9
                             };

//LED Segment display pins:
int latchPin = 13;
int clockPin = 12;
int dataPin = 11;

int startButton = 3;
int winnerLight = 6;
int loserLight = 4;

const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = A0; // analog pin connected to X output
const int Y_pin = A1; // analog pin connected to Y output

//List of notes that can be outputted by the passive buzzer.
int melody[] = {NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6};
int duration = 500; //500 ms per note.

byte userScore = 0;
byte* x = &userScore; //To keep a reference to userScore in simonSays function.

void setup() {
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(WHITE, OUTPUT);
  pinMode(winnerLight,OUTPUT);
  pinMode(loserLight,OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(SW_pin, INPUT);
  pinMode(startButton, INPUT_PULLUP);
  digitalWrite(SW_pin, HIGH);
  digitalWrite(GREEN, LOW);
  digitalWrite(RED, LOW);
  digitalWrite(BLUE, LOW);
  digitalWrite(WHITE, LOW);
  digitalWrite(winnerLight,LOW);
  digitalWrite(loserLight,LOW);
  Serial.begin(9600);

  sevenSegWrite(*x);
}

// display a number on the digital segment display
void sevenSegWrite(byte digit) {
  // set the latchPin to low potential, before sending data
  digitalWrite(latchPin, LOW);
     
  // the original data (bit pattern)
  shiftOut(dataPin, clockPin, LSBFIRST, seven_seg_digits[digit]);  
 
  // set the latchPin to high potential, after sending data
  digitalWrite(latchPin, HIGH);
}

void playDefeat() {

  digitalWrite(loserLight, HIGH);
  delay(150);
  digitalWrite(loserLight, LOW);
  delay(150);
  digitalWrite(loserLight, HIGH);
  delay(150);
  digitalWrite(loserLight, LOW);
  delay(150);
  digitalWrite(loserLight, HIGH);
  delay(150);
  digitalWrite(loserLight, LOW);
  
}

void playVictory() {

  digitalWrite(winnerLight, HIGH);
  delay(150);
  digitalWrite(winnerLight, LOW);
  delay(150);
  digitalWrite(winnerLight, HIGH);
  delay(150);
  digitalWrite(winnerLight, LOW);
  delay(150);
  digitalWrite(winnerLight, HIGH);
  delay(150);
  digitalWrite(winnerLight, LOW);
  
}

void playGreen() {

  digitalWrite(GREEN, HIGH);
  tone(7, melody[0], duration/2);
  delay(250);
  digitalWrite(GREEN, LOW);
  
}

void playRed() {

  digitalWrite(RED, HIGH);
  tone(7, melody[1], duration/2);
  delay(250);
  digitalWrite(RED, LOW);
  
}

void playBlue() {

  digitalWrite(BLUE, HIGH);
  tone(7, melody[2], duration/2);
  delay(250);
  digitalWrite(BLUE, LOW);
  
}

void playWhite() {

  digitalWrite(WHITE, HIGH);
  tone(7, melody[3], duration/2);

  delay(250);
  digitalWrite(WHITE, LOW);
  
}

void simonSays(byte* x) {
                                                                                
  //Make an empty array for the random notes to be stored.
  //Store those random values in the array.
  int randArray[4] = {(int)random(0,4), (int)random(0,4), (int)random(0,4), (int)random(0,4)};

  Serial.println("Simon's Array:");
  for (int i = 0; i < 4; i++) {
    Serial.println(randArray[i]);
  }
  Serial.println();

  int elements = sizeof(randArray) / sizeof(randArray[0]);

  for (int i = 0; i < elements; i++) {

    if (randArray[i] == 0) {

      playGreen();
      delay(500);
      
    }
    else if (randArray[i] == 1) {

      playRed();
      delay(500);
      
    }
    else if (randArray[i] == 2) {

      playBlue();
      delay(500);
      
    }
    else {

      playWhite();
      delay(500);
  
    }

    
  }

 
  //Record the user input by mapping joystick actions to hash values.
  //Run a for loop to see if the user array and random array match.
  int userNoteCount = 0;
  int userArray[4];
  
  while (userNoteCount < 4) {

    if (analogRead(Y_pin) > 1000) {

      playGreen();
      userArray[userNoteCount] = 0;
      ++userNoteCount;
    
    } 

    else if (analogRead(Y_pin) < 10) {
  
      playWhite();
      userArray[userNoteCount] = 3;
      ++userNoteCount;
      
    } 
  
    else if (analogRead(X_pin) > 1000) {
  
      playRed();
      userArray[userNoteCount] = 1;
      ++userNoteCount;
      
    } 
  
    else if (analogRead(X_pin) < 10) {
  
      playBlue();
      userArray[userNoteCount] = 2;
      ++userNoteCount;
      
    } 

    
  } //end while

  Serial.println("User's Array:");
  for (int i = 0; i < 4; i++) {
    Serial.println(userArray[i]);
  }
  Serial.println();

  //If they match, the user wins, if not, Simon wins.
  bool winState = true;
  for (int j = 0; j < 4; j++) {

    if (randArray[j] != userArray[j]) {
      winState = false;
      break;
    }

  }

  if (winState) {

    Serial.println("Winner Winner Chicken Dinner");
    (*x)++;

    if ((*x) > 9) (*x) = 0;
    
    playVictory();
    
  } else {


    Serial.println("Loser");
    (*x)--;

    if ((*x) < 0) (*x) = 0;
    
    playDefeat();
    
  }
  
  
}


void loop() {

  /*
  for (byte digit = 10; digit > 0; --digit) {
    delay(1000);
    sevenSegWrite(digit - 1); 
  } */
   
  // suspend 4 seconds
  delay(3000);

  //If the button is pressed, play Green note.
  if (digitalRead(startButton) == LOW) {

    simonSays(x);
    sevenSegWrite(*x);
    
  }
  
  //If the Joystick direction is pos Y (analogRead(Y_pin) > 1000), set GREEN pin to High.
  

    //Printing Joystick details to console
    /*
    Serial.print("Switch:  ");
    Serial.print(digitalRead(SW_pin));
    Serial.print("\n");
    Serial.print("X-axis: ");
    Serial.print(analogRead(X_pin));
    Serial.print("\n");
    Serial.print("Y-axis: ");
    Serial.println(analogRead(Y_pin));
    Serial.print("\n\n");
    */
    delay(500);
    
}
