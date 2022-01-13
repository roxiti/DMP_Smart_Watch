int sensorPin = A6;
int buzzer = 9;
int ledoutput = 0;
int ThresHOLD = 400;
int button = 2;


#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


//libraries needed for the real time clock
#include <TimeLib.h>
#include <TimerOne.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>


#include <ArduinoHttpClient.h>

#define DEBUG true
#define PIN      6
#define NUMPIXELS 30

ThreeWire myWire(20, 21, 8); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int array_brow_right_1[4] = { 0 , 14 , 23};
int array_brow_right_2[4] = { 1 , 15 , 24};

int array_brow_left_1[4] = { 6 , 10 , 25 };
int array_brow_left_2[4] = { 7 , 11 , 26};



volatile int alarm_on_int=0;
#define countof(a) (sizeof(a) / sizeof(a[0]))
char hours[2], minutes[2];

char Time[] = "TIME:00:00:00";
char Date[] = "DATE:00/00/2000";
byte last_second, second_, minute_, hour_, day_, month_;
int year_;


//In order to be able to make the buzzer and the Neopixel run concurenttly I choose to structure them in classes and 
//define parameters like onTime and offTime in their class. They are important to kep in mind because they establish 
// the time of running and change their state accordingly. I choose to work with milis() in order to be able to create the 
//effect o simulataneity. For example if the buzzer start after 100 millis and the neopixels after 150 millis , then the difference 
//in time would be so little for the human ear and eye, that it woul create the illusion that they run in the same time.
class Neopixel
{

    //class variables
    int pixel_nr;           //first pixel to turn on
    int pixel_nr2;          //second pixel to turn on
    int Ledcolor;           //set the color of the pixel
    int val;                //value used for the second update function in order to create different colors of the pixels
    long onTime;           // time the Neopixels are on
    long offTime;         // time the Neopixels are off

    //maintina the current state
    unsigned long previousState;

  public:
    Neopixel (int pix, int pix2, int color , long on, long off)         //constructor of the Neopixel
    {
      pixel_nr = pix;
      pixel_nr2 = pix2;
      Ledcolor = color;
      val = 50;

      onTime = on;
      offTime = off;

      previousState = 0;              // set previous state to 0
    }

    void Update()       //fucntion for the angry neopixel behaviour. The leds turn red and form a little brow
    {
      unsigned long currentState = millis();        //the millis function will establish the current state

      if ((Ledcolor == 255 ) && (currentState - previousState >= onTime))           //if leds are turned on, and the onTime has passed, now it's time to change their state
      {
        Ledcolor = 0;                                                               //turn off led 
        previousState = currentState;                                               //previousStae is now currentState
        pixels.setPixelColor(pixel_nr, pixels.Color(Ledcolor, 0, 0));
        pixels.setPixelColor((pixel_nr + 1), pixels.Color(Ledcolor, 0, 0));
        pixels.setPixelColor( pixel_nr2, pixels.Color(Ledcolor, 0, 0));
        pixels.setPixelColor(( pixel_nr2 + 1), pixels.Color(Ledcolor, 0, 0));
        pixels.show();
      }
      if ((Ledcolor == 0 ) && (currentState - previousState >= offTime))       //if leds are turned off, and the offTime has passed, now it's time to change their state
      {
        Ledcolor = 255;                                                       //turn on led 
        previousState = currentState;
        pixels.setPixelColor(pixel_nr, pixels.Color(Ledcolor, 0, 0));
        pixels.setPixelColor((pixel_nr + 1), pixels.Color(Ledcolor, 0, 0));
        pixels.setPixelColor( pixel_nr2, pixels.Color(Ledcolor, 0, 0));
        pixels.setPixelColor(( pixel_nr2 + 1), pixels.Color(Ledcolor, 0, 0));
        pixels.show();

      }


    }

    void Update2()   //fucntion for the normal neopixel behaviour. The leds turn in different colors and turn on in a pseudo random manner
    {
      unsigned long currentState = millis();                       //the millis function will establish the current state

     
      if ((Ledcolor > 0) && (currentState - previousState >= onTime))       //if leds are turned on, and the onTime has passed, now it's time to change their state
      {
        Ledcolor = 0;
        previousState = currentState;

        pixels.setPixelColor((pixel_nr), pixels.Color(0, 0, 0));                        //turn off pixel number 1 
        pixels.setPixelColor( pixel_nr2, pixels.Color(Ledcolor, 0, 0));                  //turn off pixel number 2
 
        pixels.show();
      }
      //    }

      if ((Ledcolor == 0 ) && (currentState - previousState >= offTime))       //if leds are turned off, and the offTime has passed, now it's time to change their state
      {
        //        for ( int i = 0;i < NUMPIXELS - 1; i++)
        //  {

        //Ledcolor = 64;                                                                          //set value to 64 in order to have a quarter of their full brightness in order to make it more pleasing to the human eye

        pixels.setPixelColor((pixel_nr ), pixels.Color( val , val - 20, Ledcolor - val));          //turn on pixel number 1  -  choose different computation for val in order to create different colors
        pixels.setPixelColor( pixel_nr2, pixels.Color( val , val / 2, Ledcolor / 2));              //turn on pixel number 2  -choose different computation for val in order to create different colors

        val += 20;

        if (val > 64)
          val = 20;
  
        previousState = currentState;                                           //previous state the current state in order to compute it the next time milis function is called 

        pixels.show();

        // }
        if (Ledcolor < 0 )
          Ledcolor = 0;


      }

    } //end Update2 function

    void Reset()                                                                //reset function in order to turn all the leds off once their program is over
    { Ledcolor = 0;
      pixels.setPixelColor(pixel_nr, pixels.Color(Ledcolor, 0, 0));
      pixels.setPixelColor((pixel_nr + 1), pixels.Color(Ledcolor, 0, 0));
      pixels.setPixelColor( pixel_nr2, pixels.Color(Ledcolor, 0, 0));
      pixels.setPixelColor(( pixel_nr2 + 1), pixels.Color(Ledcolor, 0, 0));
      previousState = 0;
      pixels.show();
    }

};


class Buzzer                                            //class definition for Buzzer
{
    //class variables
    int buzzerPin;                                        //pin of buzzer 
    long onTime;                                          // on time 
    long offTime;                                         // off time 

    //maintaint the current state
    int buzzerState;                                      //variables to keep in mind the current and previous state
    unsigned long previousState;

  public:
    Buzzer (int pin, int on , int off)                    //constructor for the Buzzer 
    {
      buzzerPin = pin;
      pinMode (buzzerPin, OUTPUT);

      onTime = on;
      offTime = off;

      buzzerState = HIGH;                               //set it to HIGH, because this buzzer is low triggered and i don't want it to start ringing
      previousState = 0;

    }

    void Update()                                     //update function that implements the main behaviour of the buzzer
    {
      unsigned long currentState = millis();          //put in the current state the result given by the millis function.

      if ((buzzerState == LOW ) && (currentState - previousState >= onTime))        // if the buzzer is on ( is low triggeref) and it's on time has passed , we change it's state and turn it off.
      {
        buzzerState = HIGH;
        previousState = currentState;
        digitalWrite(buzzerPin, buzzerState);
      }
      else if ((buzzerState == HIGH) && (currentState - previousState >= offTime))  // if the buzzer is off ( is low triggeref) and it's off time has passed , we change it's state and turn it off.
      {

        buzzerState = LOW;
        previousState = currentState;
        digitalWrite(buzzerPin, buzzerState);

      }

    }

    void Reset()                                                                   //reset function for the Buzzer
    {
      buzzerState = HIGH;
      previousState = 0;
      digitalWrite(buzzerPin, buzzerState);
    }
};


Neopixel pixel_1(0, 6, 0, 150, 300);                                               //first three object are used for the angy clock behaviour
Neopixel pixel_2(14, 10, 0, 160, 400);
Neopixel pixel_3(23, 25, 0, 170, 500);

Neopixel pixel_4(0, 28, 63, 300, 300);                                             //the next 9 objects are used for the normal clock behaviour
Neopixel pixel_5(1, 27, 60, 350, 400);
Neopixel pixel_6(2, 26, 59, 400, 500);
Neopixel pixel_7(3, 25, 58, 450, 550);
Neopixel pixel_8(5, 24, 52, 500, 600);
Neopixel pixel_9(5, 24, 48, 500, 600);
Neopixel pixel_10(6, 23, 44, 500, 600);
Neopixel pixel_11(7, 22, 40, 550, 700);
Neopixel pixel_12(8, 21, 39, 600, 800);


Buzzer buzz(9, 50, 100);                                                          //buzzer object
Buzzer buzz2(9, 300, 500);
//Buzzer_Song buzz2(9,100,200,500);

void setup() {
  // put your setup code here, to run once:
 // Serial.begin (9600);
  pinMode(buzzer, OUTPUT);                                                      //setup pinmode and asssign value 
  digitalWrite(buzzer, HIGH);                                                   

  Serial.begin(115200);                                                         //set baoud rate to the serial communication between the board and the wifi module 
  Serial1.begin(115200);

  Serial.print("compiled: ");                                                   //print the time of compilation given by the Real time clock respecting the format 
  Serial.print(__DATE__);
  Serial.println(__TIME__);

  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

    if (!Rtc.IsDateTimeValid())                                                //verification done for the RTC and some updates in order to synchronize with the current time 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }

   pinMode(2 ,INPUT);                                                                                    //setup pinMode for the button used for interrupt
   digitalWrite(2, HIGH);                                                                               // set it to high, beacuse the pushbutton has a pulldown resistor

   attachInterrupt(digitalPinToInterrupt(2),buttonInterrupt,RISING);                                    //attach an interrupt to pin2 where the button is connected. The pin 2 is INT0. 

  
  
  pixels.begin();                                                                                       //start the neopixels

  

  sendData("AT+RST\r\n", 2000, false); // reset module                                                   //setup command for the wifi module
  sendData("AT+CWMODE=3\r\n", 1000, false); // configure as
  //access point
  sendData("AT+CWLAP\r\n", 7000, false);
  sendData("AT+CIFSR\r\n", 1000, DEBUG); // get ip address
  sendData("AT+CWSAP?\r\n", 2000, DEBUG); // get SSID info
  sendData("AT+CIPMUX=1\r\n", 1000, false); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n", 1000, false); // turn on

}

int angry_clock = 0, cnt = 0;                                                                                     //variables to eep track of the change of the behaviour
unsigned long previousState = 0;
int offTime = 10000;
int onTime = 100;
volatile int alarm_on = 0;


void loop() {
  // put your main code here, to run repeatedly:
  int value = analogRead(sensorPin);
  

    unsigned long currentState = millis();



  if ( alarm_on == 1) {                                                                 //if alarm is on call the update function for the buzzer adn the 9 objects created previously. If 
                                                                                      //the alarm enters in the angry clock mode only te buzzer and the three object are called
    buzz2.Update();
    if (angry_clock == 1 )
    {
      pixel_1.Update();
      pixel_2.Update();
      pixel_3.Update();
      buzz.Update();

    }
    else {
      pixel_4.Update2();
      pixel_5.Update2();
      pixel_6.Update2();
      pixel_7.Update2();
      pixel_8.Update2();
      pixel_9.Update2();
      pixel_10.Update2();
      pixel_11.Update2();
      pixel_12.Update2();
    }




    if (cnt < 8000 )                                                                                          //count that keeps the alarm clock stuck in angry clock mode for a while
      {cnt ++; 
     //  Serial.println (cnt);
     }
    else
    { cnt = 0;
      angry_clock = 0;

      pixel_1.Reset();
      pixel_2.Reset();
      pixel_3.Reset();
      buzz.Reset();

    }


    if (value >= ThresHOLD)                                                                                   //piezoelectric sensor is used to see the pressure the user applies on the sensor in order to hit snooze. If is larger than the 
    {                                                                                                           //reference thresholf, then the alram enters in the angry clock behaviour
      // digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      Serial.println("KNOCK!");
      angry_clock = 1;

    }

    pixels.clear();                                                                                 //reset neopixels


  }
  else { buzz.Reset(); //buzz2.Reset();                                                             //reset all objects after alarm if off
  
  
  
       pixel_4.Reset();
      pixel_5.Reset();
      pixel_6.Reset();
      pixel_7.Reset();
      pixel_8.Reset();
      pixel_9.Reset();
      pixel_10.Reset();
      pixel_11.Reset();
      pixel_12.Reset();
  }
  wifi_clock_function();
  
}
void wifi_clock_function()                                                                                //function used for the wifi connection and interpret the input given from the website.
{

  RtcDateTime dt = Rtc.GetDateTime();                                                                   //get current time

   if (dt.IsValid())
{

    Time[12] = dt.Second() % 10 + 48;
    Time[11] = dt.Second() / 10 + 48;

    Time[9]  = dt.Minute() % 10 + 48;
    Time[8]  = dt.Minute() / 10 + 48;


    Time[6]  = dt.Hour()   % 10 + 48;
    Time[5]  = dt.Hour()   / 10 + 48;

}                                                                                                     //put it in proper order in a string in order to be able to compare it with the webiste input



if ( hours[0] == Time[5] &&  hours[1] == Time[6] &&  minutes[0] == Time[8] &&  minutes[1] == Time[9])             //if they are the same , ring the alarm
 { if( alarm_on_int == 0 )                                                                                        //a control variable is used in order to check if an interrupt was done while ringing the alarm. 
       alarm_on = 1;                                                                                              //If no ring till the minute passes. If the interrupt was called, then turn off the alarm, even though 
   else alarm_on = 0;                                                                                             //the minute and hours of the input are the same with the one's given by the RTC clock
}
else  { alarm_on_int = 0;}

 if (Serial1.available()) {                                                                                       //wifi interpretation

      // read HTTP request
        char c = Serial1.read();



                     if (Serial1.find("+IPD,")) {
                       delay(500);
                       int connectionId = Serial1.read() - 48; // read()

                        String header = "HTTP/1.1 200 OK\r\nContent-type:text/html\r\nConnection: close\r\n";                         //used a header in roder to be abel to run it on iphone.Because apple phones have a rigouros security polices.
                        String webclient = "\r\n<!DOCTYPE html>\r\n<html>\r\n";


                        webclient += "<p>\r\n When you set your alarm. Respect the format (ex. 03:00) </p>\r\n";                              //the user should respect the format if he wants teh data to be interpreted correctly

                        webclient += "<form action=\"/get\"> Alarm: <input type=\"text\" name=\"alarm\"><input type=\"submit\" value=\"Submit\">\r\n</form>";





                         if ( hours[0] == Time[5] &&  hours[1] == Time[6] &&  minutes[0] == Time[8] &&  minutes[1] == Time[9])                  //teh website shows the alarm is ringing
                                       {// Serial.println("alarm");
                                           webclient += "<p>\r\n Alarm is ringing \r\n</p>\r\n ";
                                       }


                 
                       String cipSend = "AT+CIPSEND=";
                       cipSend += connectionId;
                       cipSend += ",";


                       cipSend += webclient.length() + header.length();
                       cipSend += "\r\n";


                       sendData(cipSend, 100, DEBUG);
                       sendData(header, 150, DEBUG);
                       sendData(webclient, 400, DEBUG);

                       String closeCommand = "AT+CIPCLOSE=";
                       closeCommand += connectionId; // append connection id
                       closeCommand += "\r\n";
                       sendData(closeCommand, 300, DEBUG);
                     }




             }

}



String sendData(String command, const int timeout, boolean debug)                                       //sendData function used for the interpretation of serial communcation between board and wifi module.
{
  String response = "";
  Serial1.print(command); // send command to the esp8266
  long int time = millis();
  while ((time + timeout) > millis()) {
    while (Serial1.available()) {
      char c = Serial1.read(); // read next char
      response += c;
    }
  }
  if (response.indexOf("GET /get?alarm=") >= 0)                                                       //check the hhtp response to see if the user has submited an input and parse it. After parse sootre it in some global variables 
  { int index = response.indexOf("GET /get?alarm=");                                                  //in order to be able to compare it with the RTC values each time the loops executes
    //Serial.println("AICI");


    Serial.println(response[ index + 15]);
    Serial.println(response[ index + 16]);
    Serial.println(response[ index + 20]);
    Serial.println(response[ index + 21]);


    hours[0] =  response[index + 15];
    hours[1] =  response[index + 16];
    minutes[0] =  response[ index + 20];
    minutes[1] =  response[index + 21];


    String s = hours;
    s += minutes;


  }

  if (debug) {
    Serial.print(response);
  }
  return response;
}

void buttonInterrupt()                                                                                  //intrerrupt function that changes the state to turn off alarm and changes the control variable
{

  alarm_on_int = 1;
  alarm_on = 0;

  
}
