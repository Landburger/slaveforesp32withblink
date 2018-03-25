/**
   ESPNOW - Basic communication - Slave
   Date: 26th September 2017
   Author: Arvind Ravulavaru <https://github.com/arvindr21>
   Purpose: ESPNow Communication between a Master ESP32 and a Slave ESP32
   Description: This sketch consists of the code for the Slave module.
   Resources: (A bit outdated)
   a. https://espressif.com/sites/default/files/documentation/esp-now_user_guide_en.pdf
   b. http://www.esploradores.com/practica-6-conexion-esp-now/

   << This Device Slave >>

   Flow: Master
   Step 1 : ESPNow Init on Master and set it in STA mode
   Step 2 : Start scanning for Slave ESP32 (we have added a prefix of `slave` to the SSID of slave for an easy setup)
   Step 3 : Once found, add Slave as peer
   Step 4 : Register for send callback
   Step 5 : Start Transmitting data from Master to Slave

   Flow: Slave
   Step 1 : ESPNow Init on Slave
   Step 2 : Update the SSID of Slave with a prefix of `slave`
   Step 3 : Set Slave in AP mode
   Step 4 : Register for receive callback and wait for data
   Step 5 : Once data arrives, print it in the serial monitor

   Note: Master and Slave have been defined to easily understand the setup.
         Based on the ESPNOW API, there is no concept of Master and Slave.
         Any devices can act as master or salve.
*/
//######################################################################################################
//onDataRecv ints
uint8_t recVal = 3;   //recieved value from master
//ints used for controlling LEDs
uint8_t ledPin = 33;  //pin used to control the strip
uint8_t numOfLeds = 44; //number of leds on strip
//blinkLight ints and bools
unsigned long timer = 0;
long buttonTimerR = 0;
long buttonTimerL = 0;
bool blinkRightOn = false;
bool blinkLeftOn = false;
//######################################################################################################
//espressif libraries
#include <esp_now.h>
#include <WiFi.h>
//######################################################################################################
//adafruit neopixel library
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numOfLeds, ledPin, NEO_GRB + NEO_KHZ800);
//######################################################################################################
//defines
#define CHANNEL 1
//######################################################################################################
// Init ESP Now with fallback
void InitESPNow() {
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}
//######################################################################################################
// config AP SSID
void configDeviceAP() {
  char* SSID = "Slave_1";
  bool result = WiFi.softAP(SSID, "Slave_1_Password", CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}
//######################################################################################################
// Setup
void setup() {
  //esp32 magic
  Serial.begin(115200);
  Serial.println("ESPNow/Basic/Slave Example");
  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);
//######################################################################################################
  //strip-pin
  pinMode(ledPin, OUTPUT);
//######################################################################################################
  //neopixel inits
  strip.begin(); //initialize strip
  strip.show();  // Initialize all pixels to 'off'
//######################################################################################################

}
// callback when data is recv from Master, runs all the time
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
//######################################################################################################
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Last Packet Recv Data: "); Serial.println(*data);
  Serial.println("");
//######################################################################################################
  recVal = *data;

  checkRecVal();
}
//######################################################################################################

//dont use, ignore
void loop() {
  //Ignore loop, put everything in OnDataRecv
}
//turns blinkers on right side on
void blinkLightRight(bool right) {
  if (right) {
    if (millis() - timer > 500) {
      for (int i = 4; i < 12; i--) {
        strip.setPixelColor(i, 255, 60, 0);
        strip.setPixelColor(numOfLeds - i, 255, 60, 0);
        strip.show();
        delay(50);
        if (recVal != 1 * recVal != 5) {
          break;
        }
      }
      unsigned long offDelay = timer + 500;
      if (millis() - offDelay > 100) {
        for (int i = 4; i < 12; i++) {
        strip.setPixelColor(i, 0, 0, 0);
        }
        strip.show();
        timer = millis();
      }
    }
  }
}
//turns blinkers on left side on
void blinkLightLeft(bool left) {
  if (left) {
    if (millis() - timer > 500) {
      for (int i = numOfLeds - 4; i > numOfLeds - 12; i--) {
        strip.setPixelColor(i, 255, 60, 0);
        strip.show();
        delay(50);
        if (recVal != 2 * recVal != 6) {
          break;
        }
      }
      unsigned long offDelay = timer + 500;
      if (millis() - offDelay > 100) {
        for (int i = numOfLeds - 4; i > numOfLeds - 12; i--) {
        strip.setPixelColor(i, 0, 0, 0);
        }
        strip.show();
        timer = millis();
      }
    }
  }
}
//Brake lights
void brakeLight(bool on) {
  if (on) {
    for (int i = 1; i < 4; i++) {
      strip.setPixelColor(i, 255, 0, 0);
      strip.setPixelColor(numOfLeds-i, 255, 0, 0);
    }
    strip.show();
  }
  else {
    for (int i = 0; i < 4; i++) {
      strip.setPixelColor(i, 0, 0, 0);
      strip.setPixelColor(numOfLeds-i, 0, 0, 0);
    }
    strip.show();
  }
}

//check data recieved
void checkRecVal(){
  //if nothing do nothing
  if(recVal == 0){
    brakeLight(false);
    blinkLightRight(false);
    blinkLightLeft(false);
  }
  //Blink right
  if(recVal == 1){
    brakeLight(false);
    blinkLightRight(true);
    blinkLightLeft(false);
  }
  //Blink left
  if(recVal == 2){
    brakeLight(false);
    blinkLightRight(false);
    blinkLightLeft(true);
  }
  //Brake lights
  if(recVal == 3){
    brakeLight(true);
    blinkLightLeft(false);
    blinkLightRight(false);
  }
  //Brake lights and blink right
  if(recVal == 4){
    brakeLight(true);
    blinkLightRight(true);
    blinkLightLeft(false);
  }
  //Brake lights and blink left
  if(recVal == 5){
    brakeLight(true);
    blinkLightLeft(true);
    blinkLightRight(false);
  }
}

