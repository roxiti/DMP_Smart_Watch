int sensorPin = A6;
int buzzer = 9;
int ledoutput =0;
int ThresHOLD = 400;
int button =2;

#include "pitches.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

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


Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int array_brow_right_1[4] = { 0 , 14 , 23};
int array_brow_right_2[4] = { 1 , 15 , 24};

int array_brow_left_1[4] = { 6 , 10 , 25 };
int array_brow_left_2[4] = { 7 , 11 , 26};


class Neopixel 
{

//class variables
 int pixel_nr;
 int pixel_nr2;
 int Ledcolor;
 int val;
 long onTime;
 long offTime;

//maintina the currnet state
 unsigned long previousState;

  public:
   Neopixel (int pix, int pix2, int color ,long on, long off)
   {
     pixel_nr = pix;
     pixel_nr2 = pix2;
     Ledcolor = color;
     val = 50;
     
     onTime = on;
     offTime = off;

     previousState = 0;
   }

   void Update()
   {
      unsigned long currentState = millis();

      if((Ledcolor == 255 ) && (currentState - previousState >= onTime))
      {
        Ledcolor = 0;
        previousState = currentState;
        pixels.setPixelColor(pixel_nr, pixels.Color(Ledcolor,0,0)); 
        pixels.setPixelColor((pixel_nr+1), pixels.Color(Ledcolor,0,0)); 
         pixels.setPixelColor( pixel_nr2, pixels.Color(Ledcolor,0,0)); 
        pixels.setPixelColor(( pixel_nr2+1), pixels.Color(Ledcolor,0,0)); 
        pixels.show();
      }
      if((Ledcolor == 0 ) && (currentState - previousState >= offTime))
      {
        Ledcolor = 255;
        previousState = currentState;
        pixels.setPixelColor(pixel_nr, pixels.Color(Ledcolor,0,0)); 
        pixels.setPixelColor((pixel_nr+1), pixels.Color(Ledcolor,0,0)); 
        pixels.setPixelColor( pixel_nr2, pixels.Color(Ledcolor,0,0)); 
        pixels.setPixelColor(( pixel_nr2+1), pixels.Color(Ledcolor,0,0)); 
        pixels.show();
      
      }

    
   }

      void Update2()
   {
      unsigned long currentState = millis();
    //  for (int i = 0; i < NUMPIXELS - 1; i++) 
  //      {
      if((Ledcolor > 0) && (currentState - previousState >= onTime))
      {
        Ledcolor = 0;
        previousState = currentState;
        
          pixels.setPixelColor((pixel_nr), pixels.Color(0,0,0)); 
          pixels.setPixelColor( pixel_nr2, pixels.Color(Ledcolor,0,0));
      //    pixels.setPixelColor((pixel_nr + 1 + i), pixels.Color(0,0,0));
       
       // pixels.setPixelColor((pixel_nr+1), pixels.Color(Ledcolor,0,0)); 
     //    pixels.setPixelColor( pixel_nr2, pixels.Color(Ledcolor,0,0)); 
    //    pixels.setPixelColor(( pixel_nr2+1), pixels.Color(Ledcolor,0,0)); 
        pixels.show();
      }
  //    }
 
      if((Ledcolor == 0 ) && (currentState - previousState >= offTime))
      {
  //        for ( int i = 0;i < NUMPIXELS - 1; i++) 
   //  {
       
        Ledcolor = 255;
        
          pixels.setPixelColor((pixel_nr ), pixels.Color( val , val-20, Ledcolor-val)); 
          pixels.setPixelColor( pixel_nr2, pixels.Color( val , val/2, Ledcolor/2));

          val +=20;

          if (val > 255)
            val = 20;
         // pixels.setPixelColor((pixel_nr + 1 + i), pixels.Color(Ledcolor, Ledcolor/2 + 2*val ,Ledcolor - 25));
        //  Ledcolor -= val;
        
     //   pixels.setPixelColor((pixel_nr+1), pixels.Color(Ledcolor,0,0)); 
   //     pixels.setPixelColor( pixel_nr2, pixels.Color(Ledcolor,0,0)); 
   //     pixels.setPixelColor(( pixel_nr2+1), pixels.Color(Ledcolor,0,0)); 
        previousState = currentState;
    
        pixels.show();
      
     // }
      if (Ledcolor < 0 ) 
        Ledcolor = 0;

    
   }
   
 } //end Update2 function

   void Reset() 
   {  Ledcolor = 0;
      pixels.setPixelColor(pixel_nr, pixels.Color(Ledcolor,0,0)); 
        pixels.setPixelColor((pixel_nr+1), pixels.Color(Ledcolor,0,0)); 
        pixels.setPixelColor( pixel_nr2, pixels.Color(Ledcolor,0,0)); 
        pixels.setPixelColor(( pixel_nr2+1), pixels.Color(Ledcolor,0,0)); 
      previousState = 0;
      pixels.show();
   }
 
};


class Buzzer 
{
  //class variables
  int buzzerPin;
  long onTime;
  long offTime;

  //maintaint the current state
  int buzzerState;
  unsigned long previousState;

  public:
  Buzzer (int pin, int on , int off)
  {
    buzzerPin = pin;
    pinMode (buzzerPin, OUTPUT);
    
    onTime = on;
    offTime = off;

    buzzerState = HIGH; // because this buzzer is low triggered
    previousState =0;
    
  }

  void Update()
  {
    unsigned long currentState = millis();

    if ((buzzerState == LOW ) && (currentState - previousState >= onTime))
    {
       buzzerState = HIGH;
       previousState = currentState;
       digitalWrite(buzzerPin,buzzerState);
    }
    else if ((buzzerState == HIGH) && (currentState - previousState >= offTime))
    {

       buzzerState = LOW;
       previousState = currentState;
       digitalWrite(buzzerPin,buzzerState);
      
    }
     
  }
};

int melody[]= {NOTE_D5,-4, NOTE_A5,8, NOTE_FS5,8, NOTE_D5,8,
  NOTE_E5,-4, NOTE_FS5,8, NOTE_G5,4,
  NOTE_FS5,-4, NOTE_E5,8, NOTE_FS5,4,
  NOTE_D5,-2,
  NOTE_D5,-4, NOTE_A5,8, NOTE_FS5,8, NOTE_D5,8,
  NOTE_E5,-4, NOTE_FS5,8, NOTE_G5,4,
  NOTE_FS5,-1,
  NOTE_D5,-4, NOTE_A5,8, NOTE_FS5,8, NOTE_D5,8,
  NOTE_E5,-4, NOTE_FS5,8, NOTE_G5,4,
  
  NOTE_FS5,-4, NOTE_E5,8, NOTE_FS5,4,
  NOTE_D5,-2,
  NOTE_D5,-4, NOTE_A5,8, NOTE_FS5,8, NOTE_D5,8,
  NOTE_E5,-4, NOTE_FS5,8, NOTE_G5,4,
  NOTE_FS5,-1
  
  };
class Buzzer_Song
{
  int buzzerPin;
  long onTime;
  long offTime;
  int tempo;
  int wholenote;
  int notes;
  int divider;
  int noteDuration;
  int thisNote;

  //states
  int buzzerState;
  unsigned long previousState;

  public:
  Buzzer_Song (int pin, int on, int off, int tempo )
  {
    buzzerPin = pin;
    onTime = on;
    offTime =off;
    tempo = tempo;
    wholenote = (60000 * 4) / tempo;
    thisNote =0;
    notes = sizeof(melody) / sizeof(melody[0]) / 2;
    

     pinMode (buzzerPin, OUTPUT);
     buzzerState = HIGH; // because this buzzer is low triggered
     previousState =0;
  }

  void Update() {


    unsigned long currentState = millis();

    if ((buzzerState == LOW ) && (currentState - previousState >= onTime))
    {
     


      


      
       buzzerState = HIGH;
       previousState = currentState;
       digitalWrite(buzzerPin,buzzerState);
    }
    else if ((buzzerState == HIGH) && (currentState - previousState >= offTime))
    {


       divider = 0;
       noteDuration = 0;
      
       divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } 
    
    else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    tone(buzzerPin,melody[thisNote],noteDuration*0.9);

    thisNote += 2;

    if (thisNote > notes * 2) 
        thisNote =0;
      

      // buzzerState = LOW;
       previousState = currentState;
      // digitalWrite(buzzerPin,buzzerState);
      
    noTone(buzzer);

    }



    
          
}




 void Reset() 
   {
     buzzerState = HIGH;
      previousState = 0;
      digitalWrite(buzzerPin,buzzerState);
   }
  
 }; // end of class Buzzer_Song definition




ThreeWire myWire(20,21,8); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

#define countof(a) (sizeof(a) / sizeof(a[0]))
char hours[2],minutes[2];
 
char Time[] = "TIME:00:00:00";
char Date[] = "DATE:00/00/2000";
byte last_second, second_, minute_, hour_, day_, month_;
int year_;


Neopixel pixel_1(0,6,0,150,300);
Neopixel pixel_2(14,10,0,160,400);
Neopixel pixel_3(23,25,0,170,500);

Neopixel pixel_4(0,28,255,300,300);
Neopixel pixel_5(1,27,240,350,400);
Neopixel pixel_6(2,26,220,400,500);
Neopixel pixel_7(3,25,200,450,550);
Neopixel pixel_8(5,24,190,500,600);
Neopixel pixel_9(5,24,190,500,600);
Neopixel pixel_10(6,23,125,500,600);
Neopixel pixel_11(7,22,70,550,700);
Neopixel pixel_12(8,21,50,600,800);




Buzzer buzz(9,200,450);
Buzzer_Song buzz2(9,200,200,85);


void setup() {
  // put your setup code here, to run once:
  Serial.begin (9600);
   pinMode(buzzer, OUTPUT);
   digitalWrite(buzzer,HIGH);

   Serial.begin(115200);
  Serial1.begin(115200);

  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);

    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

   pixels.begin();

 sendData("AT+RST\r\n", 2000, false); // reset module
 sendData("AT+CWMODE=3\r\n", 1000, false); // configure as
 //access point
 sendData("AT+CWLAP\r\n", 7000,false);
 sendData("AT+CIFSR\r\n", 1000, DEBUG); // get ip address
 sendData("AT+CWSAP?\r\n", 2000, DEBUG); // get SSID info
 sendData("AT+CIPMUX=1\r\n", 1000, false); // configure for multiple connections
sendData("AT+CIPSERVER=1,80\r\n", 1000, false); // turn on

}

int angry_clock =0,cnt = 0;
unsigned long previousState =0;
int offTime = 10000;
int onTime = 100;
int alarm_on =0;
void loop() {
  // put your main code here, to run repeatedly:
  int value = analogRead(sensorPin);
  Serial.println (cnt);
  
unsigned long currentState = millis();
//   pixel_1.Update();
//   pixel_2.Update();
//   pixel_3.Update();
//   buzz.Update();   

//attachInterrupt(digitalPinToInterrupt(2), functieUnu, RISING); 

if ( alarm_on == 1) {
  
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
  
  
  

   if (cnt < 5000 ) 
       cnt ++;
    else 
    { cnt = 0;
     angry_clock = 0;

    pixel_1.Reset();
    pixel_2.Reset();
    pixel_3.Reset();
    buzz.Reset();
     
     }
 
  
  if (value >= ThresHOLD) 
  {
   // digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    Serial.println("KNOCK!");
     angry_clock = 1;
     
  }
  
  pixels.clear();

  
}

RtcDateTime dt = Rtc.GetDateTime();
   
   if (dt.IsValid())
{

    Time[12] = dt.Second() % 10 + 48;
    Time[11] = dt.Second() / 10 + 48;
    
    Time[9]  = dt.Minute() % 10 + 48;
    Time[8]  = dt.Minute() / 10 + 48;

    
    Time[6]  = dt.Hour()   % 10 + 48;
    Time[5]  = dt.Hour()   / 10 + 48;

}



if ( hours[0] == Time[5] &&  hours[1] == Time[6] &&  minutes[0] == Time[8] &&  minutes[1] == Time[9])
{ Serial.println("alarm 0000"); 
  alarm_on = 1;

}
else alarm_on =0;
  
 if (Serial1.available()) {

      // read HTTP request 
        char c = Serial1.read();

       

                     if (Serial1.find("+IPD,")) {
                       delay(500);
                       int connectionId = Serial1.read() - 48; // read()

                        String header = "HTTP/1.1 200 OK\r\nContent-type:text/html\r\nConnection: close\r\n";
                        String webclient = "<!DOCTYPE html>\r\n<html>\r\n";

                                  
                        webclient += "\r\n<p>\r\n Time </p>\r\n";
     
                        webclient += "<form action=\"/get\"> Alarm: <input type=\"text\" name=\"alarm\"><input type=\"submit\" value=\"Submit\">\r\n</form>";


                           
                        

                         if ( hours[0] == Time[5] &&  hours[1] == Time[6] &&  minutes[0] == Time[8] &&  minutes[1] == Time[9])
                                       { Serial.println("alarm");
                                           webclient += "<p>\r\n Alarm is ringing \r\n</p>\r\n ";
                                       }
                        

                      // String webpage = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nConnection: close\r\n\r\n<hmtl>\r\n<body>\r\n<h1>\r\nHello World!\r\n</h1>\r\n<ahref=\"/l0\">\r\n<button>ON</button>\r\n</a>";
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

void angry_clock()
{
    pixels.clear();
    pixels.setBrightness(80);
    
    ////right brow
    //void colorWipe(uint32_t color, int wait) {
    //  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    //    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    //    strip.show();                          //  Update strip to match
    //    delay(wait);                           //  Pause for a moment
    //  }
    //}
    
    for ( int i=-1; i< 3; i++ ) 
    { 
     pixels.setPixelColor(array_brow_right_1[i], pixels.Color(255,0,0));
      pixels.setPixelColor(array_brow_right_2[i], pixels.Color(255,0,0));
      pixels.setPixelColor(array_brow_left_1[i], pixels.Color(255,0,0));
      pixels.setPixelColor(array_brow_left_2[i], pixels.Color(255,0,0));
      pixels.show();
      
      delay (200);
    } 
}



void louder_alarm( int loops) 
{
    for (int i =0; i < loops ; i++)
     {  digitalWrite(buzzer,LOW);
        delay(500);
        digitalWrite(buzzer,HIGH);
        delay(100);
     }

}

void wifi_clock()
{
     RtcDateTime dt = Rtc.GetDateTime();
   
   if (dt.IsValid())
{

    Time[12] = dt.Second() % 10 + 48;
    Time[11] = dt.Second() / 10 + 48;
    
    Time[9]  = dt.Minute() % 10 + 48;
    Time[8]  = dt.Minute() / 10 + 48;

    
    Time[6]  = dt.Hour()   % 10 + 48;
    Time[5]  = dt.Hour()   / 10 + 48;

}



if ( hours[0] == Time[5] &&  hours[1] == Time[6] &&  minutes[0] == Time[8] &&  minutes[1] == Time[9])
{ Serial.println("alarm 0000"); 
  val = 1;

}
  
 if (Serial1.available()) {

      // read HTTP request 
        char c = Serial1.read();

       

                     if (Serial1.find("+IPD,")) {
                       delay(500);
                       int connectionId = Serial1.read() - 48; // read()

                        String header = "HTTP/1.1 200 OK\r\nContent-type:text/html\r\nConnection: close\r\n";
                        String webclient = "<!DOCTYPE html>\r\n<html>\r\n";

                                  
                        webclient += "\r\n<p>\r\n Time </p>\r\n";
     
                        webclient += "<form action=\"/get\"> Alarm: <input type=\"text\" name=\"alarm\"><input type=\"submit\" value=\"Submit\">\r\n</form>";


                           
                        

                         if ( hours[0] == Time[5] &&  hours[1] == Time[6] &&  minutes[0] == Time[8] &&  minutes[1] == Time[9])
                                       { Serial.println("alarm");
                                           webclient += "<p>\r\n Alarm is ringing \r\n</p>\r\n ";
                                       }
                        

                      // String webpage = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nConnection: close\r\n\r\n<hmtl>\r\n<body>\r\n<h1>\r\nHello World!\r\n</h1>\r\n<ahref=\"/l0\">\r\n<button>ON</button>\r\n</a>";
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

String sendData(String command, const int timeout, boolean debug)
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
if (response.indexOf("GET /get?alarm=") >= 0)
{    int index = response.indexOf("GET /get?alarm=");
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

void buttonInterrupt() 
{ 
   // digitalWrite(13, !digitalRead(13)); // Change the status  
    //of pin 13 
  //  lcd.setCursor(0,0); // Move the LCD cursor in the top-left  
    //corner 
 //   lcd.print("Intrerupem");// Display message 
 //   lcd.setCursor(0,1); 
 //   lcd.print("ptr stirea zilei"); 
//    buttonVariable = 1; 
}
