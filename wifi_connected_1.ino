#include <stdio.h>
#include <SPI.h>
#include <Wire.h>
#include <Pinger.h>
#include <ESP8266Ping.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

class test{
    int x;
  public:
    test (int i){
        x = i;
    }

};

extern "C"
{
  #include <lwip/icmp.h> // needed for icmp packet definitions
}

// Set global to avoid object removing after setup() routine
Pinger pinger;

void makeHeader(int mFirstscan = 0 , String mssid = "");
void cmd_Respons(void);

#define cmd_Row 17
#define CoName   "SHD.Co"
#define pVertion "version 1.0"

String ssid            = "";
String password        = "";
String incomingString  = ""; 
String mIP             = "";
String TIMEms;
const char* remote_host = "www.google.com";
const IPAddress remote_ip(172, 20, 10, 1);

int i;
int vCase = 0;
int FirstScan = 0;

void setup() {
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup done");
  display.clearDisplay();
  display.setTextSize(1);
  display.display();
  
   display.clearDisplay();
   display.display(); 
   delay(300);
   display.setTextSize(2);
   display.setTextColor(WHITE);
   display.setCursor(0 , 0);
   display.println("Networks");
   display.println("Connection");
   display.display();
   display.setTextSize(1);
   display.println(pVertion);
   display.println(CoName);
   display.display(); 
   delay(2000);
   display.clearDisplay();
   display.display(); 
   delay(300);
   Serial.println("command >");
   display.setTextSize(1);
   display.setTextColor(WHITE);
   makeHeader(FirstScan , ssid);
   display.setCursor(0 , cmd_Row);
   display.print("command > ");
   display.display();
}
 
void loop() {
  if (Serial.available() > 0) {
      incomingString = Serial.readString();
      
      if(!incomingString)                   vCase = 113;
      if(incomingString == "scan\n")        vCase = 1;
      if(incomingString == "connect\n")     vCase = 2;
      if(incomingString == "ping\n")        vCase = 3;
      if(incomingString == "pingone\n")     vCase = 4;
      if(incomingString == "disconnect\n")  vCase = 5;
      if(incomingString == "reset\n")       vCase = 111;
      if(incomingString == "clear scr\n")   vCase = 112;
       
      switch(vCase) {
           //scan
           case 1 :{
              if (FirstScan == 0) FirstScan = 1;
              Serial.println("Scanning...");
              Serial.println("scan done");
              
              display.setCursor(60 , cmd_Row);
              display.print("Scanning...");
              display.display(); 
              delay(500);
              int n = WiFi.scanNetworks();
              if (n == 0) {
                Serial.println("No networks found");
                display.setCursor(10 , cmd_Row);
                display.print("No networks found");
                display.display(); 
                delay(500);
              } else {
                Serial.print(n);
                Serial.println("Networks found");
                display.clearDisplay();
                makeHeader(FirstScan , ssid);
                display.setCursor(0 ,cmd_Row);
                display.print("   Network found : ");
                display.display();
                for (int i = 0 ; i < n; ++i) {
                  Serial.print(i + 1);
                  Serial.print(": ");
                  Serial.print(WiFi.SSID(i));
                  Serial.print(" (");
                  Serial.print(WiFi.RSSI(i));
                  Serial.print(")");
                  Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
                  delay(10);
                  display.setCursor(0 ,cmd_Row);
                  display.println(n);
                  display.display();
                  display.println(WiFi.SSID(0));
                  display.println(WiFi.SSID(1));
                  display.println(WiFi.SSID(2));
                  display.println(WiFi.SSID(3));
                  display.println(WiFi.SSID(4));
                  //display.setCursor(0,0);
                  display.display();
                  //display.clearDisplay();
                }
                makeHeader(FirstScan , ssid);
              }
           }
           break;

           //connect
           case 2 :{
              if (FirstScan != 0) { 
                    Serial.println("Connnected to network");
                    Serial.println("Enter netWork SSID:");
                    display.clearDisplay();
                    display.setCursor(0,cmd_Row);
                    display.print("Enter netWork SSID:");
                    display.display();
                    makeHeader(FirstScan , ssid);
                    cmd_Respons();
                    ssid = incomingString;
                    ssid.trim();
                    Serial.println("Enter netWork PASS:");
                    display.clearDisplay();
                    display.setCursor(0,cmd_Row);
                    display.print("Enter netWork PASS:");
                    display.display();
                    makeHeader(FirstScan , ssid);
                    cmd_Respons();
                    password = incomingString;
                    password.trim();
                    bool stationConnected = WiFi.begin(ssid, password);
                    while (WiFi.status() != WL_CONNECTED) {
                      delay(100);
                      Serial.print(".");
                    }
                    FirstScan = 2;
                    Serial.println();
                    Serial.print("WiFi connected with ip ");  
                    Serial.println(WiFi.localIP());
                    display.clearDisplay();
                    display.setCursor(0,cmd_Row);
                    display.println("WIFI Connected ");
                    display.println(WiFi.localIP());
                    display.display();
                    makeHeader(FirstScan , ssid);
                    }
                    else 
                    {
                    Serial.println("You should be frist scan netwok.");
                    display.clearDisplay();
                    display.setCursor(0,cmd_Row);
                    display.print("You should be frist scan netwok");
                    display.display();
                    makeHeader(FirstScan , ssid);
                 
                }
           }
           break;           
           
           //ping
           case 3 :{
               if (FirstScan == 2) { 
                  Serial.println("Ping");
                  Serial.println("Enter IP address:");
                  display.clearDisplay();
                  display.setCursor(0,cmd_Row);
                  display.print("Enter IP address:");
                  display.display();
                  makeHeader(FirstScan , ssid);
                  cmd_Respons();
                  mIP = incomingString;
                  mIP.trim();
                  Serial.print("Pinging ip ");
                  Serial.println(mIP);
                  Serial.println(remote_ip);
                  display.println(mIP);
                  display.display();
                  pinger.OnReceive([](const PingerResponse& response)
                  {
                    if (response.ReceivedResponse)
                    {
                      Serial.printf(
                        "Reply from %s: bytes=%d time=%lums TTL=%d\n",
                        response.DestIPAddress.toString().c_str(),
                        response.EchoMessageSize - sizeof(struct icmp_echo_hdr),
                        response.ResponseTime,
                        response.TimeToLive);
                    }
                    else
                    {
                      Serial.printf("Request timed out.\n");
                    }
                
                    // Return true to continue the ping sequence.
                    // If current event returns false, the ping sequence is interrupted.
                    return true;
                  });
                  
                  pinger.OnEnd([](const PingerResponse& response)
                  {
                    // Evaluate lost packet percentage
                    float loss = 100;
                    if(response.TotalReceivedResponses > 0)
                    {
                      loss = (response.TotalSentRequests - response.TotalReceivedResponses) * 100 / response.TotalSentRequests;
                    }
                    
                    // Print packet trip data
                    Serial.printf(
                      "Ping statistics for %s:\n",
                      response.DestIPAddress.toString().c_str());
                    Serial.printf(
                      "    Packets: Sent = %lu, Received = %lu, Lost = %lu (%.2f%% loss),\n",
                      response.TotalSentRequests,
                      response.TotalReceivedResponses,
                      response.TotalSentRequests - response.TotalReceivedResponses,
                      loss);
                
                    // Print time information
                    if(response.TotalReceivedResponses > 0)
                    {
                      Serial.printf("Approximate round trip times in milli-seconds:\n");
                      Serial.printf(
                        "    Minimum = %lums, Maximum = %lums, Average = %.2fms\n",
                        response.MinResponseTime,
                        response.MaxResponseTime,
                        response.AvgResponseTime);
                    }
                                        
                    // Print host data
                    Serial.printf("Destination host data:\n");
                    Serial.printf(
                      "    IP address: %s\n",
                      response.DestIPAddress.toString().c_str());
                    if(response.DestMacAddress != nullptr)
                    {
                      Serial.printf(
                        "    MAC address: " MACSTR "\n",
                        MAC2STR(response.DestMacAddress->addr));
                    }
                    if(response.DestHostname != "")
                    {
                      Serial.printf(
                        "    DNS name: %s\n",
                        response.DestHostname.c_str());
                    }
                
                    return true;
                  });
                  
                  //Ping ip address
                  Serial.printf(
                    "\n\nPinging default gateway with IP %s",
                    mIP);
                  if(pinger.Ping(mIP) == false)
                  {
                    Serial.println("Error during last ping command.");
                  }
                  delay(10000);
                                 
              }
              else {
                      Serial.println("You should be frist connect to netwok.");
                      display.clearDisplay();
                      display.setCursor(0,cmd_Row);
                      display.print("You should be frist scan netwok");
                      display.display();
                  }
           }
           makeHeader(FirstScan , ssid);
           break;           
          
           //ping1
           case 4 :{
               if (FirstScan == 2) { 
                  Serial.println("Ping");
                  Serial.println("Enter IP address:");
                  display.clearDisplay();
                  display.setCursor(0,cmd_Row);
                  display.print("Enter IP address:");
                  display.display();
                  makeHeader(FirstScan , ssid);
                  cmd_Respons();
                  mIP = incomingString;
                  mIP.trim();
                  Serial.print("Pinging ip ");
                  Serial.println(mIP);
                  Serial.println(remote_ip);
                  display.println(mIP);
                  display.display();
                  
                  if(Ping.ping(remote_ip)) {
                      i = Ping.averageTime();
                      Serial.println(i+" ms");
                      Serial.println("Success !");
                      display.clearDisplay();
                      display.display();
                      makeHeader(FirstScan , ssid);
                      display.setCursor(0,cmd_Row);
                      display.println(" ");
                      display.println(i + " ms");
                      display.println("Success !");
                      
                  } else {
                      Serial.println("Error :(");
                      display.setCursor(0,20);
                      display.println("Error  :(");
                  }
                  display.display();
               }
               else {
                  Serial.println("You should be frist connect to netwok.");
                  display.clearDisplay();
                  display.setCursor(0,cmd_Row);
                  display.print("You should be frist scan netwok");
                  display.display();
               }
             }
             makeHeader(FirstScan , ssid);
             break;           

           //wifi disconnecet
           case 5:{
                if( FirstScan == 2 ){   
                    FirstScan = 1;
                    if (WiFi.status() == WL_CONNECTED) {
                        WiFi.disconnect();
                        Serial.println("WiFi is disconnecting");
                        display.clearDisplay();
                        display.setCursor(0 , cmd_Row );
                        display.print("command > ");
                        display.display();
                        makeHeader(FirstScan,"");
                    }
                                   
                }else{
                    Serial.println ("No WiFi connection found!");                
                    display.clearDisplay();
                    display.setCursor(0 , cmd_Row );
                    display.println("No WiFi connection found!");
                    display.display();
               }
           }
           
           //reset 
           case 111 :{
                    if (FirstScan != 0){
                          FirstScan = 0;
                          if (WiFi.status() == WL_CONNECTED) {
                              WiFi.disconnect();
                          }
                          Serial.println("Reset");
                          display.clearDisplay();
                          display.setCursor(0 , cmd_Row );
                          display.print("RESET");
                          display.display();
                          delay(2000);
                          display.clearDisplay();
                          display.setCursor(0 , cmd_Row );
                          display.print("command > ");
                          display.display();
                    }
                    makeHeader(FirstScan,"");
           }
           break;

           //clear display
           case 112 :{
              Serial.println("clear Display");
              display.clearDisplay();
              display.setCursor(0 , cmd_Row );
              display.print("command > ");
              display.display();
              makeHeader(FirstScan , ssid);
           }
           break;

           default:{
               if( vCase != 113) {
               Serial.println("Command not found.");
               display.clearDisplay();
               display.display(); 
               delay(300);
               display.setCursor(0 , cmd_Row);
               display.print("Command not found.");
               display.display(); 
               delay(2000);
               Serial.println("command >");
               display.clearDisplay();
               display.display(); 
               delay(300);
               display.setCursor(0 , cmd_Row );
               display.print("command > ");
               display.display();
                makeHeader(FirstScan , ssid);
               } 
                    
           }
              break;
      }
  }
  vCase = 0;
  delay(50);
}

void makeHeader(int mFirstscan , String mssid ){
   if(mFirstscan != 2){
    display.setCursor(0 ,0);
    display.println("Network connection");
    display.print(pVertion);
    display.display();
   }else  {
    display.setCursor(0 ,0);
    display.println(mssid);
    display.println(WiFi.localIP());
    display.display();
    }
}

void cmd_Respons(void){
      while (!Serial.available() > 0) ;
      incomingString = Serial.readString();
      if(!incomingString) {
        display.clearDisplay();
        display.display();
        delay(50);
        display.setCursor(0 , cmd_Row );
        display.print("command > ");
        display.display();
        makeHeader(FirstScan , ssid);
      }else{
        Serial.println(incomingString);
        display.setCursor(0 , cmd_Row+10 );
        display.print(incomingString);
        display.display();
        delay(50);
      }
}
