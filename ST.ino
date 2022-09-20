/*************************连接方法*************************
  Max7129 LED Matrix Pin -> ESP8266 Pin
  Vcc                   -> 3v  (3V on NodeMCU 3V3 on WEMOS)
  Gnd                   -> Gnd (G on NodeMCU)
  DIN                   -> D7  (Same Pin for WEMOS)
  CS                    -> D4  (Same Pin for WEMOS)
  CLK                   -> D5  (Same Pin for WEMOS)
*********************************************************/

#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <time.h>
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

int pinCS = 2; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 4; // 水平方向有 4 块屏幕
int numberOfVerticalDisplays   = 1; // 垂直方向有 1 块屏幕
#define _DISPLAY_ROTATE 1 // 旋转方向，1：0°，2：90°，3：180°，4：270°

const byte buffer_size = 45;
char time_value[buffer_size];
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

const char *ssid      = "Sun";   //输入你的 WiFi 名称
const char *password  = "88888888";   //输入你的 WiFi 密码

void setup() {

  matrix.setIntensity(0);     // 屏幕亮度 0-16档
  for (int i = 0; i < numberOfHorizontalDisplays;  i++)
    matrix.setRotation(i, _DISPLAY_ROTATE);

  matrix.fillScreen(LOW);
  
  //初始四块屏幕显示内容 2距离左边距离，0距离上面距离
  matrix.drawChar(2, 0, 'L', HIGH, LOW, 1);  // H
  matrix.drawChar(8, 0, 'O', HIGH, LOW, 1);  // HH
  matrix.drawChar(14, 0, 'V', HIGH, LOW, 1); // HH:
  matrix.drawChar(20, 0, 'E', HIGH, LOW, 1); // HH:M
  matrix.drawChar(26, 0, '!', HIGH, LOW, 1); // HH:MM

  matrix.write(); // Send bitmap to display

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.begin(115200);
  Serial.println();
  Serial.printf("Flash: %d\n", ESP.getFlashChipRealSize());
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to wifi. My address:");
  IPAddress myAddress = WiFi.localIP();
  Serial.println(myAddress);

  configTime(0 * 3600, 0, "ntp1.aliyun.com", "time.nist.gov");
  setenv("TZ", "CST-8", 1);

  time_t now;
  time(&now);
  while (now < 120) {
    delay(100);
    time(&now);
    Serial.println(now);
  }
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
}

void loop() {
  time_t now;
  char time_output[buffer_size];
  String time_str;

  time(&now);
  strftime(time_output, buffer_size, "( %a  %y-%m-%d ) ( %p %T )", localtime(&now));
  time_str = String(time_output);
  time_str.trim();
  time_str.substring(23, 28).toCharArray(time_value, 10);

  Serial.println(time_value);

  matrix.fillScreen(LOW);
  //( Sun  21-07-19 ) ( PM 12:52:12 )
  matrix.drawChar(2, 0, time_value[0], HIGH, LOW, 1);  // H
  matrix.drawChar(8, 0, time_value[1], HIGH, LOW, 1);  // HH
  matrix.drawChar(14, 0, time_value[2], HIGH, LOW, 1); // HH
  matrix.drawChar(20, 0, time_value[3], HIGH, LOW, 1); // HH:M
  matrix.drawChar(26, 0, time_value[4], HIGH, LOW, 1); // HH:MM
  matrix.write(); // Send bitmap to display

  delay(5000);
}