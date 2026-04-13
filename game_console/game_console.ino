#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
// #include <EEPROM.h>
// 0 - dino highscore

#define joyYPin 32
#define joyXPin 33
#define joySwPin 25

int joyY = 0;
int joyX = 0;
bool joySw = false;

const String emptyLine = "                    ";

LiquidCrystal_I2C lcd(0x27, 20, 4);

void wait(int time) {
  joyY = 0;
  joyX = 0;
  joySw = false;

  unsigned long start = millis();
  while (true) {

    int joyYNew = analogRead(joyYPin);
    joyYNew = map(joyYNew, 0, 4095, -1, 2);
    if (joyYNew != 0) joyY = joyYNew;

    int joyXNew = analogRead(joyXPin);
    joyXNew = map(joyXNew, 0, 4095, -1, 2);
    if (joyXNew != 0) joyX = joyXNew;

    if (digitalRead(joySwPin) == 0) joySw = true;

    if (millis() - start >= time) break;

    delay(3);
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
    dinoY = 2;
    dinoTime = 0;
    score = 0;
    // EEPROM.get(0, highScore);
    run = true;
    cacti[0] = cacti[1] = cacti[2] = -1;
    lcd.createChar(0, dino);
    lcd.createChar(1, cactus);
    randomSeed(analogRead(0));
    lcd.clear();
  }

  void loop() {
    lcd.setCursor(0, 0);
    lcd.print(emptyLine);
    lcd.setCursor(0, 1);
    lcd.print(emptyLine);
    lcd.setCursor(0, 2);
    lcd.print(emptyLine);

    if (!run) {
      // EEPROM.get(0, highScore);
      // if (highScore < score) {
      //   EEPROM.put(0, score);
      //   highScore = score;
      // }

      lcd.setCursor(5, 1);
      lcd.print("Game Over!");
      lcd.setCursor(3, 2);
      lcd.print("Sc:");
      lcd.print(score);
      lcd.setCursor(10, 2);
      // lcd.print("HI:");
      // lcd.print(highScore);
      while (!joySw) {
        wait(100);
      }
      setup();
      return;
    }

    lcd.setCursor(10, 0);
    lcd.print("Score:");
    lcd.print(score);

    if (joyY == 1 && dinoY == 2) {
      dinoY = 1;
      dinoTime = 3;
    }
    if (joyY == -1) {
      dinoY = 2;
      dinoTime = 0;
    }

    if (dinoTime <= 0) dinoY = 2;
    else dinoTime--;

    lcd.setCursor(2, dinoY);
    lcd.write(0);

    for (int i = 0; i < 3; i++) {
      cacti[i]--;
      if (cacti[i] < 0) {
        cacti[i] = random(20, 40);
      }
      if (cacti[i] < 20) {
        lcd.setCursor(cacti[i], 2);
        lcd.write(1);
      }

      if (cacti[i] == 2 && dinoY == 2) {
        run = false;
        delay(1000);
        return;
      }
    }

    wait(200);
    score += 1;
  }
};


struct SnakeGame {
  int snake[20*4][2], snakeLen, head, snakeX, snakeY, dirX, dirY, appleX, appleY;

  bool run, gameOver, gameWon;

  byte snakeV[8] = {
    0b01110,
    0b01110,
    0b01110,
    0b01110,
    0b01110,
    0b01110,
    0b01110,
    0b01110,
  };
  
  byte snakeH[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b11111,
    0b11111,
    0b11111,
    0b00000,
    0b00000,
  };

  byte snakeTopLeft[8] = {
    0b01110,
    0b01110,
    0b01110,
    0b11110,
    0b11110,
    0b11110,
    0b00000,
    0b00000,
  };

  byte snakeTopRight [8] = {
    0b01110,
    0b01110,
    0b01110,
    0b01111,
    0b01111,
    0b01111,
    0b00000,
    0b00000,
  };

  byte snakeBottomLeft [8] = {
    0b00000,
    0b00000,
    0b00000,
    0b11110,
    0b11110,
    0b11110,
    0b01110,
    0b01110,
  };

  byte snakeBottomRight [8] = {
    0b00000,
    0b00000,
    0b00000,
    0b01111,
    0b01111,
    0b01111,
    0b01110,
    0b01110,
  };

  byte snakeDot[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b01110,
    0b01110,
    0b01110,
    0b00000,
    0b00000,
  };

  byte apple[8] = {
    0b00000,
    0b01010,
    0b00100,
    0b01110,
    0b01110,
    0b01110,
    0b00000,
    0b00000,
  };

  void setApple(int &x, int &y) {
    appleX = random(0, 20);
    appleY = random(0, 4);

    lcd.setCursor(appleX, appleY);
    lcd.write(7);
  }

  void incHead(int &head) {
    head = (head+1) % 80;
  }

  void drawSnake() {
    int currentX = snakeX;
    int currentY = snakeY;

    int prevX = 0;
    int prevY = 0;
    for (int offset = 0; offset < snakeLen; offset++) {
      int i = (head - offset + 80) % 80;

      if (offset != 0) {
        if (currentX == snakeX && currentY == snakeY) {
          run = false;
          delay(1000);
          return;
        }
      }

      int currentMove[2] = {snake[i][0], snake[i][1]};

      int diffX = prevX + currentMove[0];
      int diffY = prevY + currentMove[1];
      prevX = currentMove[0];
      prevY = currentMove[1];

      int snakeChar;

      if (abs(diffY) == 2) {
        snakeChar = 0;
      }
      else if (abs(diffX) == 2) {
        snakeChar = 1;
      }
      else if (diffX == -1 && diffY == -1) {
        snakeChar = 2;
      }
      else if (diffX == 1 && diffY == -1) {
        snakeChar = 3;
      }
      else if (diffX == -1 && diffY == 1) {
        snakeChar = 4;
      }
      else if (diffX == 1 && diffY == 1) {
        snakeChar = 5;
      }
      else if (diffX == 0 && abs(diffY) == 1) {
        snakeChar = 0;
      }
      else if (abs(diffX) == 1 && diffY == 0) {
        snakeChar = 1;
      }

      lcd.setCursor(currentX, currentY);
      lcd.write(snakeChar);
      currentX += currentMove[0];
      currentY += currentMove[1];
    }

    int diffX = snake[(head-snakeLen+81)%80][0];
    int diffY = snake[(head-snakeLen+81)%80][1];
    int snakeChar;

    if (abs(diffY) == 1) {
      snakeChar = 0;
    }
    else if (abs(diffX) == 1) {
      snakeChar = 1;
    }
    lcd.setCursor(currentX, currentY);
    lcd.write(snakeChar);
  }

  void setup() {
    lcd.createChar(0, snakeV);
    lcd.createChar(1, snakeH);
    lcd.createChar(2, snakeTopLeft);
    lcd.createChar(3, snakeTopRight);
    lcd.createChar(4, snakeBottomLeft);
    lcd.createChar(5, snakeBottomRight);
    lcd.createChar(6, snakeDot);
    lcd.createChar(7, apple);
    
    randomSeed(analogRead(0));

    snakeX = 9;
    snakeY = 1;
    dirX = 0;
    dirY = 0;

    snakeLen = 1;
    head = 0;

    appleX = 0;
    appleY = 0;
    setApple(appleX, appleY);

    run = false;
    gameOver = false;
    gameWon = false;

    lcd.clear();

    lcd.setCursor(snakeX, snakeY);
    lcd.write(6);

    joyX = 0;
    joyY = 0;
    while (joyX == 0 && joyY == 0) {
      wait(10);
    }
    snake[head][0] = -joyX;
    snake[head][1] = -joyY;
    snakeX -= joyX;
    snakeY -= joyY;
    run = true;
  }

  void loop() {
    lcd.setCursor(0, 0);
    lcd.print(emptyLine);
    lcd.setCursor(0, 1);
    lcd.print(emptyLine);
    lcd.setCursor(0, 2);
    lcd.print(emptyLine);
    lcd.setCursor(0, 3);
    lcd.print(emptyLine);

    if (!run) {
      if (gameOver) {
        lcd.setCursor(5, 1);
        lcd.print("Game Over!");
        lcd.setCursor(3, 2);
        lcd.print("Sc:");
        lcd.print(snakeLen);
        lcd.setCursor(10, 2);
        lcd.print("HI:");
        // lcd.print(highScore);
        while (!joySw) {
          wait(100);
        }
        setup();
        return;
      }
      else if (gameWon) {
        lcd.setCursor(6, 1);
        lcd.print("You Won!");
        lcd.setCursor(3, 2);
        lcd.print("Sc:");
        lcd.print(snakeLen);
        lcd.setCursor(10, 2);
        lcd.print("HI:");
        // lcd.print(highScore);
        while (!joySw) {
          wait(100);
        }
        setup();
        return;
      }
      return;
    }

    snakeX -= joyX;
    snakeY -= joyY;
    
    incHead(head);
    snake[head][0] = -joyX;
    snake[head][1] = -joyY;

    if (snakeX == appleX && snakeY == appleY) {
      snakeLen += 1;
      setApple(appleX, appleY);
    }

    drawSnake();

    if (snakeLen == 80) {
      gameWon = true;
      delay(2000);
      return;
    }

    wait(300);
  }
};

void (*gameLoop)()  = nullptr;

DinoGame dino;
SnakeGame snake;

void startDino() {
  gameLoop = []{ dino.loop(); };
  dino.setup();
}

void startSnake() {
  gameLoop = []{ snake.loop(); };
  snake.setup();
}

void setup() {
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  pinMode(joyYPin, INPUT);
  pinMode(joyXPin, INPUT);
  pinMode(joySwPin, INPUT_PULLUP);

  Serial.begin(9600);

  startSnake();
}

void loop() {
  gameLoop();
}
