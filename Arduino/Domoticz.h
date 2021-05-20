byte SendDataToDomoticz(int IDX, float TEMP, float HUM, byte HUM_STAT) {    //https://www.domoticz.com/wiki/Domoticz_API/JSON_URL's#Temperature.2Fhumidity
  /*URL      = /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=TEMP;HUM;HUM_STAT
    IDX      = id of your device (This number can be found in the devices tab in the column "IDX")
    TEMP     = Temperature  must be delivered as svalue (String)
    HUM      = Humidity (0-100 %) is interpeted as a int (only whole numbers)
    HUM_STAT = Humidity status (0=Normal,1=Comfortable,2=Dry,3=Wet
  */
  //The return codes can be found in "CWiFiManager::DoRequest(" in "WiFiManager.cpp"
  String Path = "/json.htm?type=command&param=udevice&idx=" + String(IDX) + "&nvalue=0&svalue=" + String(TEMP) + ";" + String(round(HUM)) + ";" + String(HUM_STAT);
  return WiFiManager.DoRequest(Dom_IP, Dom_Port, Path);
}
byte LogToDomoticz(String MSG, byte LEVEL = 1);
byte LogToDomoticz(String MSG, byte LEVEL) {
  /* LEVEL = 1=Normal, 2=Status, 4=Error
     The return codes can be found in "CWiFiManager::DoRequest(" in "WiFiManager.cpp"
     Do NOT use special characters https://www.url-encode-decode.com/
  */
  MSG.replace(" ", "%20");
  String Path = "/json.htm?type=command&param=addlogmessage&message=" + MSG + "&level=" + String(LEVEL);
  return WiFiManager.DoRequest(Dom_IP, Dom_Port, Path);
}
//byte SendDataToDomoticz_N(String IDX, float Value) {
//  //The return codes can be found in "WiFiManager.cpp" in "CWiFiManager::DoRequest("
//  String Path = "/json.htm?type=command&param=udevice&idx=" + IDX + "&nvalue=" + String(Value);
//  return WiFiManager.DoRequest(Dom_IP, Dom_Port, Path);
//}
//byte SendDataToDomoticz_S(String IDX, String Value) {
//  //The return codes can be found in "CWiFiManager::DoRequest(" in "WiFiManager.cpp"
//  String Path = "/json.htm?type=command&param=udevice&idx=" + IDX + "&svalue=" + String(Value);
//  return WiFiManager.DoRequest(Dom_IP, Dom_Port, Path);
//}
