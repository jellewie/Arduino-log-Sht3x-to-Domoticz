/* Written by JelleWho https://github.com/jellewie
   https://github.com/jellewie/Arduino-WiFiManager
*/
//===========================================================================
// Things that can/need to be defined after including "WiFiManager.h"
//===========================================================================
const byte Pin_LED  = LED_BUILTIN;                              //Just here for some examples, It's the LED to give feedback on (like blink on error)
bool WiFiManagerUser_Set_Value(byte ValueID, String Value) {
  switch (ValueID) {                                            //Note the numbers are shifted from what is in memory, 0 is the first user value
    case 0: {
        if (Value.length() > sizeof(Name))        return false; //Length is to long, it would not fit so stop here
        Value.toCharArray(Name, 16);              return true;
      } break;
    case 1: {
        if (not StringIsDigit(Value))             return false;  //No number given
        if (Value.toInt() < 1)                    return false;  //PollRate to low, should be at least 1
        PollingRateSec = Value.toInt();           return true;
      } break;
    case 2: {
        if (Value.length() > sizeof(Dom_IP))      return false; //Length is to long, it would not fit so stop here
        Value.toCharArray(Dom_IP, 16);            return true;
      } break;
    case 3: {
        if (not StringIsDigit(Value))             return false;  //No number given
        Dom_Port = Value.toInt();                 return true;
      } break;
    case 4: {
        if (not StringIsDigit(Value))             return false;  //No number given
        Address[0] = Value.toInt();               return true;
      } break;
    case 5: {
        if (not StringIsDigit(Value))             return false;  //No number given
        IDX[0] = Value.toInt();                   return true;
      } break;
    case 6: {
        if (not StringIsDigit(Value))             return false;  //No number given
        Address[1] = Value.toInt();               return true;
      } break;
    case 7: {
        if (not StringIsDigit(Value))             return false;  //No number given
        IDX[1] = Value.toInt();                   return true;
      } break;
  }
  return false;                                                 //Report back that the ValueID is unknown, and we could not set it
}
String WiFiManagerUser_Get_Value(byte ValueID, bool Safe, bool Convert) {
  //if its 'Safe' to return the real value (for example the password will return '****' or '1234')
  //'Convert' the value to a readable string for the user (bool '0/1' to 'FALSE/TRUE')
  switch (ValueID) {                                            //Note the numbers are shifted from what is in memory, 0 is the first user value
    case 0:   return String(Name);                break;
    case 1:   return String(PollingRateSec);      break;
    case 2:   return String(Dom_IP);              break;
    case 3:   return String(Dom_Port);            break;
    case 4:   return String(Address[0]);          break;
    case 5:   return String(IDX[0]);              break;
    case 6:   return String(Address[1]);          break;
    case 7:   return String(IDX[1]);              break;
  }
  return "";
}
void WiFiManagerUser_Status_Start() {                           //Called before start of WiFi
  pinMode(Pin_LED, OUTPUT);
  digitalWrite(Pin_LED, HIGH);
}
void WiFiManagerUser_Status_Done() {                            //Called after succesfull connection to WiFi
  digitalWrite(Pin_LED, LOW);
}
void WiFiManagerUser_Status_Blink() {                           //Used when trying to connect/not connected
  digitalWrite(Pin_LED, !digitalRead(Pin_LED));
}
void WiFiManagerUser_Status_StartAP() {}                        //Called before start of APmode
bool WiFiManagerUser_HandleAP() {                               //Called when in the While loop in APMode, this so you can exit it
  //Return true to leave APmode
#define TimeOutApMode 15 * 60 * 1000;                           //Example for a timeout, (time in ms)
  unsigned long StopApAt = millis() + TimeOutApMode;
  if (millis() > StopApAt)    return true;                      //If we are running for to long, then flag we need to exit APMode
  return false;
}
