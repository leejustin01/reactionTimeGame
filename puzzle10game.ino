#include <Adafruit_CircuitPlayground.h>

int level = 0;
int cursor = 0;
int goal;
int levelDelay = 50;

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
  if (switchFlag) {
    delay(5);
    sound = CircuitPlayground.slideSwitch();
    switchFlag = false;
  }
  if (buttonFlag) {
    if (cursor == goal) {
      if (sound) playSequence(win);
      level++;
      levelDelay -= 10;
    } else {
      if (sound) playSequence(lose);
      blink(3);
      Serial.println("GAME OVER... RESETTING LEVELS");
      level = 0;
      cursor = 0;
      levelDelay = 50;
    }
    goal = random(10);
    CircuitPlayground.setPixelColor(goal, 0, 255, 0);
  }
  cursor+1 > 9 ? cursor = 0 : cursor ++; // increments cursor with wraparound effect
  delay(levelDelay);
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
    delay(10);
    CircuitPlayground.clearPixels();
    delay(10);
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
