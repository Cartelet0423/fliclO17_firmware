#include <Keyboard.h>
#include <Mouse.h>
#include <ArxContainer.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET -1
Adafruit_SSD1306 display(128, 32, &Wire, OLED_RESET);

#define _Jx A3
#define _Jy A2
#define _Jsw A1
#define len(array) (sizeof(array) / sizeof(array[0]))
#define sign(a) ((a > 0) - (a < 0))
const unsigned short int cols[4] = {9u, 8u, 7u, 6u};
const unsigned short int rows[5] = {18u, 15u, 14u, 16u, 10u};
const char* nums[12] = {"1", "4", "7", "*", "2", "5", "8", "0", "3", "6", "9", "#"};
const char* boin[5] = {"a", "i", "u", "e", "o"};
const char* shiin[11] = {"\0", "t", "m", "", "k", "n", "y", "", "s", "h", "r"};
const String ex[10] = {
  "wa", ",",
  "wo", ".",
  "nn", "?",
  "-", "!",
  "~", "..."
};
arx::map<char*, char*> henkan = {
  {"\0", "l"},
  {"l", "\0"},
  {"v", "\0"},
  {"k", "g"},
  {"g", "k"},
  {"s", "z"},
  {"z", "s"},
  {"t", "d"},
  {"d", "t"},
  {"h", "b"},
  {"b", "p"},
  {"p", "h"}
};

const char* keys_e[12][5] = {
  {"@", "-", "_", "/", "1"},
  {"g", "h", "i", "\0", "4"},
  {"p", "q", "r", "s", "7"},
  {"", "", "", "", ""},
  {"a", "b", "c", "\0", "2"},
  {"j", "k", "l", "\0", "5"},
  {"t", "u", "v", "\0", "8"},
  {"'", "\"", ":", ";", "0"},
  {"d", "e", "f", "\0", "3"},
  {"m", "n", "o", "\0", "6"},
  {"w", "x", "y", "z", "9"},
  {".", ",", "?", "!", "\0"},
};

const uint8_t arrows[] = {KEY_LEFT_ARROW, KEY_UP_ARROW, KEY_RIGHT_ARROW, KEY_DOWN_ARROW};
const uint8_t mouseButton[2] = {MOUSE_RIGHT, MOUSE_LEFT};

int ax_x;
int ax_y;
char* preShiin = "";
char* preBoin = "";
char preAlp = "";
int inputMode = 0; //0:JPN, 1:ENG, 2:NUM
int preInputMode = 1;
bool Entered;
int pushButtonCnt = 0;
int Mode = 0; //0:keyboard, 1:mouse
int modeCount = 0;
int relcnt = 0;
int tilcnt = 0;
int delcnt = 0;
int p_Press = -1;
int Press = -1;
int p_Stick = 0;
int Stick = 0;


void setup() {
  Init();
  //Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  updateText();
}

void loop() {
  modeSwitch();
  if (Mode == 0) {
    functional();
    inputs();
    delay(10);
  } else {
    checkClick();
    for (int i = 0; i < 10; i++) {
      readMouse();
      Mouse.move(ax_x, ax_y, 0);
      delay(2);
    }
  }
}




void Init(void)
{
  for (int i = 0; i < 5; i++) pinMode(rows[i], OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(cols[i], INPUT_PULLUP);
  }
  pinMode(_Jx, INPUT);
  pinMode(_Jy, INPUT);
  pinMode(_Jsw, INPUT);
}

int checkPress() {
  for (int index = 0; index < 3; index++) {
    for (int i = 0; i < 5; i++) digitalWrite(rows[i], HIGH);
    digitalWrite(rows[index], LOW);
    for (int i = 0; i < 4; i++) {
      if (!digitalRead(cols[i])) {
        return (4 * index) + i;
      }
    }
  }
  return -1;
}

int getDirection() {
  int x = analogRead(_Jx) - 502;
  int y = analogRead(_Jy) - 502;
  if (abs(x) > abs(y)) {
    if (x > 200) return 2;
    else if (x < -200) return 4;
  } else {
    if (y > 100) return 3;
    else if (y < -200) return 1;
  }
  return 0;
}

int checkJoySwitch() {
  return analogRead(A1) == 0;
}


void modeSwitch() {
  if (checkJoySwitch()) modeCount++;
  else modeCount = max(0, modeCount - 1);
  if (modeCount > 50) {
    modeCount = 0;
    Mode = (Mode + 1) % 2;
    updateText();
  }
}

void inputs() {
  p_Press = Press;
  Press = checkPress();
  p_Stick = Stick;
  Stick = getDirection();
  if (Press >= 0) {
    if (p_Press < 0) {
      _Input(Press, Stick);
      Entered = true;
    } else {
      if (Stick > 0) {
        if (p_Stick != Stick) {
          if (Entered) {
            if (Press!=3){
              for (int i=0;i<delcnt;i++)
                Keyboard.write(KEY_BACKSPACE);
            }
            delcnt = 0;
            _Input(Press, Stick);
          } else {
            _Input(Press, Stick);
            Entered = true;
          }
        }
      } else {
        if (p_Stick == Stick) {
          relcnt++;
          if (relcnt == 15) {
            if (Press!=3){
              for (int i=0;i<delcnt;i++)
                Keyboard.write(KEY_BACKSPACE);
            }
            delcnt = 0;
            _Input(Press, 0);
          }
        } else {
          relcnt = 0;
        }
      }
    }
  }else{
    if (Stick>0){
      if ((p_Stick == Stick)&&!Entered){
        tilcnt++;
        if (tilcnt == 3) {
              Keyboard.press(arrows[Stick - 1]);
              preShiin = "";
              preBoin = "";
              preAlp = "";
        }
      }else{
        Keyboard.releaseAll();
        if (tilcnt>3){
          Keyboard.press(arrows[Stick - 1]);
        }else{
          tilcnt = 0;
        }
      }
    }else{
      Keyboard.releaseAll();
      Entered = false;
      tilcnt = 0;
    }  
  }
}


void _Input(int pb, int fd) {
  switch (inputMode) {
    case (0):
      kanaInput(pb, fd);
      return;
    case (1):
      enInput(pb, fd);
      return;
    case (2):
      numInput(pb);
      return;
  }
}


void kanaInput(int pb, int fd) {
  if (pb < 0) return;
  if (pb == 3) {
    bool converted = true;
    if ((preShiin == "t") && (preBoin == "u")) {
      preShiin = "lt";
    } else if (preShiin == "lt") {
      preShiin = "d";
    } else if ((preShiin == "l") && (preBoin == "u")) {
      preShiin = "v";
    } else if (preShiin == "y") {
      preShiin = "ly";
    } else if (preShiin == "ly") {
      preShiin = "y";
    } else if (henkan.find(preShiin) != henkan.end()) {
      preShiin = henkan[preShiin];
    } else {
      converted = false;
    }
    if (converted) {
      Keyboard.write(KEY_BACKSPACE);
      Keyboard.print(preShiin);
      Keyboard.print(preBoin);
    }
  } else if ((pb == 7) | (pb == 11)) {
    preShiin = "";
    preBoin = "";
    delcnt = 1;
    if ((pb == 11) && (fd == 4)) delcnt = 3;
    Keyboard.print(ex[(2 * fd) + (pb / 8)]);
  } else {
    delcnt = 1;
    if ((pb == 6) && ((fd == 1) || (fd == 3))) {
      preShiin = "";
      preBoin = "";
      if (fd == 1) Keyboard.print("(");
      else Keyboard.print(")");
    } else {
      preShiin = shiin[pb];
      preBoin = boin[fd];
      Keyboard.print(preShiin);
      Keyboard.print(preBoin);
    }
  }
}

void enInput(int pb, int fd) {
  if (pb < 0) return;
  if (pb == 3) {
    bool converted = true;
    if ((preAlp > 64) && (preAlp < 91)) {
      preAlp += 32;
    } else if ((preAlp > 96) && (preAlp < 123)) {
      preAlp -= 32;
    } else {
      converted = false;
    }
    if (converted) {
      Keyboard.write(KEY_BACKSPACE);
      Keyboard.print(preAlp);
    }
  } else {
    preAlp = *keys_e[pb][fd];
    if (preAlp>0) delcnt = 1;
    Keyboard.print(preAlp);
  }
}

void numInput(int pb) {
  Keyboard.print(nums[pb]);
}



int checkFunctionalKeys() {
  for (int index = 0; index < 2; index++) {
    for (int i = 0; i < 5; i++) digitalWrite(rows[i], HIGH);
    digitalWrite(rows[index + 3], LOW);
    for (int i = 0; i < 3; i++) {
      if (!digitalRead(cols[i])) {
        return (3 * index) + i;
      }
    }
  }
  return -1;
}


void functional() {
  int fk = checkFunctionalKeys();
  switch (fk) {
    case (0):
      Keyboard.press(KEY_BACKSPACE);
      break;
    case (1):
      Keyboard.press(KEY_RETURN);
      break;
    case (2):
      Keyboard.press(' ');
      break;
    case (3):
      if (inputMode != 2) {
        preInputMode = inputMode;
        inputMode = 2;
      } else {
        inputMode = preInputMode;
        preInputMode = 2;
      }
      updateText();
      break;
    case (4):
      if (inputMode == 2) {
        inputMode = preInputMode;
        preInputMode = 2;
      } else {
        preInputMode = inputMode;
        inputMode = 1 - inputMode;
      }
      Keyboard.press(KEY_RIGHT_ALT);
      Keyboard.press(0x60);
      Keyboard.press(0x35);
      updateText();
      break;
  }
  if (fk >= 0) {
    preShiin = "";
    preBoin = "";
    preAlp = "";
  }
  while (checkFunctionalKeys() >= 0) delay(50);
  Keyboard.release(KEY_BACKSPACE);
  Keyboard.release(KEY_RETURN);
  Keyboard.release(KEY_RIGHT_ALT);
  Keyboard.release(0x60);
}

void updateText() {
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 4);
  display.setTextColor(SSD1306_WHITE);
  if (Mode == 0) {
    if (inputMode == 0) display.print(F("Mode:JA"));
    else if (inputMode == 1) display.print(F("Mode:EN"));
    else display.print(F("Mode:No"));
  } else {
    display.print(F("Mode:MS"));
  }
  display.display();
}

void readMouse() {
  ax_x = 5 * (analogRead(_Jy) - 502) / 512;
  ax_y = -5 * (analogRead(_Jx) - 502) / 512;
}

void checkClick() {
  bool s[2] = {false, false};
  for (int index = 0; index < 2; index++) {
    for (int i = 0; i < 5; i++) digitalWrite(rows[i], HIGH);
    digitalWrite(rows[index + 3], LOW);
    for (int i = 0; i < 3; i++) {
      if (!digitalRead(cols[i])) {
        s[index] = true;
      }
    }
  }
  for (int index = 0; index < 2; index++) {
    if (s[index]) {
      if (!Mouse.isPressed(mouseButton[index])) {
        Mouse.press(mouseButton[index]);
      }
    } else {
      if (Mouse.isPressed(mouseButton[index])) {
        Mouse.release(mouseButton[index]);
      }
    }
  }
}
