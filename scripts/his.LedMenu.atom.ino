/* one-button menu for M5Atom Lite
   uint8_t led_menu(uint8_t *ret_p)
      set led color for menu by global const long menuGRBdata[ret]
      bit23-16:Green bit15-8:Red bit7-0:Blue
      NOTICE : menuGRBdata[] is arranged Green-Red-Blue,
      Depending on the specifications of RGB LED (SK6812/WS2812B-2020)

      push button :
        ret++, if ret over range of menu no,ret=0
        led color change to menuGRBdata[new ret]
      long-push button : flush led and exit led_menu()
*/
#include "M5Atom.h"
#include <WiFi.h>
// #include <WiFiMulti.h>
#include <HTTPClient.h>

//My hone WiFi & host to PUT
const char* ssid       = "wifi-SSID";
const char* password   = "wifi-password";

// led color data for Menu. G-R-B order
const long menuGRBdata[] = {
  0x002000,  //Red
  0x400000,  //Green
  0x000040,  //Blue
  0x402000,  //Yellow
  0x400030,  //BlueGreen
  0x002040,  //Pink
  0x402040,  //White
  0x000000   //black
};
// No of Menu
#define MENUSIZE (sizeof(menuGRBdata)/sizeof(menuGRBdata[0]))

const char* urlData[] = {
  "http://192.168.0.115/POST/0",
  "http://192.168.0.115/POST/1",
  "http://192.168.0.115/POST/2",
  "http://192.168.0.115/POST/3",
  "http://192.168.0.115/POST/4",
  "http://192.168.0.115/POST/5",
  "http://192.168.0.115/POST/6",
  "http://192.168.0.115/POST/7"
};

//------------------------------------------------

void led_menu(uint8_t *ret_p) {
  bool selected = false;

  M5.dis.drawpix(0, menuGRBdata[*ret_p]);
  while (1) {
    delay(50);
    M5.update();
    if (M5.Btn.pressedFor(500)) {
      selected = true;
      while (1) {
        M5.dis.drawpix(0, 0);
        delay(50);
        M5.dis.drawpix(0, menuGRBdata[*ret_p]);
        delay(50);
        M5.update();
        if (M5.Btn.wasReleased()) break;
      }
      break;
    }
    if (M5.Btn.wasReleased()) {
      (*ret_p)++;
      if (*ret_p >= MENUSIZE) *ret_p = 0;
      M5.dis.drawpix(0, menuGRBdata[*ret_p]);  // これでOK?
      Serial.printf("MENU:%d\n", *ret_p);
    }
  }
  M5.dis.drawpix(0, 0);
}
//====================================

void setup() {
  M5.begin(true, false, true);
  delay(10);
  Serial.println("Hello");
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");
}

//====================================

uint8_t ret = 0;
void loop() {


  HTTPClient http;
  led_menu(&ret);
  Serial.printf("SELECTED:menu = %d , Color = %06lx\n", ret, menuGRBdata[ret] );
  Serial.print("[HTTP] begin...\n");
  //  Serial.print(httpServer);
  Serial.print(urlData[ret]);
  http.begin(urlData[ret]); //HTTP
  // start connection and send HTTP header
  int httpCode = http.POST(String(menuGRBdata[ret]));

  delay(2000);
}
