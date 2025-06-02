//Portable weather station made using Arduino Nano
// Following modules are used: 
//ST7735 LCD Display
//MQ-135 Air Quality Sensor
//DHT-11 Tempature and Humidity Sensor
//DS3231 RTC Modules (real-time-clock)

#include <Adafruit_GFX.h>    // Core graphics library for display
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <RTClib.h>          // Library for rtc module functionality
#include <DHT.h>            //library necessary for DHT-11 (tempature) Sensor

//contant declarations for necessary pins on display
#define DISPLAY_CS        10 
#define DISPLAY_RST       9 
#define DISPLAY_DC        8


Adafruit_ST7735 display = Adafruit_ST7735(DISPLAY_CS, DISPLAY_DC, DISPLAY_RST); //creates object for display with necessary pins

//variables to hold colors used in display themes
uint16_t BGCOLOR;
uint16_t LINECOLOR;
uint16_t PRIMARYTEXT;
uint16_t SECONDARYTEXT;

unsigned long refreshInterval = 1000; //time interval to refresh data on display in ms (every 1 second)
unsigned long startTime = 0;

RTC_DS3231 realTimeClock; //creates object for rtc(real time clock) functionality

int LEDPIN = 3, //pin declaration for LED on display
    year, //variable delcartions to hold information from RTC module
    month,
    day,
    hour,
    minute,
    second,
    convertedHour,
    currentDOW,
    dhtPin = 2; //digital pin for output of tempature and humidity sensor


String AmorPm, //string var to hold whether it's AM or PM
       currentDayOfWeek; //holds current day of week

String daysOfWeek[7] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"}; //array to hold days of week

//Boolean vars to detect change in time and icon change
bool dayChange = false,
     hourChange = false,
     minuteChange  = false,
     ampmChange = false,
     changeIcon = false,
     tempChange = false,
     humidityChange = false,
     airQualityChange = false;

//Vars to help us detect if day,month,year, day of the week and for icon change change (the -1 is to make sure if statment triggers causing a cycle of periodic checking )
int lastDay = -1,
    lastMonth = -1,
    lastYear = -1,
    lastHour = -1,
    lastDOW = -1;

//Vars that hold information from tempature sensor
float tempF,
      tempC;

int humidity;

DHT tempSen(dhtPin,DHT11); // creates object for tempature sensor

//Vars to help us detect change in tempature and humidity so it's updated only when needed (these act as a base case)
float lastTempF = -300,
      lastHumidity = -1;

//Vars to used for airquality module
int gasPin = A0,
    gasValue,
    lastGasValue = -1;

String airQuality;

//Vars to used for switch functionality, to toggle both light/dark mode
int switchPin = 4,
    initialStatus,
    lastStatus,
    currentStatus;

int currentIcon; //variable to track current icon

//Bit maps made using image2cpp to display icons on screen
const unsigned char morningIcon[] PROGMEM = { //morning Icon bitmap

  0x00, 0x00, 0x18, 0x0f, 0x00, 0x00, 0x1c, 0x7e, 0x00, 0x00, 0x0e, 0x70, 0x00, 0x00, 0x06, 0x00, 
	0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0xe3, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00, 0x73, 0x00, 
	0x00, 0x00, 0x73, 0x1e, 0x00, 0x00, 0x73, 0x3e, 0x00, 0x00, 0x73, 0x1e, 0x00, 0x00, 0x73, 0x00, 
	0x00, 0x00, 0xe3, 0x00, 0x00, 0x00, 0xe3, 0x00, 0x00, 0x01, 0xc6, 0x00, 0x00, 0x01, 0xc6, 0x40, 
	0x00, 0x01, 0x8e, 0x70, 0x00, 0x08, 0x1c, 0x7c, 0x00, 0x18, 0x3c, 0x1f, 0x00, 0x08, 0x78, 0x07, 
	0x80, 0x00, 0xf0, 0x00, 0xc0, 0x01, 0xe0, 0x00, 0xfc, 0x1f, 0xc6, 0x00, 0x3f, 0xfe, 0x07, 0x00, 
	0x0f, 0xf8, 0x03, 0x80, 0x00, 0x00, 0x01, 0x80, 0xc0, 0x01, 0x00, 0x00, 0xc0, 0x81, 0x80, 0x00, 
	0xc0, 0x81, 0x80, 0x00, 0x80, 0x80, 0xc0, 0x00, 0x80, 0x80, 0xc0, 0x00, 0x00, 0x80, 0xe0, 0x00

};

const unsigned char nightIcon[] PROGMEM = { //night-icon bitmap
  
  0x00, 0x00, 0x00, 0xb0, 0x00, 0x00, 0x60, 0x18, 0x00, 0x00, 0x91, 0x0c, 0x03, 0xc0, 0x10, 0x8c, 
	0x0f, 0xe0, 0xa3, 0x4e, 0x1c, 0x30, 0x40, 0x8e, 0x18, 0x10, 0x00, 0x0a, 0x18, 0x18, 0x00, 0x0b, 
	0x18, 0x18, 0x00, 0x0b, 0x0c, 0x38, 0x00, 0x0f, 0x0e, 0x70, 0x00, 0x07, 0x03, 0xe0, 0x00, 0x03, 
	0x00, 0x00, 0x00, 0x0b, 0x80, 0x00, 0x00, 0x17, 0x00, 0x00, 0x07, 0x1b, 0x80, 0x00, 0x0c, 0x9b, 
	0x00, 0x00, 0x10, 0x53, 0x00, 0x18, 0x10, 0x4a, 0x00, 0x38, 0x10, 0x86, 0x00, 0x20, 0x09, 0x86, 
	0xa8, 0x38, 0x06, 0x0c, 0x78, 0x00, 0x00, 0x0c, 0x38, 0x00, 0x00, 0x18, 0x10, 0x00, 0x60, 0x30, 
	0x80, 0x00, 0xe0, 0x60, 0xc0, 0x00, 0x60, 0xc0, 0xf0, 0x00, 0x03, 0x80, 0x78, 0x00, 0x0f, 0x00, 
	0x1e, 0x00, 0x7c, 0x00, 0x07, 0xff, 0xf0, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00
  
  };

const unsigned char middayIcon[] PROGMEM{ //mid-day icon bitmap

  0x00, 0x3c, 0xc0, 0x00, 0x00, 0x0d, 0x80, 0x00, 0x00, 0x02, 0x08, 0x00, 0x00, 0x01, 0x38, 0x00, 
	0x00, 0x01, 0xa0, 0x00, 0x00, 0x00, 0xc3, 0xc0, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x00, 0x70, 0x00, 
	0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x3c, 0x00, 
	0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x37, 0xe0, 0x00, 0x00, 0x20, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0xe0, 0x00, 0x01, 0x9c, 0x3f, 0x80, 0x02, 0x08, 0x10, 0x40, 
	0x1e, 0x00, 0x10, 0x40, 0x34, 0x00, 0x00, 0x78, 0x20, 0x00, 0x00, 0x8c, 0x00, 0x30, 0x00, 0x04, 
	0x00, 0xf8, 0x00, 0x0c, 0x00, 0x0c, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x0c, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

};

const unsigned char humidityIcon[] PROGMEM{ //humidity icon bitmap

  0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x60, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x90, 0x00, 0x01, 
	0x18, 0x00, 0x03, 0x0c, 0x00, 0x02, 0x04, 0x00, 0x04, 0x02, 0x00, 0x04, 0x02, 0x00, 0x09, 0xd1, 
	0x00, 0x09, 0x61, 0x00, 0x09, 0xe1, 0x00, 0x08, 0x79, 0x00, 0x08, 0xb9, 0x00, 0x0c, 0x13, 0x00, 
	0x04, 0x02, 0x00, 0x03, 0x0c, 0x00, 0x01, 0xf8, 0x00, 0x00, 0x00, 0x00

};

const unsigned char airQualityIcon[] PROGMEM{ //air-quality icon bitmap

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xf8, 0x00, 0x01, 0xff, 0xfe, 0x00, 
	0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x7f, 0xfb, 0x00, 0x03, 0xff, 0xff, 0x00, 
	0x03, 0xbb, 0x77, 0x00, 0x01, 0xbf, 0xf6, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x70, 0x38, 0x00, 
	0x00, 0x1f, 0xe0, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x08, 0x70, 0x00, 0x00, 0x08, 0x10, 0x00, 
	0x00, 0x0c, 0x5c, 0x00, 0x00, 0x0f, 0xcc, 0x00, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x78, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

};

const unsigned char bannerInitials[] PROGMEM{ //Banner with my first initial on it "B" bitmap

  0x00, 0x00, 0x7f, 0x00, 0x43, 0x00, 0x6b, 0x00, 0x6b, 0x00, 0x63, 0x00, 0x6d, 0x00, 0x41, 0x00, 
	0x7f, 0x00, 0x77, 0x00, 0x41, 0x00, 0x00, 0x00

};


void setup() { //only run once (upon starting up the device for the first time)

  pinMode(LEDPIN,OUTPUT); //LED pin gets set to output to control brightness of display
  analogWrite(LEDPIN,220); //controls brightness of display using PWM methood (0-255) values

  //upon pluggin in device initial theme will be set
  pinMode(switchPin,INPUT);
  initialStatus = digitalRead(switchPin);

  if(initialStatus == HIGH){ //if switch was left on (left) switch theme to light mode upon starting up

    BGCOLOR = display.color565(255, 255, 255);
    LINECOLOR = display.color565(50, 58, 69);
    PRIMARYTEXT = display.color565(26, 61, 117);
    SECONDARYTEXT = display.color565(58, 142, 101);

  }

  else{ //if switch was left off (right) switch theme to dark mode upon starting up

    BGCOLOR = display.color565(48, 52, 63);
    LINECOLOR = display.color565(95,173,86);
    PRIMARYTEXT = display.color565(57, 255, 20);
    SECONDARYTEXT = display.color565(21,138,0);

  }

  display.initR(INITR_REDTAB);        //initilize display
  display.setRotation(2);             //roates orientation screen of display
  drawBorderLines(initialStatus);     //draw borders using theme taken from initial status of switch

  realTimeClock.begin(); // initilizes rtc module
  //realTimeClock.adjust(DateTime(2025,5,29,19,58,0)); //sets time of module to May 22,2025 and 12:00:00 AM [setting time is based on 24 hours system] (upload once then comment out so RTC module keeps track of time)

  tempSen.begin(); //initilizes tempature sensor

}

void loop(){ //Main loop thats run repeatedly

  currentStatus = digitalRead(switchPin); //read current status of switch

  if (currentStatus != initialStatus){ //if the current position of the switch is not the same as the initial position (change theme)
    initialStatus = currentStatus;

    if(currentStatus == HIGH){ //if switch is on, set theme to light mode

      BGCOLOR = display.color565(255, 255, 255);
      LINECOLOR = display.color565(50, 58, 69);
      PRIMARYTEXT = display.color565(26, 61, 117);
      SECONDARYTEXT = display.color565(58, 142, 101);

    }

    else{ //if switch is off, set theme to dark mode

      BGCOLOR = display.color565(48, 52, 63);
      LINECOLOR = display.color565(95,173,86);
      PRIMARYTEXT = display.color565(57, 255, 20);
      SECONDARYTEXT = display.color565(21,138,0);

    }

    drawBorderLines(currentStatus); //Calls drawBorderLines() function to draw them depending on which switch case is true

  }

  
  unsigned long currentTime = millis(); //starts counting in ms as soon as program is run and stores it into var

  //if statement so an action is perferomed every second that passes (falls in line with the getTime())

  if (currentTime - startTime >= refreshInterval){
      getTime();        //calls getTime function for data
      getDayOfWeek();   //calls getDayOfWeek function for data
      getTemperature(); //calls getTempatrue function for data
      getAirQuality();  //calls getAirQuality function for data
  
      startTime = currentTime; //resets current time after one occurrence happens and repeats

      display.fillRect(87, 4, 22, 17,BGCOLOR); //draw a filled rectangle to change seconds repeatedly

      //to actually change the data you need to draw a rectangle repeatedly over the area your changing
      //standard size of font1:5 pixels wide, 7 tall
      //(x-start, y-start, width, height ,color)

  }

  drawCurrentIcon(currentIcon);//adds icon based off of current time of day to screen

  //Top Secion of screen// 

  //Adds current day of the week and time to screen
  display.setTextColor(SECONDARYTEXT);
  
  display.setCursor(3,9);
  display.print(currentDayOfWeek);

  display.setTextColor(PRIMARYTEXT);

  display.setTextSize(2);
  display.setCursor(27, 6);
  display.print(zeroAdder(convertedHour));

  display.setCursor(47,6);
  display.print(":");

  display.setCursor(56, 6);
  display.print(zeroAdder(minute));

  display.setCursor(78, 6);
  display.print(":");

  display.setCursor(87,6);
  display.print(zeroAdder(second));

  display.setTextSize(1);

  display.setTextColor(ST77XX_WHITE);

  display.setTextColor(SECONDARYTEXT);

  display.setCursor(112,12);
  display.print(AmorPm);

  //Middle portion of scrren//

  //Adds current date MONTH/DAY/YEAR to screen
  display.setCursor(68,30);
  display.print(zeroAdder(month));
  
  display.setCursor(84,30);
  display.print(zeroAdder(day));
  
  display.setCursor(100,30);
  display.print(zeroAdder(year));

  //Adds tempature to screen

  display.setTextSize(2);
  display.setTextColor(PRIMARYTEXT);


  display.setCursor(6, 61);
  display.print(tempF,1);

  display.setCursor(68,61);
  display.print(tempC,1);

  display.setTextSize(1);
  display.setTextColor(SECONDARYTEXT);


  display.drawCircle(56,62, 2,SECONDARYTEXT);
  display.setCursor(55, 67);
  display.print("F");

  display.drawCircle(118,62, 2,SECONDARYTEXT);
  display.setCursor(117, 67);
  display.print("C");

  //Bottom portion of screen//

  //Adds humidity to screen along with its icon
  display.setCursor(4, 83);
  display.print("Humidity");

  display.setTextSize(2);
  display.setTextColor(PRIMARYTEXT);

  display.setCursor(8, 100);
  display.print(humidity,1);

  display.setTextColor(SECONDARYTEXT);
  display.setTextSize(1);


  display.drawBitmap(30,96,humidityIcon,20,20,SECONDARYTEXT); //Adds humidity icon to screen

  //Adds air quality to scrren along with its icon
  display.setCursor(59, 83);
  display.print("Air-Quality");

  display.setTextColor(PRIMARYTEXT);

  display.setCursor(77,102);
  display.print("feels:");

  //modifies placement of airquality depending on condition met, purely for eligibility
  if (airQuality == "Moderate"){
    display.setCursor(77,111);
  }

  else{
      display.setCursor(82, 111);
  }
  display.print(airQuality);

  display.setTextColor(ST77XX_WHITE);
  display.setTextSize(1);

  display.drawBitmap(52,86,airQualityIcon,30,30,SECONDARYTEXT); //Adds air-quality icon to scrren



}

//drawBorderLines function takes in the current switch status as a input (where it gets its theme from) and draws all lines based off that
void drawBorderLines(bool switchStatus){

  //Border Lines for display
  //shorten all borders by 1 pixel to show edges
  display.fillScreen(BGCOLOR);   //adds a black background to display

  display.drawLine(1,1,126,1,LINECOLOR); //top line
  display.drawLine(1,126,126,126,LINECOLOR); //bottom line
  display.drawLine(1,1,1,126,LINECOLOR); //left line
  display.drawLine(126,1,126,126,LINECOLOR); //right line

  display.drawLine(1,25,126,25,LINECOLOR);
  display.drawLine(22,1,22,25,LINECOLOR);
  display.drawLine(63,25,63,40,LINECOLOR);
  display.drawLine(63,40,126,40,LINECOLOR);

  display.drawLine(81,30,81,36,LINECOLOR);
  display.drawLine(97,30,97,36,LINECOLOR);

  display.drawLine(63,61,63,74,LINECOLOR);
  display.drawLine(1,81,126,81,LINECOLOR);

  display.drawLine(55,81,55,126,LINECOLOR);

  display.drawBitmap(116,40,bannerInitials,12,12,PRIMARYTEXT); //draws banner with my initial on it


}


//getTime function retreives all necessary time related data from rtc module and returns it for use
int getTime(){

  DateTime information = realTimeClock.now(); //retreives current time from rtc module and stores it into information object

  //retrieves and stores necessary imformation from information object using built in functions from RTC library
  year = information.year();
  month = information.month();
  day = information.day();
  hour = information.hour();
  minute = information.minute();
  second = information.second();
  currentDOW = information.dayOfTheWeek();


  if(hour >= 12){ //if the current hour (based on 24 hour system is >=12) set the clock to PM

    AmorPm = "PM";
    ampmChange = true;

    if (hour > 12){ //converts the hour from the 24 hour system to a 12 the 12 hour system and stores it into variable
      convertedHour = (hour - 12);
      
    }

    else if(hour == 12){ //special case when the clock hits the 12'th hour meaning it's 12pm
      convertedHour = 12;
      
    }

  }

  else{ //if the current hour (based on 24 hour system) is < 12 set the clock to AM
    AmorPm = "AM";
    ampmChange = true;
    

    if (hour == 0){ //converts the 24 hour system of 0:00:00 to 12 hour system (12:00 AM)
      convertedHour = 12;

    }

    else{ //if given hour is in between 0 and 12 just leave it as is
      convertedHour = hour;
    }

  }
    

    if (ampmChange == true){
      display.fillRect(111,12,10,7,BGCOLOR);
    }

    //detects change in hour to optimize system (no unecessary drawing of pixles,only done when needed))
    if(minute == 0 && second == 0){

      hourChange = true;

        if (hourChange == true){
          display.fillRect(27, 4, 24, 17,BGCOLOR);
        }

    }
    
    //detects change in minute to optimize system (no unecessary drawing of pixles,only done when needed)
    if (second == 0){
      minuteChange = true;

      if (minuteChange == true){

        display.fillRect(56,4,24,17,BGCOLOR);

      }
    }

    // if statments that check if day, month, and year change,upon first run lastday = -1 so this runs automatically for the first run
    // it then sets lastday = to current day and draw's a box over it to replace because of the condtion and process repeates everytime function is called

    if(day != lastDay){
      lastDay = day;

      display.fillRect(83,30,12,7,BGCOLOR);
      display.setCursor(84,30);
      display.print(zeroAdder(day));
      
    }

    if(month != lastMonth){
      lastMonth = month;

      display.fillRect(67,30,12,7,BGCOLOR);
      display.setCursor(68,30);
      display.print(zeroAdder(month));

    }

    if(year != lastYear){
      lastYear = year;

      display.fillRect(100,30,24,7,BGCOLOR); 
      display.setCursor(100,30);
      display.print(zeroAdder(year));

    }



    //if statments to check if hour changes repeatedly, has a brute force initial and after that repeats checking
    //if a certain hour threshold is passed then changeIcon is triggered to true and icon is changes [Done this way to reduce flickering on SPI display]

    if (hour != lastHour){

      lastHour = hour;

      if (hour >= 6 && hour <= 15){ //Time parameters for icon to be morning
      changeIcon = true;
      currentIcon = 0;

        if(changeIcon == true){

          display.fillRect(2, 26, 32, 32, BGCOLOR);
          display.drawBitmap(2,26,morningIcon,32,32,PRIMARYTEXT);

        }
      }

      if (hour >=16 && hour <19){ //time parameters for icon to be midday
        changeIcon = true;
        currentIcon = 1;

        if(changeIcon == true){

          display.fillRect(2, 26, 32, 32, BGCOLOR);
          display.drawBitmap(2,26,middayIcon,32,32,PRIMARYTEXT);

        }
      }

      if (hour >=19 || hour < 6) { //accounts for all other hours of the day which is night time
        changeIcon = true;
        currentIcon = 2;

        if (changeIcon == true){

          display.fillRect(2, 26, 32, 32, BGCOLOR);
          display.drawBitmap(2,26,nightIcon,32,32,PRIMARYTEXT);

        }

      }
  }

  return(year,month,day,hour,minute,second,currentDOW);

}

//function to get current day of week
String getDayOfWeek(){

  if (currentDOW != lastDOW){ //detects a change in the day of week

    lastDOW = currentDOW;
    display.fillRect(3, 9, 17, 7,BGCOLOR); //covers previous day of the week and prints new one over it

  }

  currentDayOfWeek = daysOfWeek[currentDOW]; //stores day of week into string using array and index of it

  return currentDayOfWeek;

}


//function that retrieves tempature and humidty data from DHT-11 and returns it for use
int getTemperature(){

  tempF = tempSen.readTemperature(true); //reads and stores the tempature in degrees Farenheit
  tempC = tempSen.readTemperature();    //reads and stores the tempature in degrees Celcius
  humidity = tempSen.readHumidity();    //reads and stores the humidity


  //if statment to check if tempature has changed, done this way to optimize and minimize drawing done
  //only checks if farenheit changes because both cel and far and directly proportional so no need to check both
  if (tempF != lastTempF){

    lastTempF = tempF;
    tempChange = true;

    if (tempChange == true){

      display.setTextSize(2);
      display.setTextColor(PRIMARYTEXT);

      display.fillRect(6, 61, 47, 15, BGCOLOR);  //redraws a black box over old data and replaces it with new data for both farenheit and celcius
      display.setCursor(6, 61);
      display.print(tempF,1);

      display.fillRect(68, 61, 47, 15, BGCOLOR);  
      display.setCursor(68,61);
      display.print(tempC,1);

      display.setTextColor(ST77XX_WHITE);
      display.setTextSize(1);

    }

  }

  if (humidity != lastHumidity){

    lastHumidity = humidity;
    humidityChange = true;

    if(humidityChange == true){

      display.setTextSize(2);
      display.setTextColor(PRIMARYTEXT);

      display.fillRect(8, 100, 24, 16,BGCOLOR);
      display.setCursor(8, 100);
      display.print(humidity,1);

      display.setTextColor(ST77XX_WHITE);
      display.setTextSize(1);


    }

  }

  return(tempF,tempC,humidity);

}

//function that retrieves air quality from MQ-135 and returns it to user
String getAirQuality(){

  gasValue = analogRead(gasPin); //Reads analog pin connected to sensor (0-1023); clean air = lower analog value, polluted air = high analog value


  if (gasValue != lastGasValue){ //acts as a base case to detect change in gasValue

    lastGasValue = gasValue;

    display.setTextColor(PRIMARYTEXT);

    if (gasValue <= 100){
      airQuality = "Good";

      display.fillRect(76, 111, 48, 8, BGCOLOR);
      display.setCursor(82, 111);
      display.print(airQuality);
      
    }

    else if (gasValue <=250){
      airQuality = "Moderate";

      display.fillRect(76, 111, 49, 8, BGCOLOR);
      display.setCursor(77, 111);
      display.print(airQuality);
      
    
    }

    else{
      airQuality = "Poor";

      display.fillRect(76, 111, 48, 8, BGCOLOR);        
      display.setCursor(82, 111);
      display.print(airQuality);

    }


  }
  
  display.setTextColor(ST77XX_WHITE);
  return(airQuality);

}

//drawCurrentIcon function, takes the input given from the getTime function and draws the needed icon based off of it
void drawCurrentIcon(int currentIcon){
  if(currentIcon == 0){
    display.drawBitmap(2,26,morningIcon,32,32,PRIMARYTEXT);
  }

  else if (currentIcon == 1){

    display.drawBitmap(2,26,middayIcon,32,32,PRIMARYTEXT);

  }

  else{

    display.drawBitmap(2,26,nightIcon,32,32,PRIMARYTEXT);
    
  }
}

//function that adds a 0 infront of a number if less than 10 (purely for eligibilty and neatness)
int zeroAdder(int x){

  if (x < 10){ //if given number is < 10 it prints a zero on the same line as it
    display.print("0");
  }
  return(x); //returns x with the 0 now in front

}
