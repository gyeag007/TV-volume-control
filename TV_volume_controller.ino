/*
PROJECT: TV Volume Guard - stolen from http://www.instructables.com/id/TV-Volume-Loudness-Guard-using-Arduino/
AUTHOR: Hazim Bitar (techbitar)
DATE: FEB 9, 2013
CONTACT: techbitar at gmail dot com
LICENSE: My code is in the public domain.
IRremote library: copyright by Ken Shirriff http://arcfn.com
Modified to raise volume when commercial is over (volume is too low).
Also modified to read peak-to-peak sound values - from Adafruit "MeasuringSoundLevels" sketch.
A0 is from microphone analog output.
*/

#include "PinDefinitionsAndMore.h"
#include <IRremote.h>
//IRsend irsend; // instantiate IR object
 
#define NOISE_LEVEL_MAX    400       // Max level of noise to detect from 0 to 1023
#define NOISE_LEVEL_MIN    110        // Min level of noise to detect from 0 to 1023
#define MUTE_LEVEL_MIN     60        // Min level of noise to catch mute
#define REPEAT_TX          3          // how many times to transmit the IR remote code
#define IR_SEND_PIN         3

//Sony Bravia TV (living room) remote info
//#define VOL_DOWN_CODE_L    0xc90      // volume down remote code to transmit. Living room
//#define VOL_UP_CODE_L      0x490      // volume down remote code to transmit. Living room
//#define REMOTE_BIT         12         // how many bits is remote code?
 
#define LED                13         // pin for LED used to blink when volume too high
int AmbientSoundLevel = 200;            // Microphone sensor initial value
const int sampleWindow = 50;          // Sample window width in mS (50 mS = 20Hz)
long time_sent = 0;
long send_interval = 000;


void setup()
{
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  Serial.println("TV Volume Guard");
  Serial.print("NOISE_LEVEL_MIN is ");
  Serial.println(NOISE_LEVEL_MIN);
  Serial.print("NOISE_LEVEL_Max is ");
  Serial.println(NOISE_LEVEL_MAX);
  Serial.print("Ambient sound level: ");
  Serial.println(AmbientSoundLevel);
  Serial.println("-------------------");
}
 
void loop()
{


  if(millis() - time_sent >= send_interval){
    Serial.print("Ambient sound level: ");
    AmbientSoundLevel = getAmbientSoundLevel();
    Serial.println(AmbientSoundLevel);

    if (AmbientSoundLevel > NOISE_LEVEL_MAX){ // compare to noise level threshold you decide
      Serial.print("sound level is ABOVE maximum of ");
      Serial.println(NOISE_LEVEL_MAX);
      digitalWrite(LED, HIGH); // LED on
      delay(200);
      Serial.println("LOWERing volume...");
      
      for (int i = 0; i < 2; i++) {
        IrSender.sendSony(0x30, 0x13, 2, 15); //volume down


        delay(100);
      }
    }

    else if ((AmbientSoundLevel < NOISE_LEVEL_MIN) && (AmbientSoundLevel > MUTE_LEVEL_MIN))
    {
      Serial.print("sound level is below minimum of ");
      Serial.println(NOISE_LEVEL_MIN);
      digitalWrite(LED, LOW); // LED off
      delay(200);
      Serial.println("raising volume...");
      for (int i = 0; i < 1; i++) {
        IrSender.sendSony(0x30, 0x12, 2, 15); //volume up 


        delay(40);
      }
    }
    else if ((AmbientSoundLevel < MUTE_LEVEL_MIN))
    {
      Serial.println("muted");
    }
    else 
    {
      Serial.println("just right...");
    }
    digitalWrite(LED, LOW); // LED off
    time_sent = millis();
  }
}
 
int getAmbientSoundLevel()
{
  unsigned long startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  unsigned int sample;
  unsigned int samples[200];             // Array to store samples
  int sampleAvg = 0;
  long sampleSum = 0;
  unsigned int exp_sample;
  int experimentalsamplesum = 0;
  int experimentalsampleavg = 0;
  unsigned int experimentalsamples[10];

 
  for (int i = 0; i <= 200; i++) {
    // collect data for 50 mS     
      startMillis = millis();

    while (millis() - startMillis < sampleWindow)
    {
      sample = analogRead(0);
      //exp_sample = analogRead(0);
      //Serial.println(sample);
      if (sample < 1024 && sample > 0)  // toss out spurious readings
      {
        if (sample > signalMax)
        {
          signalMax = sample;  // save just the max levels
        }
        else if (sample < signalMin)
        {
          signalMin = sample;  // save just the min levels
        }
      }
    }
    peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
    samples[i] = peakToPeak;
    //experimentalsamples[i] = exp_sample;
    //Serial.println(i);

  }
  for (int i = 0; i <= 200; i++) {
    
    sampleSum = sampleSum + samples[i];
    //experimentalsamplesum = experimentalsamplesum + experimentalsamples[i];
                    //Serial.println(i);

         // Serial.println(samples[i]);
             //Serial.print("sampleSum: ");

               // Serial.println(sampleSum);


  }
  sampleAvg = sampleSum/200;
  //experimentalsampleavg = experimentalsamplesum/10;
   //Serial.print("sampleavg: ");

 //Serial.println(sampleAvg);

  return sampleAvg;
}
