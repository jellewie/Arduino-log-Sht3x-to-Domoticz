/*Written by JelleWho https://github.com/jellewie
  TODO:  https://github.com/jellewie/Arduino-log-Sht3x-to-Domoticz/issues

  ESP32 I2C default layout:
    SDA = GPIO 21
    SCL = GPIO 22
    GND = GND
    VCC = 3.3V
*/
#if !defined(ESP32)
#error "Please check if the 'DOIT ESP32 DEVKIT V1' board is selected, which can be downloaded at https://dl.espressif.com/dl/package_esp32_index.json"
#endif

//#define SerialEnabled
#ifdef SerialEnabled
#define     WiFiManager_SerialEnabled                           //WM:
#endif //SerialEnabled

char Dom_IP[16] = "192.168.255.255";                            //SOFT_SETTING The IP to connect to where Domoticz is hosted on
int Dom_Port = 80;                                              //SOFT_SETTING The port where Domoticz is hosted on
byte Address[2] = {0x44, 0x45};                                 //SOFT_SETTING Address of I2C secondary
int IDX[2] = {0, 0};                                            //SOFT_SETTING The IDX in Domoticz of the dummy RH device
int PollingRateSec = 300;                                       //SOFT_SETTING Amount of seconds to 'wait' between getting data and sending it to Dpomoticz
byte CMD_MEAS_POLLING_H[2] = {0x24, 0x00};                      //The measurement command to send to the  I2C secondary

#include <Wire.h>
#include "Functions.h"
#include "SH3x.h"
#include "WiFiManagerBefore.h"                                  //Define what options to use/include or to hook into WiFiManager
#include "WiFiManager/WiFiManager.h"                            //Includes <WiFi> and <WebServer.h> and setups up 'WebServer server(80)' if needed
#include "WiFiManagerLater.h"                                   //Define options of WiFiManager (can also be done before), but WiFiManager can also be called here (example for DoRequest)
#include "Domoticz.h"

void setup() {
#ifdef SerialEnabled
  Serial.begin(115200);
#endif //SerialEnabled
  Wire.begin();
  server.on("/test", handle_Test);
  byte Answer = WiFiManager.Start();                            //Run the Wi-Fi start-up (and save results)
  Answer = Answer + 0;                                          //Remove the compiler warning, by 'using' the value. This is for when SerialEnabled=false
#ifdef SerialEnabled
  Serial.println("WiFi setup executed with responce code '" + String(Answer) + "'"); //The return codes can be found in "WiFiManager.cpp" in "CWiFiManager::Start("
#endif //SerialEnabled
  WiFiManager.EnableSetup(true);                                //(runtime) Enable the settings, only enabled in APmode by default

  for (byte i = 0; i < sizeof(Address) / sizeof(Address[0]); i++) { //Report one-time warning if config is wrong
    if (Address[i] == 0 or IDX[i] == 0)
      LogToDomoticz(String(Name) + " at " + IpAddress2String(WiFi.localIP()) + " reports config error on ID_" + String(i) + ", some value hasn't been set, and will be skipped; Address=" + String(Address[0]) + " IDX=" + String(IDX[0]), 2);
  }
}
void loop() {
  WiFiManager.RunServer();                                      //(runtime) Note when using OTA, you need to run the server in your loop
  static unsigned long LastTimeA = 2147999999;                  //Set really high above mid point, so the first call is TRUE
  if (TickEveryXms(&LastTimeA, PollingRateSec * 1000)) {        //Execute code every x Seconds
    digitalWrite(Pin_LED, HIGH);
    for (byte i = 0; i < sizeof(Address) / sizeof(Address[0]); i++) {
      SHT Answer = DoMeasurement(Address[i], CMD_MEAS_POLLING_H, IDX[i]);
#ifdef SerialEnabled
      Serial.println("Answer of ID_" + String (i) + ": C=" + String(Answer.Code) + " T=" + String(Answer.Temperature) + " H=" + String(Answer.Humidity));
#endif //SerialEnabled
      if (Answer.Code > 1)                                      //If we received an error (and it wasn’t a general not-configured error
        LogToDomoticz(String(Name) + " at " + IpAddress2String(WiFi.localIP()) + " reports DoMeasurement ID_" + String(i) + " failed with code " + Answer.Code, 4);
    }
    digitalWrite(Pin_LED, LOW);
  }
}

SHT DoMeasurement(byte _I2CAddress, byte _CMD[2], int _IDX) {
  SHT Return;
  /* Returns
    0       = Succesfull
    1       = Config error; no 'IDX' 'I2CAddress' or 'CMD' given
    16<x<32 = SendCommand failed with I2CAddress code #-16         See "SendCommand("
    32<x<48 = Sending data to Domoticz failed with code #-32       See "CWiFiManager::DoRequest(" in "WiFiManager.cpp" called by "SendDataToDomoticz("
  */
  if (_I2CAddress == 0 or _IDX == 0 or (_CMD[0] == 0 and _CMD[1] == 0)) {
    Return.Code = 1;
    return Return;                                              //If there is a config error (something isn't set) return and report it
  }
  Return = SendCommand(_I2CAddress, _CMD);                      //Get data from SHT3x
  if (Return.Code != 0) {
    Return.Code = Return.Code + 16;                             //If there was an error, stop and return it
    return Return;
  }
  //HUM_WET >70,  HUM_DRY <30,  HUM_COMFORTABLE 40-60,  HUM_NORMAL 30-40 & 60-70 (just everything else)
  byte Hum_Stat = Return.Humidity > 70 ? HUM_WET : (Return.Humidity < 30 ? HUM_DRY : ((Return.Humidity >= 40 and Return.Humidity <= 60) ? HUM_COMFORTABLE : HUM_NORMAL));
  byte Result = SendDataToDomoticz(_IDX, Return.Temperature, Return.Humidity, Hum_Stat);
  if (Result != 1) {
    Return.Code = Return.Code + 32;                             //If there was an error, stop and return it
    return Return;
  }
  return Return;                                                //Return no error code, we where successful (for once...)
}

void handle_Test() {
  String message = "Reports to Domoticz at " + String(Dom_IP) + ":" + String(Dom_Port) + "\n";
  for (byte i = 0; i < sizeof(Address) / sizeof(Address[0]); i++) {
    SHT Answer = DoMeasurement(Address[i], CMD_MEAS_POLLING_H, IDX[i]);
    message = message + "Answer of ID_" + String (i) + " at I2C=" + String(Address[i]) + "(0x" + String(Address[i], HEX) + ") with IDX=" + String(IDX[i]) + ": C=" + String(Answer.Code) + " T=" + String(Answer.Temperature) + " H=" + String(Answer.Humidity) + "\n";
  }
  server.send(200, "text/plain", message);
}
