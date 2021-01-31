![flick|O17](/images/fliclO17.jpg?raw=true)
# flic|O17 firmware
物理フリック入力キーボード「flic|O17」のファームウェアです。

## Dependency
C++, [ArxContainer.h](https://github.com/hideakitai/ArxContainer), Wire.h, Adafruit_SSD1306.h

## Usage
[Arduino IDE](https://www.arduino.cc/en/software)を使用してPro microに書き込みます。

## PIN assignments

### Key matrix
||18|15|14|16|10|
|:---:|:---:|:---:|:---:|:---:|:---:|
|**９**|あ|か|さ|BS|Num|  
|**８**|た|は|ま|Enter|あA| 
|**７**|や|ら|わ|Space||
|**６**|”゜|わ|!?|||


### Joy stick
|PIN|JoyStick|
|:--:|:--:|
|**VCC**|VCC|
|**GND**|GND|
|**21(A3)**|X|
|**20(A2)**|Y|
|**19(A1)**|Push switch|

### OLED
|PIN|OLED|
|:--:|:--:|
|**VCC**|VCC|
|**GND**|GND|
|**3(SDA)**|SDA|
|**2(SCL)**|SCL|

## License
MIT

## Authors
**Cartelet**
[Twitter](https://twitter.com/_mitaki_/)
[Qiita](https://qiita.com/Cartelet)
