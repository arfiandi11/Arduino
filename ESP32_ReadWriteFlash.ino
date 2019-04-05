/* 
Refer to 
http://ruten-proteus.blogspot.com/2016/12/ESP8266ArduinoQA-02.html
http://wyj-learning.blogspot.com/2018/03/nodemcu-flash.html

NodeMCU (ESP32)
Author : ChungYi Fu (Kaohsiung, Taiwan)  2019-4-5 14:30
https://www.facebook.com/francefu
*/

#include <WiFi.h>   
#include <ESP.h>

char ssid[64]    = "xxxxx";
char password[64]    = "xxxxx";

const char* apssid = "ESP32";
const char* appassword = "12345678";   //AP password require at least 8 characters.

char buff_ssid[64];
char buff_password[64];
const uint32_t addrstart = 0x3FA000; 
const uint32_t addrend   = 0x3FB000;

void setup() {
  Serial.begin(115200);

  /* Test
  flashWrite();  // Write SSID and password to SPI FLASH
  delay(100);
  strcpy(ssid,"123");  // Set nonexistent SSID
  */
  
  connectWIFI(ssid, password);
  
  if (WiFi.localIP().toString()=="0.0.0.0") {
    Serial.println("Connection Failed");
    Serial.println("\nRead SPI FLASH...");
    flashRead();
    connectWIFI(buff_ssid, buff_password);
  }
}

void loop() {
    delay(500);
}

void connectWIFI(char* id, char* pwd) {
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(id, pwd);
  delay(1000);
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  long int StartTime=millis();
  while (WiFi.status() != WL_CONNECTED) 
  {
  delay(500);
  if ((StartTime+10000) < millis()) break;
  } 

  if (WiFi.localIP().toString()!="0.0.0.0") {
    Serial.println("");
    Serial.println("STAIP address: ");
    Serial.println(WiFi.localIP());    
    WiFi.softAP((WiFi.localIP().toString()+"_"+(String)apssid).c_str(), appassword);
    flashWrite();
  }
  else
    WiFi.softAP(apssid, appassword);  
}

void flashWrite() {
  flashErase();
  uint32_t flash_address;
    
  flash_address = addrstart;
  if( !ESP.flashWrite( flash_address, (uint32_t*)ssid, sizeof(ssid) - 1 ) ) {
      Serial.printf( "%2d: [error] write addr: %p\n", 0, flash_address );
  } else 
    Serial.printf( "%2d: addr: %p write [%s] OK\n", 0, flash_address, ssid );

  flash_address = addrstart + 64;
  if( !ESP.flashWrite( flash_address, (uint32_t*)password, sizeof(password) - 1 ) ) {
      Serial.printf( "%2d: [error] write addr: %p\n", 1, flash_address );
  } else 
    Serial.printf( "%2d: addr: %p write [%s] OK\n", 1, flash_address, password );  
}

void flashRead() {
  uint32_t flash_address;
  
  flash_address = addrstart;
  memset( buff_ssid, 0, sizeof( buff_ssid ) );
  if ( ESP.flashRead( flash_address, (uint32_t*)buff_ssid, sizeof( buff_ssid )-1) ) {
    Serial.printf( "[ssid] = %s \n", buff_ssid );
    strcpy(ssid, buff_ssid);
  }
  
  flash_address = addrstart + 64;
  memset( buff_password, 0, sizeof( buff_password ) );
  if ( ESP.flashRead( flash_address, (uint32_t*)buff_password, sizeof( buff_password )-1) ) {
    Serial.printf( "[password] = %s \n", buff_password );
    strcpy(password, buff_password);
  }
}

void flashErase() {
  if( !ESP.flashEraseSector( addrstart >> 12 ) ) {
    Serial.println( "\nErase error");
  } else {
    Serial.println( "\nErase OK");
  }
}

/*
bool flashEraseSector(uint32_t sector);
bool flashWrite(uint32_t offset, uint32_t *data, size_t size);
bool flashRead(uint32_t offset, uint32_t *data, size_t size);
void *memset(void *str, int c, size_t n)
*/
