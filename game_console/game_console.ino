#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
// 0 - dino highscore

#define joyYPin A3
#define joyXPin A2
#define joySwPin 2

int joyY = 0;
int joyX = 0;
bool joySw = false;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void wait(int time) {
  joyY = 0;
  joyX = 0;
  joySw = false;

  unsigned long start = millis();
  while (true) {

    int joyYNew = analogRead(joyYPin);
    joyYNew = map(joyYNew, 0, 1024, -1, 2);
    if (joyYNew != 0) joyY = joyYNew;

    int joyXNew = analogRead(joyXPin);
    joyXNew = map(joyXNew, 0, 1024, -1, 2);
    if (joyXNew != 0) joyX = joyXNew;

    if (digitalRead(joySwPin) == 0) joySw = true;

    if (millis() - start >= time) break;
  }
}

struct DinoGame {
  int dinoY, dinoTime, score, highScore, cacti[3];
  bool run;

  byte dino[8] = {
    0b00000,
    0b11100,
    0b10111,
    0b11111,
    0b01100,
    0b01110,
    0b10100,
    0b10100,
  };

  byte cactus[8] = {
    0b00000,
    0b00100,
    0b10100,
    0b10101,
    0b11101,
    0b00111,
    0b00100,
    0b00100,
  };

  void setup() {
    dinoY = 1;
    dinoTime = 0;
    score = 0;
    EEPROM.get(0, highScore);
    run = true;
    cacti[0] = cacti[1] = cacti[2] = -1;
    lcd.createChar(0, dino);
    lcd.createChar(1, cactus);
    randomSeed(analogRead(0));
  }

  void loop() {
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("                ");

    if (!run) {
      EEPROM.get(0, highScore);
      if (highScore < score) {
        EEPROM.put(0, score);
        highScore = score;
      }

      lcd.setCursor(3, 0);
      lcd.print("Game Over!");
      lcd.setCursor(1, 1);
      lcd.print("Sc:");
      lcd.print(score);
      lcd.setCursor(8, 1);
      lcd.print("HI:");
      lcd.print(highScore);
      while (!joySw) {
        wait(100);
      }
      setup();
      return;
    }

    lcd.setCursor(6, 0);
    lcd.print("Score:");
    lcd.print(score);

    if (joyY == 1 && dinoY == 1) {
      dinoY = 0;
      dinoTime = 3;
    }
    if (joyY == -1) {
      dinoY = 1;
      dinoTime = 0;
    }

    if (dinoTime <= 0) dinoY = 1;
    else dinoTime--;

    lcd.setCursor(1, dinoY);
    lcd.write(0);

    for (int i = 0; i < 3; i++) {
      cacti[i]--;
      if (cacti[i] < 0) {
        cacti[i] = random(16, 31);
      }
      lcd.setCursor(cacti[i], 1);
      lcd.write(1);

      if (cacti[i] == 1 && dinoY == 1) {
        run = false;
        delay(1000);
        return;
      }
    }

    wait(200);
    score += 1;
  }
};


void (*gameLoop)()  = nullptr;

DinoGame dino;

void startDino() {
  gameLoop = []{ dino.loop(); };
  dino.setup();
}

void setup() {
  lcd.init();
  lcd.backlight();

  pinMode(joyYPin, INPUT);
  pinMode(joyXPin, INPUT);
  pinMode(joySwPin, INPUT_PULLUP);

  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);

  Serial.begin(9600);

  startDino();
}

void loop() {
  gameLoop();
}
