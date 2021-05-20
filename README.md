# Arduino-log-Sht3x-to-Domoticz
A way to add 2 Sensirion SHT3x (RH/T) Digital Humidity Sensor(s) to Domoticz

[Note that HUM seems to be an int, and will truncate decimals](https://github.com/domoticz/domoticz/blob/6fa3ed25990d46a70d0fc149d3ed5b7c44790a6b/main/EventSystem.cpp#L670
), this firmware will round HUMIDITY to the nearest whole number.

# What you need
- An [Domoticz](https://www.domoticz.com/) hub (and its IP ofc) to send data to
- An dummy ["Temp+hum"](https://www.domoticz.com/wiki/Dummy_for_virtual_Switches#Temperature.2Fhumidity) device in Domoticz and its IDX
- [ESP32](https://dl.espressif.com/dl/package_esp32_index.json)
- SHT3x sensor board

# How it works
1. Make sure the hub is already set-up and working, and you know it's IP.
2. Flash the Arduino sketch to the PCB, or simply flash the released BIN file
3. You can either setup the settings like SSID and passwords and such in the code, but you can also just upload the sketch and power it on and set it up: It will go into APMODE (since it can’t connect to WIFI) connect to it and go to it's IP (192.168.4.1)
4. You will get a window with 'SSID Wi-Fi name', 'Wi-Fi password', 'the IP of the Hub like "192.168.4.1"', 'hup port, use default 80 when unsure', and such. Change or fill these in and submit.

The page [SHT.local/test](http://SHT.local/test) can be used to manual force data to be pulled (and the feedback is shown)

# LED feedback
There a lot of blinking patterns, but I tried to list them all here
- (Main LED) Turns on on boot, and turn of when done
- (Main LED) will blink every 100ms to show we are not connected (APMODE)
- (Main LED) will blink every 500ms to show we are trying to connect
- (Main LED) Turns when collection the data and reporting it to Domoticz, and turn of when done

# Soft settings
There are multiple soft settings, these are settings that can be changed after the sketch has been uploaded, but are still saved so they can be restored after a reboot.
The most up-to-date values can be found in the top of the [WiFiManagerBefore.h](Arduino/WiFiManagerBefore.h) source file, and can only be set in [SHT.local/ip](http://SHT.local/ip).
Note that the character " and TAB (EEPROM_Seperator) cannot be used, these will be replaced with ' and SPACE respectively. Leave black to skip updating these, use spaces ' ' to clear the values
- **Domoticz IP** The Domoticz ip to connect to to sent the data to (and the log)
- **Domoticz port** The Domoticz port to connect to on the given ip
- **I2C address0 DEC!** The I2C address of the first RH sensor, its in DEC (HEX 0x45 = DEC 68), leave blank to skip (One time warning on startup)
- **Dom IDX0 (Temp+hum)** the IDX of the "Temp+hum" dummy sensor in Domoticz to write the data to
- **I2C address1 DEC!** Optional second sensor
- **Dom IDX1 (Temp+hum)** Optional second sensor

# OTA (Over The Air update)
This page can be accesed on [SHT.local/ota](http://SHT.local/ota) (or 'IP/ota') and enables you to update firmware over WiFi.
On this page is a 'choose file' button where the new version can be selected. Make sure the right, compatible, most updated file is selected ("Smart_clock.bin"). This can be downloaded from [releases](https://github.com/jellewie/Arduino-smart-home-switch/releases). 
After which the 'Upload' button needs to be press for the update process to begin, the unit will automatically update and reboot afterwards.
Note that [SoftSettings](#soft-settings) are preserved.
