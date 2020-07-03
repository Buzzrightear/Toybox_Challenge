//Version 6 of hjas2 IoT Toybox project

//Wifi connection code from: https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFiClientSecure/examples/WiFiClientSecureEnterprise/WiFiClientSecureEnterprise.ino
//RFID scanning code from PN52 library examples
//Time stamp code from ESP32 library examples and NTPClient lib https://randomnerdtutorials.com/esp32-ntp-client-date-time-arduino-ide/ 
//File system code from https://techtutorialsx.com/2018/08/05/esp32-arduino-spiffs-writing-a-file/ 
#include "SPIFFS.h"
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <WiFi.h>
#include "time.h"
#include <WiFiClientSecure.h>
#include "esp_wpa2.h"
#include <NTPClient.h> // 
#include <HTTPClient.h>

//Toybox scanner ID declaration
String toyboxUUID = "";

//WiFi declarations:
#define EAP_ANONYMOUS_IDENTITY "" //anonymous identity
#define EAP_IDENTITY "hjas2@kent.ac.uk"                  //user identity
#define EAP_PASSWORD "B0dyR0ck3r5" //eduroam user password
const char* ssid = "eduroam"; // eduroam SSID
const char* host = "arduino.php5.sk"; //external server domain for HTTPS connection
int counter = 0;
const char* test_root_ca = \
                           "-----BEGIN CERTIFICATE-----\n" \
                           "MIIEsTCCA5mgAwIBAgIQCKWiRs1LXIyD1wK0u6tTSTANBgkqhkiG9w0BAQsFADBh\n" \
                           "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
                           "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
                           "QTAeFw0xNzExMDYxMjIzMzNaFw0yNzExMDYxMjIzMzNaMF4xCzAJBgNVBAYTAlVT\n" \
                           "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
                           "b20xHTAbBgNVBAMTFFJhcGlkU1NMIFJTQSBDQSAyMDE4MIIBIjANBgkqhkiG9w0B\n" \
                           "AQEFAAOCAQ8AMIIBCgKCAQEA5S2oihEo9nnpezoziDtx4WWLLCll/e0t1EYemE5n\n" \
                           "+MgP5viaHLy+VpHP+ndX5D18INIuuAV8wFq26KF5U0WNIZiQp6mLtIWjUeWDPA28\n" \
                           "OeyhTlj9TLk2beytbtFU6ypbpWUltmvY5V8ngspC7nFRNCjpfnDED2kRyJzO8yoK\n" \
                           "MFz4J4JE8N7NA1uJwUEFMUvHLs0scLoPZkKcewIRm1RV2AxmFQxJkdf7YN9Pckki\n" \
                           "f2Xgm3b48BZn0zf0qXsSeGu84ua9gwzjzI7tbTBjayTpT+/XpWuBVv6fvarI6bik\n" \
                           "KB859OSGQuw73XXgeuFwEPHTIRoUtkzu3/EQ+LtwznkkdQIDAQABo4IBZjCCAWIw\n" \
                           "HQYDVR0OBBYEFFPKF1n8a8ADIS8aruSqqByCVtp1MB8GA1UdIwQYMBaAFAPeUDVW\n" \
                           "0Uy7ZvCj4hsbw5eyPdFVMA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEF\n" \
                           "BQcDAQYIKwYBBQUHAwIwEgYDVR0TAQH/BAgwBgEB/wIBADA0BggrBgEFBQcBAQQo\n" \
                           "MCYwJAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBCBgNVHR8E\n" \
                           "OzA5MDegNaAzhjFodHRwOi8vY3JsMy5kaWdpY2VydC5jb20vRGlnaUNlcnRHbG9i\n" \
                           "YWxSb290Q0EuY3JsMGMGA1UdIARcMFowNwYJYIZIAYb9bAECMCowKAYIKwYBBQUH\n" \
                           "AgEWHGh0dHBzOi8vd3d3LmRpZ2ljZXJ0LmNvbS9DUFMwCwYJYIZIAYb9bAEBMAgG\n" \
                           "BmeBDAECATAIBgZngQwBAgIwDQYJKoZIhvcNAQELBQADggEBAH4jx/LKNW5ZklFc\n" \
                           "YWs8Ejbm0nyzKeZC2KOVYR7P8gevKyslWm4Xo4BSzKr235FsJ4aFt6yAiv1eY0tZ\n" \
                           "/ZN18bOGSGStoEc/JE4ocIzr8P5Mg11kRYHbmgYnr1Rxeki5mSeb39DGxTpJD4kG\n" \
                           "hs5lXNoo4conUiiJwKaqH7vh2baryd8pMISag83JUqyVGc2tWPpO0329/CWq2kry\n" \
                           "qv66OSMjwulUz0dXf4OHQasR7CNfIr+4KScc6ABlQ5RDF86PGeE6kdwSQkFiB/cQ\n" \
                           "ysNyq0jEDQTkfa2pjmuWtMCNbBnhFXBYejfubIhaUbEv2FOQB3dCav+FPg5eEveX\n" \
                           "TVyMnGo=\n" \
                           "-----END CERTIFICATE-----\n";
// You can use x.509 client certificates if you want
//const char* test_client_key = "";   //to verify the client
//const char* test_client_cert = "";  //to verify the client
WiFiClientSecure client;
int timer = 0;

//Alt. wifi declarations for non-enterprise wifi:
const char* ssid2     = "NETGEAR94";
const char* password = "dizzyshoe710";

//Date time declarations
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP); // Define NTP Client to get time
String formattedDate;
String dayStamp;
String timeStamp;

//Scanner PN532 device declarations
PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);  

//ToyTags list declarations
int MAX_SIZE = 1000;
int current_size = 0;
String toyTags[1000][3]; //Store tags in 2d array in format - {toybox scanner UUID; toytagID; timestamp} https://forum.arduino.cc/index.php?topic=220385.0 

//Buzzer declarations: https://techtutorialsx.com/2017/07/01/esp32-arduino-controlling-a-buzzer-with-pwm/
//https://techtutorialsx.com/2017/06/15/esp32-arduino-led-pwm-fading/
int freq = 500; //Changing this will change the tone
int channel = 0;
int resolution = 8; //This is the resolution (8 bits)
int dutyCycle = 50; //Should be the volume

//LED declarations
const int ledPin = 33;
  
void setup(void) {
  //Begin setup
  Serial.begin(115200);
  Serial.println("Hello!");
  
  //Temporary connection to home wifi...............................
  
  Serial.print("Attempting to connect to basic network: ");
  Serial.println(ssid2);  
  Serial.print("tempResponse is ");
  Serial.print(WiFi.begin(ssid2,password));
  delay(1000);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to home WiFi network");
  //.................................................................

  
  //Populate toyTags list with a "*" in each index
  for (int i=0;i<MAX_SIZE;i++){
    toyTags[i][0] = "";
    toyTags[i][1] = "*";    
    toyTags[i][2] = "";
  }
  
  //connect to WiFi
  //wifiConnect();

  //get Toybox UUID if required
  getUUID();

  
  //init and get the time
  timeClient.begin();
  timeClient.setTimeOffset(0); // Set offset time in seconds to adjust for your timezone
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //Set up file system
  setupFileMount();

  
  //Initialise RFID Scanner board
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();

  //Setup Buzzer
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(12, channel);//Pin 12 selected

  //setup LED pin as a digital output pin
  pinMode (ledPin, OUTPUT);

  //Test LED and buzzer
  buzzerLEDON();
}

//Main loop *****************************************
void loop(void) {
  timeClient.forceUpdate();//sometimes the NTP Client retrieves 1970. To ensure that doesn’t happen we need to force the update.
  scanForTags();
  timer++;  //scanForTags() has a delay of 1000ms built in, so we're increasing timer by 1000ms here
  if (timer >=5){
    checkWifi();
    timer = 0;
  }
 
}
//***************************************************


//scan for toy tags
void scanForTags(){
  
  Serial.println("scanforTags() executing");
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    printLocalTime(); //Need to do something with this timestamp
    processTag(uid);
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    
    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ... 
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
    
      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    
    // Start with block 4 (the first block of sector 1) since sector 0
    // contains the manufacturer data and it's probably better just
    // to leave it alone unless you know what you're doing
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
    
      if (success)
      {
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        uint8_t data[16];
    
        // If you want to write something to block 4 to test with, uncomment
    // the following line and this text should be read back in a minute
        // data = { 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0};
        // success = nfc.mifareclassic_WriteDataBlock (4, data);

        // Try to read the contents of block 4
        success = nfc.mifareclassic_ReadDataBlock(4, data);
    
        if (success)
        {
          // Data seems to have been read ... spit it out
          Serial.println("Reading Block 4:");
          nfc.PrintHexChar(data, 16);
          Serial.println("");
      
          // Wait a bit before reading the card again
          delay(1000);
        }
        else
        {
          Serial.println("Ooops ... unable to read the requested block.  Try another key?");
        }
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
      }
    }
    
    if (uidLength == 7)
    {
      // We probably have a Mifare Ultralight card ...
      Serial.println("Seems to be a Mifare Ultralight tag (7 byte UID)");
    
      // Try to read the first general-purpose user page (#4)
      Serial.println("Reading page 4");
      uint8_t data[32];
      success = nfc.mifareultralight_ReadPage (4, data);
      if (success)
      {
        // Data seems to have been read ... spit it out
        nfc.PrintHexChar(data, 4);
        Serial.println("");
    
        // Wait a bit before reading the card again
        delay(1000);
      }
      else
      {
        Serial.println("Ooops ... unable to read the requested page!?");
      }
    }
  }  
}


//Either add or remove newly scanned tag to/from list of toy tags
void processTag(uint8_t aTag[]){
  formattedDate = timeClient.getFormattedDate();//dateTime stamp
  Serial.println(formattedDate);
  
  /**If you want to get date and time separately, you need to split that string. The “T” letter separates the date from the time
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  Serial.println(dayStamp);
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.println(timeStamp);
  **/

  String someTag = tagToString(aTag);
  int tempIndex = isItInArray(someTag);
  String tempPostInfo = toyboxUUID + ", " + someTag + ", " + formattedDate; 

  if (tempIndex >= 0 && current_size > 0){
    toyTags[tempIndex][0] = "";
    toyTags[tempIndex][1]="*";    
    toyTags[tempIndex][2] = "";
    current_size--;
    Serial.println("Toy removed from list: " + tempPostInfo);
    writeToFile();
    postTagInfo(tempPostInfo);
    buzzerLEDON();    
  }
  else if (tempIndex <0 && current_size < MAX_SIZE) {
    int tempIndex2 = findFirstNullSpace();
    toyTags[tempIndex2][0] = toyboxUUID;
    toyTags[tempIndex2][1] = someTag;
    toyTags[tempIndex2][2] = formattedDate;    
    current_size++;   
    Serial.println("Toy added to list: " + tempPostInfo);    
    Serial.println(toyTags[tempIndex2][0] + ", " + toyTags[tempIndex2][1] + ", "  + toyTags[tempIndex2][2]);    
    writeToFile();
    postTagInfo(tempPostInfo);
    buzzerLEDON();
  }
  else if (current_size >= MAX_SIZE) {
    Serial.println("Ooops, too many toys in box, cannot add any more");
    //Send message to server  
  }
  else if (current_size <=0){
    Serial.println("Ooops, no toys in box, can't take this one out");
    //Send message to server      
  }
}

//post message to server
void postTagInfo(String aTag){
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
    HTTPClient http;   
    http.begin("http://helloworld-env.matddinqfi.eu-west-2.elasticbeanstalk.com/hello");  //***************************Specify destination for HTTP request
    http.addHeader("Content-Type",  "application/x-www-form-urlencoded");             //Specify content-type header    
    int httpResponseCode = http.POST(aTag);   //Send the actual POST request
    if(httpResponseCode>0){
      String response = http.getString();                       //Get the response to the request
      Serial.println("Response code is " + httpResponseCode);   //Print return code
      Serial.println("response is " + response);           //Print request answer
    }
    else{
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();  //Free resources
  }
  else{
    Serial.println("Error in WiFi connection");   
  }
}

//Find first empty slot in list to store new tag
int findFirstNullSpace(){
  bool foundSpace = false;
  int counter2 = 0;
  while (!foundSpace && counter2 < MAX_SIZE){
    if (toyTags[counter2][1] == "*"){
      foundSpace = true;
      return counter2;
    }
    else{
       counter2++;  
    }   
  }
}

//Check if supplied toy tag is currently stored in array
int isItInArray(String aTag){
  for (int j=0;j<MAX_SIZE;j++){
    if (toyTags[j][1] == aTag){
      return j;
    }
  }
  return -1;
}
  
//Convert tag from integer array to String
String tagToString(uint8_t aTag[]){
  String tempTag;
  for(int i =0; i<sizeof(aTag);i++){
    tempTag = tempTag + aTag[i];
  }
  return tempTag;
}

//Print local time to serial output
void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println("Time is: ");
  Serial.println(&timeinfo, "%B %d %Y %H:%M:%S");
}

//Initiate connection to wifi
void wifiConnect(){
  Serial.print("Attempting to connect to basic network: ");
  Serial.println(ssid2);  
  Serial.print("tempResponse is ");
  Serial.print(WiFi.begin(ssid2,password));
  delay(1000);
  if(WiFi.status() != WL_CONNECTED){
    Serial.println();
    Serial.print("Connecting to network: ");
    Serial.println(ssid);
    WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
    WiFi.mode(WIFI_STA); //init wifi mode
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY)); //provide identity
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); //provide username
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD)); //provide password
    esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();
    esp_wifi_sta_wpa2_ent_enable(&config);
    WiFi.begin(ssid); //connect to wifi
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      counter++;
      if (counter >= 60) { //after 30 seconds timeout - reset board (on unsucessful connection)
        ESP.restart();
      }
    }
    client.setCACert(test_root_ca);
    //client.setCertificate(test_client_key); // for client verification - certificate
    //client.setPrivateKey(test_client_cert);  // for client verification - private key
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address set: ");
    Serial.println(WiFi.localIP()); //print LAN IP  
  }
}

//run wifi connection
void checkWifi(){
  if (WiFi.status() == WL_CONNECTED) { //if we are connected to eduroam network
    counter = 0; //reset counter
    Serial.println("Wifi is still connected with IP: ");
    Serial.println(WiFi.localIP());   //inform user about his IP address
  } else if (WiFi.status() != WL_CONNECTED) { //if we lost connection, retry
    WiFi.begin(ssid);
  }
  while (WiFi.status() != WL_CONNECTED) { //during lost connection, print dots
    delay(500);
    Serial.print(".");
    counter++;
    if (counter >= 60) { //30 seconds timeout - reset board
      ESP.restart();
    }
  }
  /**
  Serial.print("Connecting to website: ");
  Serial.println(host);
  if (client.connect(host, 443)) {
    String url = "/rele/rele1.txt";
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: ESP32\r\n" + "Connection: close\r\n\r\n");
    while (client.connected()) {
      String header = client.readStringUntil('\n');
      Serial.println(header);
      if (header == "\r") {
        break;
      }
    }
    String line = client.readStringUntil('\n');
    Serial.println(line);
  } else {
    Serial.println("Connection unsucessful");
  }
  **/
  //delay(5000);
}

//get UUID if required
String getUUID(){
  Serial.println("Checking for toyboxUUID...");
  if (toyboxUUID==""){
    if ((WiFi.status() == WL_CONNECTED)){
      Serial.println("Getting new toyboxUUID...");
      HTTPClient http; // http GET code example from https://techtutorialsx.com/2017/05/19/esp32-http-get-requests/
      http.begin("https://www.uuidgenerator.net/api/version1");
      int httpCode = http.GET(); 
      if (httpCode > 0) { //Check for the returning code
        toyboxUUID = http.getString();
        Serial.println("httpCode is " + httpCode);
        Serial.println("toyboxUUID is " + toyboxUUID);
        return toyboxUUID;
      }
      else {
        Serial.println("Error on HTTP request");
      }
      http.end(); //Free the resources
    }
    else{
      Serial.println("No wifi connection, cannot get new toyboxUUID...");        
    }
  }
  else {
    Serial.println("Already have UUID");
  }
}

//Setup SPIFFS file system
void setupFileMount(){
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}

//write those records in list with data to file with UUID and timestamp of file write
void writeToFile(){
  File file = SPIFFS.open("/toyboxInfo.csv", FILE_WRITE);
  if (!file) {
    Serial.println("There was an error opening the file for writing");
    return;
  }
  if (file.println("TEST")) {
    file.println("This toybox's unique ID: ");
    file.println(toyboxUUID);
    file.println("Timestamp for this write: ");
    file.println(formattedDate = timeClient.getFormattedDate());
    for (int k=0; k<MAX_SIZE; k++){
      if(toyTags[k][1] != "*"){
        file.print(toyTags[k][0]);
        file.print(",");
        file.print(toyTags[k][1]);
        file.print(",");
        file.println(toyTags[k][2]);
      }
    }
    Serial.println("String was written to file successfully");
  } 
  else {
    Serial.println("String file write failed");
  }
  file.close();  
}

//Turn buzzer and LED on at same time
//Turn buzzer on - https://techtutorialsx.com/2017/07/01/esp32-arduino-controlling-a-buzzer-with-pwm/ - best duty cycle is of 50% (changing this will change volume)
////LED ON - https://www.instructables.com/id/Blinking-an-LED-With-ESP32/
void buzzerLEDON(){
    ledcWriteTone(channel, 2000);
    ledcWrite(channel, dutyCycle);
    digitalWrite (ledPin, HIGH);  // turn on the LED
    delay(3000);
    ledcWrite(channel, 0);//Should turn buzzer off
    digitalWrite (ledPin, LOW); // turn off the LED
}
