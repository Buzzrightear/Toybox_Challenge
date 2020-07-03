//Version 1 of hjas2 IoT sweetBox project

//Wifi connection code from: https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFiClientSecure/examples/WiFiClientSecureEnterprise/WiFiClientSecureEnterprise.ino
//Time stamp code from ESP32 library examples and NTPClient lib https://randomnerdtutorials.com/esp32-ntp-client-date-time-arduino-ide/ 


#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <WiFi.h>
#include "time.h"
#include <WiFiClientSecure.h>
#include "esp_wpa2.h"
#include <NTPClient.h> // 
#include <HTTPClient.h>
#include <EEPROM.h>


//Toybox scanner ID declaration
String sweetBoxUUID = "";

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

//Timers
int timer = 0;
int timer2 = 0;

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


//Buzzer declarations: https://techtutorialsx.com/2017/07/01/esp32-arduino-controlling-a-buzzer-with-pwm/
//https://techtutorialsx.com/2017/06/15/esp32-arduino-led-pwm-fading/
int freq = 500; //Changing this will change the tone
int channel = 0;
int resolution = 8; //This is the resolution (8 bits)
int dutyCycle = 50; //Should be the volume
const int buzzerPin=4; //this is the GPIO pin

//LED declarations
const int ledRedPin = 13;
const int ledGreenPin = 27;

//sweetBox latch declaration
const int latchPin1=14;//yellow wire
const int latchPin2=32;//orange wire
//const int latchPin3=15;//red wire
//const int latchPin4=33;//brown wire

//micro switch declaration
const int switchPin=21;//try 17 if this doesn't work

//sweetBox state
boolean locked = true;
int lidState = 0;         // variable for reading the switch status

void setup(void) {
  //Begin setup
  Serial.begin(115200);
  Serial.println("Hello! Starting setup");
  
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

  //connect to WiFi (Enterprise environment e.g. university)
  //wifiConnect();

  //get sweetBox UUID if required
  getUUID();
  
  //init and get the time
  timeClient.begin();
  timeClient.setTimeOffset(0); // Set offset time in seconds to adjust for your timezone
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
 
  //Setup Buzzer
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(buzzerPin, channel);

  //setup LED pin as a digital output pin
  pinMode (ledRedPin, OUTPUT);
  pinMode (ledGreenPin, OUTPUT);

  //setup latch
  pinMode(latchPin1,OUTPUT );
  pinMode(latchPin2,OUTPUT );
  //pinMode(latchPin3,OUTPUT );
  //pinMode(latchPin4,OUTPUT );
  
  //setup switch
  pinMode(switchPin, INPUT_PULLUP);//LOW = switch is compressed / HIGH = switch is released
  
  delay(1000);//Just need some time to reset the serial display on the computer
  
  
  //Test LEDs and buzzer
  buzzerLEDON();

  //set red LED on from the get go
  redLEDON();

  Serial.println("Setup complete");
}

//Main loop *****************************************
void loop(void) {
  timeClient.forceUpdate();//sometimes the NTP Client retrieves 1970. To ensure that doesn’t happen we need to force the update.
  //testLatch();
  checkLockSitch();  
  //testSwitch();
  delay(1000);
  timer++;  // we're increasing timer by 1000ms here
  if (timer >=5){
    checkWifi();
    timer = 0;
  }
 
}
//***************************************************

//test switch working
void testSwitch(){
  lidState = digitalRead(switchPin);
  Serial.println("Switch is " + String(lidState));  
}

//find correct config for latch
void testLatch(){
  /*
  int combos[][4] = {
  {1,1,1,1},
  {0,1,1,1},
  {1,0,1,1},
  {0,0,1,1},
  {1,1,0,1},
  {0,1,0,1},
  {1,0,0,1},
  {0,0,0,1},
  {1,1,1,0},
  {0,1,1,0},
  {1,0,1,0},
  {0,0,1,0},
  {1,1,0,0},
  {0,1,0,0},
  {1,0,0,0},
  {0,0,0,0}
  };
  */
  int combos[][2] = {
  {1,1},
  {0,1},
  {1,0},
  {0,0}
  };
  
  
  Serial.println("testLatch running");
  
  //for (int i=0;i<16;i++){
    for (int i=0;i<4;i++){
    int a;
    int b;
    int c;
    int d;
    if (combos[i][0]==0){
      a = LOW;
    }
    else{
      a = HIGH;
    }
     if (combos[i][1]==0){
      b = LOW;
    }
    else{
      b = HIGH;
    }
    /*
    if (combos[i][2]==0){
      c = LOW;
    }
    else{
      c = HIGH;
    }
     if (combos[i][3]==0){
      d = LOW;
    }
    else{
      d = HIGH;
    }
    Serial.println("Testing: " + String(a) +" " +String(b)+" "+String(c)+" "+String(d));
    */

    Serial.println("Testing: " + String(a) +" " +String(b));
    digitalWrite(latchPin1, a);
    digitalWrite(latchPin2, b);
    //digitalWrite(latchPin3, c);
    //digitalWrite(latchPin4, d);
    delay(1000);
    digitalWrite(latchPin1, LOW);
    digitalWrite(latchPin2, LOW);
    //digitalWrite(latchPin3, LOW);
    //digitalWrite(latchPin4, LOW);    
    
  }
 


}

//To get time:
  //formattedDate = timeClient.getFormattedDate();//dateTime stamp
  //Serial.println(formattedDate);
  /**If you want to get date and time separately, you need to split that string. The “T” letter separates the date from the time
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  Serial.println(dayStamp);
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.println(timeStamp);
  **/

//check lock situation - should latch be locked or unlocked?
void checkLockSitch(){
  Serial.println("checkLockSitch running");
  timer2++;
  if(!getLidState() && locked){ //lid closed and locked state is true (lid status - TRUE=OPEN / FALSE = CLOSED)
    if(shouldIUnlock()){ //if server says to unlock
      pulseLatch(); //unlock latch
      greenLEDAndBuzzerON(); //switch red LED off and green LED and buzzer on
      //start timer
      timer2=0;
    }
  }
  else if(!getLidState() && !locked ){ //if lid closed and locked state is false
    //check timer is over 60 seconds - if it is, lock
    if (timer2 >=60){
      pulseLatch();
    }
  }

}

//return true if server says to unlock, else false 
boolean shouldIUnlock(){
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
    HTTPClient http;   
    http.begin("http://helloworld-env.matddinqfi.eu-west-2.elasticbeanstalk.com/hello");  //***************************Specify destination for HTTP request
    http.addHeader("Content-Type",  "application/x-www-form-urlencoded");             //Specify content-type header    
    int httpResponseCode = http.POST("UNLOCK_REQUEST");   //Send the actual POST request
    Serial.println("Checking with server if I should unlock");
    if(httpResponseCode>0){
      String response = http.getString();                       //Get the response to the request
      Serial.println("Response code is " + httpResponseCode);   //Print return code
      Serial.println("response is" + response + "END");           //Print request answer
      if (response.startsWith("U")){
        Serial.println("returning true");           
        return true;  
      }
      else{
        Serial.println("returning false");
        return false;
      }
    }
    else{
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
      return false;
    }
    http.end();  //Free resources
  }
  else{
    Serial.println("Error in WiFi connection");   
    return false;
  }
}

//pulse Latch to open or close
void pulseLatch(){
  Serial.println("pulseLatch running");
  //pulse latch
  digitalWrite(latchPin1, HIGH);
  digitalWrite(latchPin2, LOW);
  //digitalWrite(latchPin3, HIGH);
  //digitalWrite(latchPin4, LOW);
  delay(1000);
  digitalWrite(latchPin1, LOW);
  digitalWrite(latchPin2, LOW);
  //digitalWrite(latchPin3, LOW);
  //digitalWrite(latchPin4, LOW);
  
  
  //and update status of locked state variable
  if (locked){
    Serial.println("Box should be unlocked");
    locked=false;
    digitalWrite(ledGreenPin, HIGH);
    digitalWrite(ledRedPin, LOW);
  }
  else{
    Serial.println("Box should be locked");
    locked=true;  
    digitalWrite(ledGreenPin, LOW);
    digitalWrite(ledRedPin, HIGH);
  }
}

//get lid status - TRUE=OPEN / FALSE = CLOSED
boolean getLidState(){
  lidState = digitalRead(switchPin); //read the state of the pushbutton value:
  if (lidState==HIGH){
    Serial.println("Lid should be open");
    return true;
  }
  else{
    Serial.println("Lid should be closed");
    return false;
  }
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
  Serial.println("Checking for sweetBoxUUID...");
  if (sweetBoxUUID==""){
    if ((WiFi.status() == WL_CONNECTED)){
      Serial.println("Getting new sweetBoxUUID...");
      HTTPClient http; // http GET code example from https://techtutorialsx.com/2017/05/19/esp32-http-get-requests/
      http.begin("https://www.uuidgenerator.net/api/version1");
      int httpCode = http.GET(); 
      if (httpCode > 0) { //Check for the returning code
        sweetBoxUUID = http.getString();
        Serial.println("httpCode is " + httpCode);
        Serial.println("sweetBoxUUID is " + sweetBoxUUID);
        return sweetBoxUUID;
      }
      else {
        Serial.println("Error on HTTP request");
      }
      http.end(); //Free the resources
    }
    else{
      Serial.println("No wifi connection, cannot get new sweetBoxUUID...");        
    }
  }
  else {
    Serial.println("Already have UUID");
  }
}

//Turn buzzer and LED on at same time
//Turn buzzer on - https://techtutorialsx.com/2017/07/01/esp32-arduino-controlling-a-buzzer-with-pwm/ - best duty cycle is of 50% (changing this will change volume)
////LED ON - https://www.instructables.com/id/Blinking-an-LED-With-ESP32/
void buzzerLEDON(){
    ledcWriteTone(channel, 2000);
    ledcWrite(channel, dutyCycle);
    digitalWrite (ledRedPin, HIGH);  // turn on the red LED
    digitalWrite (ledGreenPin, HIGH);  // turn on the green LED
    delay(1000);
    ledcWrite(channel, 0);//Should turn buzzer off
    digitalWrite (ledRedPin, LOW); // turn off the red LED
    digitalWrite (ledGreenPin, LOW); // turn off the green LED
}


void greenLEDAndBuzzerON(){
   Serial.println("greenLEDAndBuzzerON running");
   digitalWrite (ledRedPin, LOW); // turn off the red LED  
   ledcWriteTone(channel, 2000);
   ledcWrite(channel, dutyCycle);
   digitalWrite (ledGreenPin, HIGH);  // turn on the red LED
   delay(3000);
   ledcWrite(channel, 0);//turn buzzer off after 3 seconds
   digitalWrite (ledGreenPin, LOW);  // turn on the red LED
}

void redLEDON(){
  Serial.println("redLEDON running");
  ledcWrite(channel, 0);//Should turn buzzer off
  digitalWrite (ledGreenPin, LOW); // turn off the green LED  
  digitalWrite (ledRedPin, HIGH); // turn on the red LED  
}
