/*
  WriteMultipleVoltages
  
  Reads analog voltages from pins 0-7 and writes them to the 8 fields of a channel on ThingSpeak every 20 seconds as ThingSpeak doesn't like to be poked more often than once every 15 seconds.
  
  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize and 
  analyze live data streams in the cloud.
  
  Copyright 2017, The MathWorks, Inc.
  
  Documentation for the ThingSpeak Communication Library for Arduino is in the extras/documentation folder where the library was installed.
  See the accompanying license file for licensing information.
*/

#include "ThingSpeak.h"

// ***********************************************************************************************************
// This example selects the correct library to use based on the board selected under the Tools menu in the IDE.
// Yun, Ethernet shield, WiFi101 shield and MKR1000 are supported. 
// EPS8266 and ESP32 are not compatible with this example. -> Actaully they are.
// With Yun, the default is that you're using the Ethernet connection.
// If you're using a wi-fi 101 or ethernet shield (http://www.arduino.cc/en/Main/ArduinoWiFiShield), uncomment the corresponding line below
// ***********************************************************************************************************

//#define USE_WIFI101_SHIELD
//#define USE_ETHERNET_SHIELD

#if defined(ARDUINO_ARCH_ESP32)
  #error "Please remove this block and check with ESP32 to confirm it works. Does it even have A0 pin ? As of Dec 2017 there were also problems with ADC being having non linerar response.."
#endif

#if !defined(USE_WIFI101_SHIELD) && !defined(USE_ETHERNET_SHIELD) && !defined(ARDUINO_SAMD_MKR1000) && !defined(ARDUINO_AVR_YUN) && !defined(ARDUINO_ARCH_ESP8266) && !defined(ARDUINO_ARCH_ESP32)
  #error "Uncomment the #define for either USE_WIFI101_SHIELD or USE_ETHERNET_SHIELD"
#endif

#if defined(ARDUINO_AVR_YUN)
  #include "YunClient.h"
    YunClient client;
#else
  #if defined(USE_WIFI101_SHIELD) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32) 
    // Use WiFi
    #if defined(USE_WIFI101_SHIELD) || defined(ARDUINO_SAMD_MKR1000)
      #include <SPI.h>
      #include <WiFi101.h>
      WiFiClient  client;
    #elif defined(ARDUINO_ARCH_ESP8266)
      #include <ESP8266WiFi.h>
      WiFiClient client;
    #elif defined(ARDUINO_ARCH_ESP32)
      #include <WiFi.h>
      WiFiClient client;      
    #endif  
    // common for every WiFi platfrom
    char ssid[] = "YOUR_NETWORK";    //  your network SSID (name)
    char pass[] = "YOUR_PASSWORD";   //  your network password
    int status = WL_IDLE_STATUS;
  #else if defined(USE_ETHERNET_SHIELD)
    // Use wired ethernet shield
    #include <SPI.h>
    #include <Ethernet.h>
    byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
    EthernetClient client;
  #endif
#endif

#ifdef ARDUINO_ARCH_AVR
  // On Arduino:  0 - 1023 maps to 0 - 5 volts
  #define VOLTAGE_MAX 5.0
  #define VOLTAGE_MAXCOUNTS 1023.0
#elif ARDUINO_SAMD_MKR1000
  // On MKR1000:  0 - 1023 maps to 0 - 3.3 volts
  #define VOLTAGE_MAX 3.3
  #define VOLTAGE_MAXCOUNTS 1023.0
#elif ARDUINO_SAM_DUE
  // On Due:  0 - 1023 maps to 0 - 3.3 volts
  #define VOLTAGE_MAX 3.3
  #define VOLTAGE_MAXCOUNTS 1023.0
#elif ARDUINO_ARCH_ESP8266
  ADC_MODE(ADC_VCC);
  #define VOLTAGE_MAX 3.3
  #define VOLTAGE_MAXCOUNTS 1023.0
#elif ARDUINO_ARCH_ESP32
  #define VOLTAGE_MAX 3.3
  #define VOLTAGE_MAXCOUNTS 4095.0  
#endif



/*
  *****************************************************************************************
  **** Visit https://www.thingspeak.com to sign up for a free account and create
  **** a channel.  The video tutorial http://community.thingspeak.com/tutorials/thingspeak-channels/
  **** has more information. You need to change this to your channel, and your write API key
  **** IF YOU SHARE YOUR CODE WITH OTHERS, MAKE SURE YOU REMOVE YOUR WRITE API KEY!!
  *****************************************************************************************/
unsigned long myChannelNumber = <YOUR_CHANNEL_ID>;
const char * myWriteAPIKey = "<YOUR_WRITE_API_KEY>";

void setup() {

#ifdef ARDUINO_AVR_YUN
  Bridge.begin();
#else
  #if defined(USE_WIFI101_SHIELD) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    WiFi.begin(ssid, pass);
  #elif defined(USE_ETHERNET_SHIELD)
    Ethernet.begin(mac);
  #endif
#endif

ThingSpeak.begin(client);

}

void loop() {
  // Read the input on each pin, convert the reading, and set each field to be sent to ThingSpeak.
  // On Uno,Mega,Yun:   0 - 1023 maps to 0 - 5 volts
  // On MKR1000,Due:    0 - 4095 maps to 0 - 3.3 volts
  // On ESP8266:        0 - 1023 maps to 0 - 3.3 volts of supply Vcc
  float pinVoltage = analogRead(A0) * (VOLTAGE_MAX / VOLTAGE_MAXCOUNTS);
  ThingSpeak.setField(1, pinVoltage);

  // Write the fields that you've set all at once.
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  delay(20000); // ThingSpeak will only accept updates every 15 seconds.
}
