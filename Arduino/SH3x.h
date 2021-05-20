enum {HUM_NORMAL, HUM_COMFORTABLE, HUM_DRY, HUM_WET};
enum {WIRE_SUCCES, WIRE_TOMUCH, WIRE_NACKADRES, WIRE_NACKDATA, WIRE_OTHER};

struct SHT {
  byte Code = 0;                                                //Error code (0=Succes)
  float Temperature = 0.0;                                      //Temperature in [°C]
  float Humidity = 0.0;                                         //Relative humidity in [%]
};

static uint8_t crc8(const uint8_t *data, int len) {
  //CRC-8 formula from page 14 of SHT spec pdf    0xBE, 0xEF should yield 0x92
  const uint8_t POLYNOMIAL(0x31);                               //Polynomial 0x31 (x8 + x5 +x4 +1)
  uint8_t crc(0xFF);                                            //Initialization data 0xFF
  for (int j = len; j; --j) {
    crc ^= *data++;
    for (int i = 8; i; --i)
      crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
  }
  return crc;
}
#define SendCommandLength 2
SHT SendCommand(byte _I2CAddress, byte _CMD[SendCommandLength], byte _DelayBeforeReadingMs = 15, byte _ResponceLength = 6);
SHT SendCommand(byte _I2CAddress, byte _CMD[SendCommandLength], byte _DelayBeforeReadingMs, byte _ResponceLength) {
  /*CMD[2] Only accepts 2 bytes ATM: '0x2400' = 0x24 and 0x00 (Hardcoded in this array[x] header call line thingy)
    DelayBeforeReading  = time in ms to wait before asking for an answer     //15 = max Measurement duration of High repeatability
    ResponceLength      = Amount of bytes of the answer we want

    Returns '.Code':
      0=SUCCES ('.Temperature' and '.Humidity' will also have their respected values (else its 'NULL'/'0')
      1=WIRE_TOMUCH      //https://www.arduino.cc/en/Reference/WireEndTransmission
      2=WIRE_NACKADRES
      3=WIRE_NACKDATA
      4=WIRE_OTHER
      5=Unknown WIRE responce code
      6=Reponce length wrong
      7=CRC error occured
  */
  SHT Return;                                                   //Create return variable
  Wire.beginTransmission(_I2CAddress);
  for (byte i = 0; i < SendCommandLength; i++)
    Wire.write(_CMD[i]);                                        //Send all bytes to Secondary
  byte Answer = Wire.endTransmission();
  if (Answer != WIRE_SUCCES) {                                  //If sending wasn't succesfull, return the error code
    Return.Code = Answer < 5 ?  Answer : 5;
    return Return;
  }
  delay(_DelayBeforeReadingMs);                                 //Wait a bit for the measurement
  Wire.requestFrom(_I2CAddress, _ResponceLength);
  byte AvailableLength = Wire.available();
  if (AvailableLength != _ResponceLength) {
    Return.Code = 6;
    return Return;
  }
  byte ReturnValue[_ResponceLength];
  for (byte i = 0; i < _ResponceLength; i++)                    //Fill in whole array with all the recieved data
    ReturnValue[i] = Wire.read();
  if (ReturnValue[2] != crc8(ReturnValue, 2) || ReturnValue[5] != crc8(ReturnValue + 3, 2)) { //If the checksum is wrong (and the data is corupted)
    Return.Code = 7;
    return Return;
  }
  Return.Temperature = ((((ReturnValue[0] * 256.0) + ReturnValue[1]) * 175.0) / 65535.0) - 45.0; //Convert answer for the Temperature
  Return.Humidity = (((ReturnValue[3] * 256.0) + ReturnValue[4]) * 100.0) / 65535.0;             //Convert answer for the Humidity
  return Return;
}
