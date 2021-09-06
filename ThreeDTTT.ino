// This sketch is for the Subsystems 3D TTT game. It was compiled
// with Adafruit_NeoPixel version 1.8.2.
// Compile for Arduino UNO if downloading to the Subsystems game unit
//

#define PIX_COUNT 27 // Max number of Neo Pixels
#define PIX_PIN    6 // Pin that communicates with NeoPixels
#include <Adafruit_NeoPixel.h>

#define but_Place  2
#define but_Z  3
#define but_X  4
#define but_Y  5

Adafruit_NeoPixel strip(PIX_COUNT, PIX_PIN, NEO_GRB + NEO_KHZ800);

char theMatrix[27]; //Holds the current status of all Pixels
// 1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9
// First level          Second level      Third level
// value is 0 for nothing, 1 for green, 2 for red

boolean whosTurn = false; // false = green, true = red

void setup() {
  pinMode(but_Place, INPUT_PULLUP);
  pinMode(but_X, INPUT_PULLUP);
  pinMode(but_Y, INPUT_PULLUP);
  pinMode(but_Z, INPUT_PULLUP);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(40); // Set BRIGHTNESS to about 1/5 (max = 255)
  clearMatrix();
}

void loop() {
  // check to see if there is a winner
  char letsSee = checkWin();
  if (letsSee) winAnim(letsSee);

  // get next move
  getPlayerMove(whosTurn);
  whosTurn = !whosTurn;
}

void updateMatrix(char ux, char uy, char uz, char uValue) {
  char theIndex = uz * 9 + uy * 3 + ux;
  theMatrix[theIndex] = uValue;
}

char getMatrix(char gx, char gy, char gz) {
  char theIndex = gz * 9 + gy * 3 + gx;
  return theMatrix[theIndex];
}

void clearMatrix() {
  for (int i = 0; i < 27; i++) theMatrix[i] = 0;
}

char decodeM(char cx, char cy, char cz) {
  return cx + cy * 3 + cz * 9;
}

void displayMatrix() {
  strip.clear();
  for (int i = 0; i < 27; i++) {
    if (theMatrix[i] == 1)   strip.setPixelColor(i, strip.Color(  0, 255, 0));
    if (theMatrix[i] == 2)   strip.setPixelColor(i, strip.Color(  255, 0, 0));
  }
  strip.show();
}

void getPlayerMove(boolean player) {
  char mx = 0;
  char my = 0;
  char mz = 0;
  // Holds the values of color for the particular event (moving, placing)
  char cRed = 0;
  char cGreen = 255;
  boolean validPlace = false;
  char but_push;

  if (player) {
    cRed = 255;
    cGreen = 0;
  }

  // Respond to move or place command
  while (!validPlace) {

    // Display current Matrix status
    strip.clear();
    displayMatrix();
    // Add current cursor position
    strip.setPixelColor(decodeM(mx, my, mz), strip.Color(  cRed, cGreen, 100));
    strip.show();

    but_push = btnPress(); // The function waits for a valid key to return

    switch (but_push) {
      case 1:  // X was pressed
        mx++;
        if (mx == 3) mx = 0;
        break;
      case 2:  // Y was pressed
        my++;
        if (my == 3) my = 0;
        break;
      case 3:  // Z was pressed
        mz++;
        if (mz == 3) mz = 0;
        break;
      case 4:  // Place
        // Is it a blank?
        if (getMatrix(mx, my, mz) == 0) {
          // Set the player color
          strip.setPixelColor(decodeM(mx, my, mz), strip.Color(  cRed, cGreen, 0));
          if (whosTurn) updateMatrix(mx, my, mz, 2);
          else updateMatrix(mx, my, mz, 1);
          validPlace = true;
        }
    }
  }
  displayMatrix();
}

char btnPress() {
  // Waits for a valid key to return

  while (true) {
    // Return X, Y, Z, PLACE -> 1, 2, 3, 4
    if (!digitalRead(but_X)) {
      delay(10); // debounce the switch
      while (!digitalRead(but_X));  // Wait for the button to be released
      return 1;
    }
    if (!digitalRead(but_Y)) {
      delay(10); // debounce the switch
      while (!digitalRead(but_Y));  // Wait for the button to be released
      return 2;
    }
    if (!digitalRead(but_Z)) {
      delay(10); // debounce the switch
      while (!digitalRead(but_Z));  // Wait for the button to be released
      return 3;
    }
    if (!digitalRead(but_Place)) {
      delay(10); // debounce the switch
      while (!digitalRead(but_Place));  // Wait for the button to be released
      return 4;
    }
  }
}

char checkWin() {
  // Checks all possible wins. Returns the player number if there is a win, zero otherwise

  // Check for the straight line wins on each level
  for (int j = 0; j < 3; j++) { // Tracks the z level
    for (int i = 0; i < 3; i++) { // Runs through the x and y conditions
      if (getMatrix(i, 0, j) != 0 && getMatrix(i, 0, j) == getMatrix(i, 1, j) && getMatrix(i, 1, j) == getMatrix(i, 2, j)) return getMatrix(i, 0, j);
      if (getMatrix(0, i, j) != 0 && getMatrix(0, i, j) == getMatrix(1, i, j) && getMatrix(1, i, j) == getMatrix(2, i, j)) return getMatrix(0, i, j);
    }
  }
  // Now check the diagnal wins on a given level
  for (int j = 0; j < 3; j++) {
    if (getMatrix(0, 0, j) != 0 && getMatrix(0, 0, j) == getMatrix(1, 1, j) && getMatrix(1, 1, j) == getMatrix(2, 2, j)) return getMatrix(0, 0, j);
    if (getMatrix(2, 0, j) != 0 && getMatrix(2, 0, j) == getMatrix(1, 1, j) && getMatrix(1, 1, j) == getMatrix(0, 2, j)) return getMatrix(2, 0, j);
  }

  // Now check straight line wins across levels
  for (int j = 0; j < 3; j++) { // Tracks the x values
    for (int i = 0; i < 3; i++) { // Tracks the y values
      if (getMatrix(j, i, 0) != 0 && getMatrix(j, i, 0) == getMatrix(j, i, 1) && getMatrix(j, i, 1) == getMatrix(j, i, 2)) return getMatrix(j, i, 0);
    }
  }
  // The 3D diagnals on the planes
  for (int j = 0; j < 3; j++) { // Tracks the x or y values
    if (getMatrix(j, 0, 0) != 0 && getMatrix(j, 0, 0) == getMatrix(j, 1, 1) && getMatrix(j, 1, 1) == getMatrix(j, 2, 2)) return getMatrix(j, 0, 0);
    if (getMatrix(j, 0, 2) != 0 && getMatrix(j, 0, 2) == getMatrix(j, 1, 1) && getMatrix(j, 1, 1) == getMatrix(j, 2, 0)) return getMatrix(j, 0, 2);
    if (getMatrix(0, j, 0) != 0 && getMatrix(0, j, 0) == getMatrix(1, j, 1) && getMatrix(1, j, 1) == getMatrix(2, j, 2)) return getMatrix(0, j, 0);
    if (getMatrix(2, j, 0) != 0 && getMatrix(2, j, 0) == getMatrix(1, j, 1) && getMatrix(1, j, 1) == getMatrix(0, j, 2)) return getMatrix(2, j, 0);
  }
  // Finally, the 4 wacky diagnals (across all planes)
  if (getMatrix(0, 0, 0) != 0 && getMatrix(0, 0, 0) == getMatrix(1, 1, 1) && getMatrix(1, 1, 1) == getMatrix(2, 2, 2)) return getMatrix(0, 0, 0);
  if (getMatrix(0, 0, 2) != 0 && getMatrix(0, 0, 2) == getMatrix(1, 1, 1) && getMatrix(1, 1, 1) == getMatrix(2, 2, 0)) return getMatrix(0, 0, 2);
  if (getMatrix(2, 0, 0) != 0 && getMatrix(2, 0, 0) == getMatrix(1, 1, 1) && getMatrix(1, 1, 1) == getMatrix(0, 2, 2)) return getMatrix(2, 0, 0);
  if (getMatrix(0, 2, 0) != 0 && getMatrix(0, 2, 0) == getMatrix(1, 1, 1) && getMatrix(1, 1, 1) == getMatrix(2, 0, 2)) return getMatrix(0, 2, 0);

  // If you got here, you have no winner yet
  return 0;
}

void winAnim(char winner) {
  // Show a short annimation ending in the winning player's color being displayed
  strip.clear();
  strip.show();
  char cRed = 255;
  char cGreen = 0;
  if (winner == 1) {
    cRed = 0;
    cGreen = 255;
  }
  for (int i = 0; i < 9; i++) strip.setPixelColor(i, strip.Color(random(256), random(256), random(256)));
  strip.show();
  delay(1000);
  strip.clear();
  strip.show();
  for (int i = 9; i < 18; i++) strip.setPixelColor(i, strip.Color(random(256), random(256), random(256)));
  strip.show();
  delay(1000);
  while (true) {
    strip.clear();
    strip.show();
    delay(1000);
    for (int i = 18; i < 27; i++) strip.setPixelColor(i, strip.Color(cRed, cGreen, 0));
    strip.show();
    delay(1000);
  }
}
