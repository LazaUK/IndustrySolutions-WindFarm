#include <SoftwareSerial.h>
#include <Arduino.h>
#include <Arduino.h>
#include "disk91_LoRaE5.h"
#include <SensirionI2CSht4x.h>
#include <Wire.h>

SensirionI2CSht4x sht4x;
 
Disk91_LoRaE5 lorae5(&Serial); // Where the AT command and debut traces are printed

#define Frequency DSKLORAE5_ZONE_EU868
/*
Select your frequency band here.
DSKLORAE5_ZONE_EU868
DSKLORAE5_ZONE_US915
DSKLORAE5_ZONE_AS923_1
DSKLORAE5_ZONE_AS923_2
DSKLORAE5_ZONE_AS923_3
DSKLORAE5_ZONE_AS923_4
DSKLORAE5_ZONE_KR920
DSKLORAE5_ZONE_IN865
DSKLORAE5_ZONE_AU915
 */
 
char deveui[] = "<PLACEHOLDER_FOR_DEVEUI>";
char appeui[] = "<PLACEHOLDER_FOR_APPEUI>";
char appkey[] = "<PLACEHOLDER_FOR_APPKEY>";

 
void data_decord(int val_1, int val_2, uint8_t data[4])
{
  int val[] = {val_1, val_2};

  for(int i = 0, j = 0; i < 2; i++, j += 2)
  {
    if(val[i] < 0)
    {
      val[i] = ~val[i] + 1;
      data[j] = val[i] >> 8 | 0x80;
      data[j+1] = val[i] & 0xFF;
    }
    else
    {
      data[j] = val[i] >> 8 & 0xFF;
      data[j+1] = val[i] & 0xFF;
    }
  }

}
 

void setup(void)
{
  Serial.begin(9600);

  uint32_t start = millis();
  while ( !Serial && (millis() - start) < 1500 );  // Open the Serial Monitor to get started or wait for 1.5"
  
  Wire.begin();

  uint16_t error;
  char errorMessage[256];

  sht4x.begin(Wire);

  uint32_t serialNumber;
  error = sht4x.serialNumber(serialNumber);
  delay(5000);
  if (error) {
      Serial.print("Error trying to execute serialNumber(): ");
      errorToString(error, errorMessage, 256);
      Serial.println(errorMessage);
  } else {
      Serial.print("Serial Number: ");
      Serial.println(serialNumber);
  }

  // init the library, search the LORAE5 over the different WIO port available
  if ( ! lorae5.begin(DSKLORAE5_SWSERIAL_WIO_P2) ) {
    Serial.println("LoRa E5 Init Failed");
    while(1); 
  }

  // Setup the LoRaWan Credentials
  if ( ! lorae5.setup(
        Frequency,
        deveui,
        appeui,
        appkey
     ) ){
    Serial.println("LoRa E5 Setup Failed");
    while(1);         
  }
}
 
void loop(void)
{
  uint16_t error;
  float temperature, humidity;
  int int_temp, int_humi;
  
  error = sht4x.measureHighPrecision(temperature, humidity);
  Serial.print("Temperature: "); Serial.print(temperature);
  Serial.print(" Humidity: "); Serial.println(humidity);

  int_temp = temperature*100;
  int_humi = humidity*100;
  
  static uint8_t data[4] = { 0x00 };  //Use the data[] to store the values of the sensors

  data_decord(int_temp, int_humi, data);
  
  if ( lorae5.send_sync(              //Sending the sensor values out
        8,                            // LoRaWan Port
        data,                         // data array
        sizeof(data),                 // size of the data
        false,                        // we are not expecting a ack
        7,                            // Spread Factor
        14                            // Tx Power in dBm
       ) 
  ) {
      Serial.println("Uplink done");
      if ( lorae5.isDownlinkReceived() ) {
        Serial.println("A downlink has been received");
        if ( lorae5.isDownlinkPending() ) {
          Serial.println("More downlink are pending");
        }
      }
  }
  delay(150000);
}
