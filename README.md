# Portable Weather Station (prototype)
An Arduino Nano based embedded system for real time weather monitoring with responsive UI
</br>
create by Bryan Diaz | Focus: Embedded systems and hardware


# Project Overview
Fully functional prototype of a portable weather station that integrates the use of multiple sensors, real time data visualization, and user interaction.
</br>
This system is designed to be both efficient in performance along with visual output and to be compact and portable.
</br>
I built this to explore various concepts of embedded systems, I wanted more expirence in data visualization on a display and power efficient programming on a microcontroller.
</br>

This system displays and updates weather and time related information in real time using a flicker free responsive UI that includes dynamic weather icons that correspond to the current time of day
along with a toggle switch that the user can flip to toggle a light and dark mode to better suit the current enviorment of the user.
</br>
</br>
TFT displays tend to run into the issue of flickering whenever updating data quickly on a slower microcontroller like the Ardunio Nano, when working on thsi project I ran into the issue of the display constantly flickering when updating the data
</br>
To get past this issue I came up with implementing a condtion system to only update data when detecting change in sensor values instead of updating it constantly in the loop function which better optimized the system as well

#Technologies and Components
Hardware Related:
+ Perfboard assembly
+ Soldering
+ Arduino Nano
+ MQ-135 (air quality sensor)
+ DHT-11 (tempature and humidity sensor)
+ DS3231 real time clock module
+ ST7735 1.44" tft lcd display (operates using SPI)
+ Toggle Switch (to toggle between theme)
+ Power bank (5000 MaH)
+ 6 in USB to Mini Usb cable

Software Related:
+ Arduino IDE
+ C++ (custom functions for optimizing display logic (only updating stuff when needed), managing toggle switch states, and for drawing UI upon start of system to correspond with initial switch state)
+ Various libraries for compenets:
'+ Adafruit GFX








Portable weather station prototype made using Arduino Nano along with
- MQ-135 air quality sensor
- DHT-11 tempature and humidity sensor
- DS3231 real time clock module
- ST7735 1.44" tft lcd display (operates using SPI)

Weather station displays, current Time (AM/PM), day of the week, the current date in (month/day/year) format along with
tempature in both degrees farenheit and celcius. Also includes humidity and airquality readings with
time cordinated icons.

Has a switch that can change the theme of the weather station to better suit the enviorment of the user
switch facing left = light/day theme; switch facing right = dark/night theme
