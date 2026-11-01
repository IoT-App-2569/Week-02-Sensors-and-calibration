![[Pasted image 20260707081107.png]]


## Summary of all ESP32 GPIO pins 

Summary of all ESP32 GPIO pins

|GPIO|INPUT|OUTPUT|Comments|
|---|---|---|---|
|0|YES (Internal pull-up)|YES|Must be at 0V during the FLASH|
|1 (TX0)|NO|YES|UART communication with the PC|
|2|YES (Internal pull-down)|YES|Must be at 0V during the FLASH|
|3 (RX0)|YES|NO|UART communication with the PC|
|4|YES|YES||
|5|YES|YES||
|6|NO|NO|Connected to the internal flash|
|7|NO|NO|Connected to the internal flash|
|8|NO|NO|Connected to the internal flash|
|9|NO|NO|Connected to the internal flash|
|10|NO|NO|Connected to the internal flash|
|11|NO|NO|Connected to the internal flash|
|12 (MTDI)|YES (Internal pull-down)|YES|Must be at 0V during the BOOT|
|13|YES|YES||
|14|YES|YES||
|15 (MTDO)|YES (Internal pull-up)|YES|Startup log if at 3.3V|
|16|YES|YES|Not available on WROVER|
|17|YES|YES|Not available on WROVER|
|18|YES|YES||
|19|YES|YES||
|21|YES|YES||
|22|YES|YES||
|23|YES|YES||
|25|YES|YES||
|26|YES|YES||
|27|YES|YES||
|32|YES|YES||
|33|YES|YES||
|34|YES|NO|No internal pull-up/pull-down|
|35|YES|NO|No internal pull-up/pull-down|
|36 (VP)|YES|NO|No internal pull-up/pull-down|
|39 (VN)|YES|NO|No internal pull-up/pull-down|
|EN|NO|NO|Connected to the EN button (ESP32 Reset)|