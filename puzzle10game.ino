#include <Adafruit_CircuitPlayground.h>

int level = 0;
int cursor = 0;
int goal;
int levelDelay = 100;
int points = 0;
bool gameOver = false;
bool playerWon = false;

float midi[127];
int lose[] = {85, 81, 77};
int win[] = {77, 81, 85};
bool sound;

volatile bool buttonFlag = false;
volatile bool switchFlag = false;

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
  generateMIDI();

  pinMode(4, INPUT_PULLDOWN);
  pinMode(5, INPUT_PULLDOWN);
  pinMode(7, INPUT);

  attachInterrupt(digitalPinToInterrupt(4), buttonISR, RISING);
  attachInterrupt(digitalPinToInterrupt(5), buttonISR, RISING);
  attachInterrupt(digitalPinToInterrupt(7), switchISR, CHANGE);

  sound = CircuitPlayground.slideSwitch();
  goal = random(10);
  CircuitPlayground.setPixelColor(goal, 0, 255, 0);
}

void loop() {
  CircuitPlayground.setPixelColor(cursor, 255,255,255);
  if (switchFlag) {
    delay(5);
    sound = CircuitPlayground.slideSwitch();
    switchFlag = false;
  }

  if (buttonFlag) {
    delay(5);
    Serial.println(cursor);
    Serial.println(goal);
    if (cursor == goal) {
      if (sound) CircuitPlayground.playTone(midi[map(level, 0, 9, 60, 100)], 50);
      level++;
      levelDelay -=5;
      playerWon = level==10;
      gameOver = playerWon;
      delay(50);
    } else {
      gameOver = true;
    }
    goal = random(10);
    CircuitPlayground.setPixelColor(goal, 0, 255, 0);
    buttonFlag = false;
  }

  if (gameOver) {
    if (playerWon) {
      if (sound) playSequence(win);
      Serial.println("YOU WIN"); // eventually will print score
      CircuitPlayground.clearPixels();
    } else {
      blink(3);
      if (sound) playSequence(lose);
      Serial.println("GAME OVER... RESETTING LEVELS"); // eventually will print score
    }
    level = 0;
    levelDelay = 100;
    playerWon = false; // reset the game
    gameOver = false;
    cursor = 0;
    goal = random(10);
    CircuitPlayground.setPixelColor(goal, 0, 255, 0);
  } else {
    delay(levelDelay);
    CircuitPlayground.setPixelColor(cursor, 0, cursor==goal ? 255 : 0, 0); //if cursor is on top of goal, set the led back to green, otherwise turn it off
    cursor+1 > 9 ? cursor = 0 : cursor ++; // increments cursor with wraparound effect
  }
}

void playSequence(int arr[]) {
  CircuitPlayground.playTone(midi[arr[0]], 50);
  CircuitPlayground.playTone(midi[arr[1]], 50);
  CircuitPlayground.playTone(midi[arr[2]], 150);
}

void blink(int reps) {
  for (int i = 0; i < reps; i++) {
    for (int j = 0; j < 10; j++) {
      CircuitPlayground.setPixelColor(j,255,0,0);
    }
    delay(75);
    CircuitPlayground.clearPixels();
    delay(75);
  }
}

void buttonISR() {
  buttonFlag = true;
}

void switchISR() {
  switchFlag = true;
}

void generateMIDI()
{
  for (int x = 0; x < 127; ++x) {
    midi[x] = (13.75) * pow(2.0, ((x - 9.0) / 12.0));
    Serial.println(midi[x]);
  }
}
