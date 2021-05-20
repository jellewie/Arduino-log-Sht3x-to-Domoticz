bool TickEveryXms(unsigned long *_LastTime, unsigned long _Delay) {
  //With overflow, can be adjusted, no overshoot correction, true when (Now < _LastTime + _Delay)
  /* Example:   static unsigned long LastTime;
                if (TickEveryXms(&LastTime, 1000)) {//Code to run}    */
  static unsigned long _Middle = -1;                              //We just need a really big number, if more than 0 and less than this amount of ms is passed, return true)
  if (_Middle == -1) _Middle = _Middle / 2;                       //Somehow declairing middle on 1 line does not work
  if (millis() - (*_LastTime + _Delay + 1) < _Middle) {           //If it's time to update (keep brackets for overflow protection). If diffrence between Now (millis) and Nextupdate (*_LastTime + _Delay) is smaller than a big number (_Middle) then update. Note that all negative values loop around and will be really big (for example -1 = 4,294,967,295)
    *_LastTime = millis();                                        //Set new LastTime updated
    return true;
  }
  return false;
}
String IpAddress2String(const IPAddress& ipAddress) {
  return String(ipAddress[0]) + String(".") +
         String(ipAddress[1]) + String(".") +
         String(ipAddress[2]) + String(".") +
         String(ipAddress[3]);
}
bool StringIsDigit(String IN, char IgnoreCharA = '0', char IgnoreCharB = '0');
bool StringIsDigit(String IN, char IgnoreCharA, char IgnoreCharB) {
  //IgnoreChar can be used to ignore ',' or '.' or '-'
  for (byte i = 0; i < IN.length(); i++) {
    if (isDigit(IN.charAt(i))) {                                  //If it is a digit, do nothing
    } else if (IN.charAt(i) == IgnoreCharA) {                     //If it is IgnoreCharA, do nothing
    } else if (IN.charAt(i) == IgnoreCharB) {                     //If it is IgnoreCharB, do nothing
    } else {
      return false;
    }
  }
  return true;
}
