# Portable Weather Station (prototype)
An Arduino Nano based embedded system for real time weather monitoring with responsive UI
</br>
create by Bryan Diaz | Focus: Embedded systems and hardware


# Project Overview
Fully functional prototype of a portable weather station that integrates the use of multiple sensors, real time data visualization, and user interaction.
</br>
This system is designed to be both efficient in performance along with visual output and to be compact and portable
</br>
I built this to explore various concepts of embedded systems, I wanted more expirence in data visualization on a display and power efficient programming on a microcontroller






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
