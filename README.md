"ANOTHER.ino" code is having a problem.
Temperature values are not getting displayed properly as we want on 7-Segment LED.
And i am trying to resolve that issue.

Data flow :
DS18B20 sends the temperature value to ESP32 which is then converted to integer value and displayed on TM1638 and LCD
To print the sensor value we use multiplication , divison and modulus of integer sensor value.
