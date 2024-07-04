#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <DHT.h>  // Include the DHT sensor library
#include <Arduino.h>
#include <FanMonitor.h>
#include "PMS.h"
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "Adafruit_ILI9341.h"

TFT_eSPI tft = TFT_eSPI();
uint16_t touch_x = 0, touch_y = 0;
uint16_t button_color = TFT_BLUE;
uint16_t button_pressed_color = TFT_RED;

#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   15  // Chip select control pin
#define TFT_DC    2  // Data Command control pin
#define TFT_RST   4  // Reset pin (could connect to RST pin)

// Touch screen settings
#define TOUCH_CS  5  // Chip select pin for touch screen

#define BUTTON1_X 23
#define BUTTON1_Y 240
#define BUTTON1_W 189
#define BUTTON1_H 40

#define BUTTON2_X 24
#define BUTTON2_Y 280
#define BUTTON2_W 79
#define BUTTON2_H 40

#define BUTTON3_X 134
#define BUTTON3_Y 280
#define BUTTON3_W 79
#define BUTTON3_H 40

HardwareSerial pmsSerial(2); // Define Serial2 on UART 2
#define RX_PIN 16 // Define the pin connected to RX on ESP32
#define TX_PIN 17 // Define the pin connected to TX on ESP32

// Define DHT sensor parameters
#define DHTPIN 14
#define DHTTYPE DHT22
float ppm;
float ppm2;
int PM25=0;
int PM252;
float temperature;
float humidity;

//fuzzy
float SSCO, SCO, MCO, BCO, SBCO;
float SSPM, SPM, MPM, BPM, SBPM;
float SPKK,PKK,PKK1,PKK2,PKK3,MKK,MKK1,MKK2,MKK3,MKK4,MKK5,CKK,CKK1,CKK2,CKK3,CKK4,CKK5,CKK6,CKK7;
float SCKK,SCKK1,SCKK2,SCKK3,SCKK4,SCKK5,SCKK6,SCKK7,SCKK8,SCKK9,HA,HAauto,HAauto2;

const int FAN_OUTPUT_PIN = 25;

const int DELAY_MS = 100;  // delay between fade increments
const int tachoPin = 26;
volatile unsigned long counter = 0;
int rpm =0;
FanMonitor _fanMonitor = FanMonitor(tachoPin, FAN_TYPE_BIPOLE);

//Sensor CO
#define AO_PIN 33
float RS_gas = 0;
float ratio = 0;
float sensorValue = 0;
float sensor_volt = 0;
float R0 = 1500.0;

// Define WiFi credentials
#define WIFI_SSID "Hharison"
#define WIFI_PASSWORD "Hharison8"

// Define Firebase API Key, Project ID, and user credentials
#define API_KEY "AIzaSyDDECbWk7z8Oo_US1cNwhx8OUwSmhVh_e8"
#define FIREBASE_PROJECT_ID "ta-air-purifier"
#define USER_EMAIL "test123@gmail.com"
#define USER_PASSWORD "test123"

// Define Firebase Data object, Firebase authentication, and configuration
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Initialize the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  tft.begin();
  tft. init();
  tft.fillScreen(TFT_BLACK);
  
  pmsSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  _fanMonitor.begin();

  // Initialize the DHT sensor
  dht.begin();

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Print Firebase client version
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  // Assign the API key
  config.api_key = API_KEY;

  // Assign the user sign-in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the callback function for the long-running token generation task
  config.token_status_callback = tokenStatusCallback;  // see addons/TokenHelper.h

  // Begin Firebase with configuration and authentication
  Firebase.begin(&config, &auth);

  // Reconnect to Wi-Fi if necessary
  Firebase.reconnectWiFi(true);

  Serial1.begin(9600);
}

struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};
 
struct pms5003data data;

void drawButtons() {
  // Draw Button 1 (Power Button)
  // if (powerState) {
  //   tft.fillRoundRect(BUTTON1_X, BUTTON1_Y, BUTTON1_W, BUTTON1_H, 10, TFT_GREEN); // On state
  //   tft.setTextColor(TFT_BLACK);
  //   tft.setTextSize(2);
  //   tft.drawCentreString("ON", BUTTON1_X + BUTTON1_W / 2, BUTTON1_Y + BUTTON1_H / 2 - 15, 2);
  // } else {
  //   tft.fillRoundRect(BUTTON1_X, BUTTON1_Y, BUTTON1_W, BUTTON1_H, 10, TFT_RED); // Off state
  //   tft.setTextColor(TFT_BLACK);
  //   tft.setTextSize(2);
  //   tft.drawCentreString("OFF", BUTTON1_X + BUTTON1_W / 2, BUTTON1_Y + BUTTON1_H / 2 - 15, 2);
  // }

  // Draw Button 2 (Mode Button)
  // if (autoMode) {
  //   tft.fillRoundRect(BUTTON2_X, BUTTON2_Y, BUTTON2_W, BUTTON2_H, 10, TFT_BLUE); // Auto mode
  // } else {
  //   tft.fillRoundRect(BUTTON2_X, BUTTON2_Y, BUTTON2_W, BUTTON2_H, 10, TFT_ORANGE); // Manual mode
  // }
  // tft.setTextColor(TFT_BLACK);
  // tft.setTextSize(2);
  // tft.drawCentreString("Mode", BUTTON2_X + BUTTON2_W / 2, BUTTON2_Y + BUTTON2_H / 2 - 15, 2);

  // Draw Fan Button
  // if (autoMode) {
  //   tft.fillRoundRect(BUTTON3_X, BUTTON3_Y, BUTTON3_W, BUTTON3_H, 10, TFT_DARKGREY); // Disabled state
  //   tft.setTextColor(TFT_LIGHTGREY);
  // } else {
  //   tft.fillRoundRect(BUTTON3_X, BUTTON3_Y, BUTTON3_W, BUTTON3_H, 10, TFT_GREEN); // Default state
  //   tft.setTextColor(TFT_BLACK);
  // }
  // tft.setTextSize(2);
  // tft.drawCentreString("Fan: " + String(fanSpeed), BUTTON3_X + BUTTON3_W / 2, BUTTON3_Y + BUTTON3_H / 2 - 9, 1); // Display fan speed
}

void loop() {
  static unsigned long lastSensorReadTime = 0;
  static unsigned long lastSaveTime = 0;
  SSCO = 0, SCO = 0, MCO = 0, BCO = 0, SBCO = 0;
  SSPM = 0, SPM = 0, MPM = 0, BPM = 0, SBPM = 0;
  SPKK = 0,PKK = 0,PKK1 = 0,PKK2 = 0,PKK3 = 0,MKK = 0,MKK1 = 0,MKK2 = 0,MKK3 = 0,MKK4 = 0,MKK5 = 0,CKK = 0,CKK1 = 0,CKK2 = 0,CKK3 = 0,CKK4 = 0,CKK5 = 0,CKK6 = 0,CKK7 = 0;
  SCKK = 0,SCKK1 = 0,SCKK2 = 0,SCKK3 = 0,SCKK4 = 0,SCKK5 = 0,SCKK6 = 0,SCKK7 = 0,SCKK8 = 0,SCKK9 = 0,HA = 0;
  int HA100=0,HA255=0;
  
  // Define the path to the Firestore document
  String documentPath = "EspData/DHT11";
  String documentPath2 = "EspData/pm";
  String documentPath3 = "EspData/co";
  String documentPath4 = "EspData/hum";
  String documentPath5 = "EspData/temp";
  String documentPath6 = "EspData/Sent From Mobile";
  String Path = "EspData";

  // Create a FirebaseJson object for storing data
  FirebaseJson content;

  if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", Path.c_str(), "")) {
    // Stream& input;
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, fbdo.payload().c_str());

    if (!error) {
      //data firestore Sent From Mobile
      JsonObject documents_0 = doc["documents"][0];
      const char* documents_0_name = documents_0["name"];
      
      for (JsonPair documents_0_field : documents_0["fields"].as<JsonObject>()) {
        unsigned char CO_FS = documents_0["fields"]["CO"]["stringValue"];
        unsigned char Humidity_FS = documents_0["fields"]["Humidity"]["stringValue"];
        unsigned char PM25_FS = documents_0["fields"]["PM25"]["stringValue"];
        unsigned char Temperature_FS = documents_0["fields"]["Temperature"]["stringValue"];
        bool documents_0_fields_powerState_booleanValue = documents_0["fields"]["powerState"]["booleanValue"];
        bool documents_0_fields_autoMode_booleanValue = documents_0["fields"]["autoMode"]["booleanValue"];
        int documents_0_fields_sliderValue_integerValue = documents_0["fields"]["sliderValue"]["integerValue"];
    
        if (documents_0_fields_powerState_booleanValue == true ){
          
          static unsigned long lasttime = 0;
          if (millis() - lasttime > 10000){
            lasttime = millis();
            // Read temperature and humidity from the DHT sensor
            float temperature = dht.readTemperature();
            float humidity = dht.readHumidity();
            sensorValue = analogRead(AO_PIN);
            sensor_volt = sensorValue/1024*5.0;
            RS_gas = (5.0-sensor_volt)/sensor_volt;
            ratio = RS_gas/R0;
            float x = 1538.46 * ratio;
            ppm2 = ppm;
            Serial.println(ppm2);
            ppm = pow(x,-1.709);
            if (isnan(ppm)){
              ppm = ppm2;        
            }
            
            //PM2.5
            if (readPMSdata(&pmsSerial)) {
              // reading data was successful!
              Serial.println();
              Serial.println("---------------------------------------");
              Serial.println("Concentration Units (standard)");
              Serial.print("PM 1.0: "); Serial.print(data.pm10_standard);
              Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_standard);
              Serial.print("\t\tPM 10: "); Serial.println(data.pm100_standard);
              Serial.println("---------------------------------------");
              Serial.println("Concentration Units (environmental)");
              Serial.print("PM 1.0: "); Serial.print(data.pm10_env);
              Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_env);
              Serial.print("\t\tPM 10: "); Serial.println(data.pm100_env);
              Serial.println("---------------------------------------");
              Serial.print("Particles > 0.3um / 0.1L air:"); Serial.println(data.particles_03um);
              Serial.print("Particles > 0.5um / 0.1L air:"); Serial.println(data.particles_05um);
              Serial.print("Particles > 1.0um / 0.1L air:"); Serial.println(data.particles_10um);
              Serial.print("Particles > 2.5um / 0.1L air:"); Serial.println(data.particles_25um);
              Serial.print("Particles > 5.0um / 0.1L air:"); Serial.println(data.particles_50um);
              Serial.print("Particles > 10.0 um / 0.1L air:"); Serial.println(data.particles_100um);
              Serial.println("---------------------------------------");
            }
            PM252 = PM25;
            Serial.println(PM252); 
            PM25 = data.pm25_env;
            if (PM25>=250){
              PM25 = 250;
            }
            else if (PM25 == 0){
              PM25 = PM252;
            }
            
            // Print temperature and humidity values
            Serial.println(temperature);
            Serial.println(humidity);
            Serial.println(ppm); 
            Serial.println(PM25);

            // Check if the values are valid (not NaN)
            if (!isnan(temperature) && !isnan(humidity) && !isnan(PM25) && !isnan(PM252)) {
              // Set the 'Temperature' and 'Humidity' fields in the FirebaseJson object
              content.set("fields/Temperature/stringValue", String(temperature,1));
              content.set("fields/Humidity/stringValue", String(humidity,1));
              if (isnan(ppm)){
                content.set("fields/CO/stringValue", String(ppm,2));
                content.set("fields/CO_0/stringValue", String(ppm,1));
              }
              else{
                content.set("fields/CO/stringValue", String(ppm2,2));
                content.set("fields/CO_0/stringValue", String(ppm2,1));
              }
              if (PM25 == 0){
                content.set("fields/PM25/stringValue", String(PM252));
                content.set("fields/PM25_0)/stringValue", String(PM252));
              }
              else {
                content.set("fields/PM25/stringValue", String(PM25));
                content.set("fields/PM25_0/stringValue", String(PM25));
              }
              Serial.print("Update/Add DHT Data... ");
          
              // Use the patchDocument method to update the Temperature and Humidity Firestore document
              if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "Temperature") && Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "Humidity") && Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "CO") && Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "PM25")) {
                Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
              }
              if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath2.c_str(), content.raw(), "PM25_0")){
                // Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
              }  
              if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath3.c_str(), content.raw(), "CO_0")){
                // Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
              }  
              else {
                Serial.println(fbdo.errorReason());
              }
            } 
            else {
              Serial.println("Failed to read DHT data.");
            }
            for (int i = 11; i > 0; i--){
              String(currentField)  = "PM_" + String(i);
              String previousField = "PM_" + String(i - 1);
              content.set("fields/" + currentField + ")/stringValue", String(PM252));
              content.set("fields/currentField/stringValue", String(PM252));
            }
          }
            if (documents_0_fields_autoMode_booleanValue ==  true){
              static unsigned long lasttime = 0;
              if (millis() - lasttime > 10000){
                lasttime = millis();
                //Fussyfikasi CO
                //SSCO
                if (ppm <= 5){SSCO = 1;}
                if (ppm > 5 && ppm < 10){SSCO = (10-ppm)/5.00;}
                if (ppm > 10){SSCO = 0;}
                //SCO
                if (ppm == 10){SCO = 1;}
                if (ppm > 5 && ppm < 10){SCO = (ppm-5)/5.00;}
                if (ppm > 10 && ppm < 15){SCO = (15-ppm)/5.00;}
                if (ppm > 15){SCO = 0;}
                //MCO
                if (ppm == 15){MCO = 1;}
                if (ppm > 10 && ppm < 15){MCO = (ppm-10)/5.00;}
                if (ppm > 15 && ppm < 20){MCO = (20-ppm)/5.00;}
                if (ppm > 20){MCO = 0;}
                //BCO
                if (ppm == 20){BCO = 1;}
                if (ppm > 15 && ppm < 20){BCO = (ppm-15)/5.00;}
                if (ppm > 20 && ppm < 25){BCO = (25-ppm)/5.00;}
                if (ppm > 25){BCO = 0;}
                //SBCO
                if (ppm >= 25){SBCO = 1;}
                if (ppm > 20 && ppm < 25){SBCO = (ppm-20)/5.00;}
                if (ppm < 20){SBCO = 0;}
              
                //Fussyfikasi PM2.5
                //SSPM
                if (PM25 <= 8){SSPM = 1;}
                if (PM25 > 8 && PM25 < 18){SSPM = (18-PM25)/10.00;}
                if (PM25 > 18){SSPM = 0;}
                //SPM
                if (PM25 == 18){SPM = 1;}
                if (PM25 > 8 && PM25 < 18){SPM = (PM25-8)/10.00;}
                if (PM25 > 18 && PM25 < 40){SPM = (40-PM25)/22.00;}
                if (PM25 > 40){SPM = 0;}
                //MPM
                if (PM25 == 40){MPM = 1;}
                if (PM25 > 18 && PM25 < 40){MPM = (PM25-18)/22.00;}
                if (PM25 > 40 && PM25 < 65){MPM = (65-PM25)/25.00;}
                if (PM25 > 65){MPM = 0;}
                //BPM
                if (PM25 == 65){BPM = 1;}
                if (PM25 > 40 && PM25 < 65){BPM = (PM25-40)/25.00;}
                if (PM25 > 65 && PM25 < 180){BPM = (180-PM25)/115.00;}
                if (PM25 > 180){BPM = 0;}
                //SBPM
                if (PM25 >= 180){SBPM = 1;}
                if (PM25 > 65 && PM25 < 180){SBPM = (PM25-65)/115.00;}
                if (PM25 < 250){SBPM = 0;}

                Serial.println(PM25); 
                Serial.print("SSCO=");
                Serial.println(SSCO);
                Serial.print("SCO=");
                Serial.println(SCO);
                Serial.print("MCO=");
                Serial.println(MCO);
                Serial.print("BCO=");
                Serial.println(BCO);
                Serial.print("SBCO=");
                Serial.println(SBCO);
                Serial.print("SSPM=");
                Serial.println(SSPM);
                Serial.print("SPM=");
                Serial.println(SPM);
                Serial.print("MPM=");
                Serial.println(MPM);
                Serial.print("BPM=");
                Serial.println(BPM);
                Serial.print("SBPM=");
                Serial.println(SBPM);
                
                //rules
                //1
                if (SSCO != 0 && SSPM != 0){SPKK = min(SSCO,SSPM);}
                //2
                if (SCO != 0 && SSPM != 0){PKK1 = min(SCO,SSPM);}
                //3
                if (SSCO != 0 && SPM != 0){PKK2 = min(SSCO,SPM);}
                //4
                if (SCO != 0 && SPM != 0){PKK3 = min(SCO,SPM);}
                //5
                if (MCO != 0 && SSPM != 0){MKK1 = min(MCO,SSPM);}
                //6
                if (MCO != 0 && SPM != 0){MKK2 = min(MCO,SPM);}
                //7
                if (MCO != 0 && MPM != 0){MKK3 = min(MCO,MPM);}
                //8
                if (SSCO != 0 && MPM != 0){MKK4 = min(SSCO,MPM);}
                //9
                if (SCO != 0 && MPM != 0){MKK5 = min(SCO,MPM);}
                //10
                if (BCO != 0 && SSPM != 0){CKK1 = min(BCO,SSPM);}
                //11
                if ( BCO != 0 && SPM != 0){CKK2 = min(BCO,SPM);}
                //12
                if (BCO != 0 && MPM != 0){CKK3 = min(BCO,MPM);}
                //13
                if (BCO != 0 && BPM != 0){CKK4 = min(BCO,BPM);}
                //14
                if (SSCO != 0 && BPM != 0){CKK5 = min(SSCO,BPM);}
                //15
                if (SCO != 0 && BPM != 0){CKK6 = min(SCO,BPM);}
                //16
                if (MCO != 0 && BPM != 0){CKK7 = min(MCO,BPM);}
                //17
                if (SBCO != 0 && SSPM != 0){SCKK1 = min(SBCO,SSPM);}
                //18
                if (SBCO != 0 && SPM != 0){SCKK2 = min(SBCO,SPM);}
                //19
                if (SBCO != 0 && MPM != 0){SCKK3 = min(SBCO,MPM);}
                //20
                if (SBCO != 0 && BPM != 0){SCKK4 = min(SBCO,BPM);}
                //21
                if (SBCO != 0 && SBPM != 0){SCKK5 = min(SBCO,SBPM);}
                //22
                if (BCO != 0 && SBPM != 0){SCKK6 = min(BCO,SBPM);}
                //23
                if (MCO != 0 && SBPM != 0){SCKK7 = min(MCO,SBPM);}
                //24
                if (SCO != 0 && SBPM != 0){SCKK8 = min(SCO,SBPM);}
                //25
                if (SSCO != 0 && SBPM != 0){SCKK9 = min(SSCO,SBPM);}
                Serial.print("SPKK=");
                Serial.println(SPKK);
                Serial.print("PKK1=");
                Serial.println(PKK1);
                Serial.print("PKK2=");
                Serial.println(PKK2);
                Serial.print("PKK3=");
                Serial.println(PKK3);
                Serial.print("MKK1=");
                Serial.println(MKK1);
                Serial.print("MKK2=");
                Serial.println(MKK2);
                Serial.print("MKK3=");
                Serial.println(MKK3);
                Serial.print("MKK4=");
                Serial.println(MKK4);
                Serial.print("MKK5=");
                Serial.println(MKK5);
                Serial.print("CKK1=");
                Serial.println(CKK1);
                Serial.print("CKK2=");
                Serial.println(CKK2);
                Serial.print("CKK3=");
                Serial.println(CKK3);
                Serial.print("CKK4=");
                Serial.println(CKK4);
                Serial.print("CKK5=");
                Serial.println(CKK5);
                Serial.print("CKK6=");
                Serial.println(CKK6);
                Serial.print("CKK7=");
                Serial.println(CKK7);
                Serial.print("SCKK1=");
                Serial.println(SCKK1);
                Serial.print("SCKK2=");
                Serial.println(SCKK2);
                Serial.print("SCKK3=");
                Serial.println(SCKK3);
                Serial.print("SCKK4=");
                Serial.println(SCKK4);
                Serial.print("SCKK5=");
                Serial.println(SCKK5);
                Serial.print("SCKK6=");
                Serial.println(SCKK6);
                Serial.print("SCKK7=");
                Serial.println(SCKK7);
                Serial.print("SCKK8=");
                Serial.println(SCKK8);
                Serial.print("SCKK9=");
                Serial.println(SCKK9);
              
                PKK = max(PKK1,max(PKK2,PKK3));
                MKK = max(MKK1,max(MKK2,max(MKK3,max(MKK4,MKK5))));
                CKK = max(CKK1,max(CKK2,max(CKK3,max(CKK4,max(CKK5,max(CKK6,CKK7))))));
                SCKK = max(SCKK1,max(SCKK2,max(SCKK3,max(SCKK4,max(SCKK5,max(SCKK6,max(SCKK7,max(SCKK8,SCKK9))))))));
              
                Serial.print("SPKK=");
                Serial.println(SPKK);
                Serial.print("PKK=");
                Serial.println(PKK);
                Serial.print("MKK=");
                Serial.println(MKK);
                Serial.print("CKK=");
                Serial.println(CKK);
                Serial.print("SCKK=");
                Serial.println(SCKK);
              
                float HA1=0,HA2=0,HA3=0;
                //Defuzzyfikasi
                if (SPKK != 0){
                  //((1400.00*SPKK)/(SPKK*7.00));
                  HA=  1400.00*SPKK;
                  HA1= SPKK*7.00;
                  HA2= HA/HA1;
                  if (PKK != 0){
                    //(((1400.00*SPKK) + (7000.00*PKK))/((SPKK*7.00)+(PKK*7.00)));
                    HA= (1400.00*SPKK) + (7000.00*PKK);
                    HA1= (SPKK*7.00)+(PKK*7.00);
                    HA2= HA/HA1;
                  }
                }
                if (PKK != 0){
                  //((4200.00*PKK)/(PKK*7.00));
                  HA=  4200.00*PKK;
                  HA1= PKK*7.00;
                  HA2= HA/HA1;
                  if (MKK != 0){
                    //(((4200.00*PKK) + (9800.00*MKK))/((PKK*7.00)+(MKK*7.00)));
                    HA= (4200.00*PKK) + (9800.00*MKK);
                    HA1= (PKK*7.00)+(MKK*7.00);
                    HA2= HA/HA1;
                  }
                }
                if (MKK !=0){
                  //((7000.00*MKK)/(MKK*7.00));
                  HA=  7000.00*MKK;
                  HA1= MKK*7.00;
                  HA2= HA/HA1;
                  if (CKK != 0){
                    //(((7000.00*MKK) + (12600.00*CKK))/((MKK*7.00)+(CKK*7.00)));
                    HA= (7000.00*MKK) + (12600.00*CKK);
                    HA1= (PKK*7.00)+(MKK*7.00);
                    HA2= HA/HA1;
                  }
                }
                if (CKK != 0){
                  //((12600.00*CKK)/(CKK*7.00));
                  HA=  12600.00*CKK;
                  HA1= CKK*7.00;
                  HA2= HA/HA1;
                  if (SCKK !=0){
                    //(((9800.00*CKK) + (15400.00*SCKK))/((CKK*7.00)+(SCKK*7.00)));
                    HA= (9800.00*CKK) + (15400.00*SCKK);
                    HA1= (CKK*7.00)+(SCKK*7.00);
                    HA2= HA/HA1;
                  }
                }
                if (SCKK != 0){
                  //HA =  ((15400.00*SCKK)/(SCKK*7.00));
                  HA=  15400.00*SCKK;
                  HA1= SCKK*7.00;
                  HA2= HA/HA1;
                }
              
                //((HA2/2400.00)*255.00);
                HA3 = HA2/2400.00;
                HA255 = HA3*255;
                //((HA2/2400.00)*100.00);
                HA3 = HA2/2400.00;
                HA100 = HA3*100;
                
                Serial.print("HA100=");
                Serial.println(HA100);
                Serial.print("HA255=");
                Serial.println(HA255);
                
                analogWrite(FAN_OUTPUT_PIN, HA255);
              
                uint16_t rpm = _fanMonitor.getSpeed();
                rpm = rpm *2;
                Serial.println(rpm);
                rpm =0;
                counter =0;

                tft.fillRect(25,61,78,40,TFT_BLACK);
                tft.fillRect(135,61,78,40,TFT_BLACK);
                tft.fillRect(135,171,78,40,TFT_BLACK);
                tft.fillRect(25,171,78,40,TFT_BLACK);
                tft.fillRect(105,61,28,40,TFT_BLACK);
                tft.fillRect(215,61,40,40,TFT_BLACK);
                tft.fillRect(0,91,23,40,TFT_BLACK);

                tft.setTextColor(TFT_BLUE);
                if (PM25 >= 600 || PM25 == 0){
                  tft.setTextSize(3);   
                  tft.setCursor(42,75);
                  tft.print(PM252); 
                }
                else {
                  tft.setTextSize(3);   
                  tft.setCursor(42,75);
                  tft.print(PM25);
                }

                if (isnan(ppm)){
                  tft.setTextSize(3);   
                  tft.setCursor(142,75);
                  tft.print(ppm);
                }
                else {
                  tft.setTextSize(3);   
                  tft.setCursor(142,75);
                  tft.print(ppm2);
                }
              
                tft.setTextSize(3);   
                tft.setCursor(29,185);
                tft.print(humidity,1);
                
                tft.setTextSize(3);   
                tft.setCursor(139,185);
                tft.print(temperature,1);

              }
              tft.setTextColor(TFT_WHITE);
              tft.setTextSize(1);
              tft.drawCentreString("PM2.5", 64, 20, 4); // Comment out to avoid font 4
              tft.drawRect(24, 15, 80, 90, TFT_YELLOW);
              tft.drawLine(24, 55, 103, 55, TFT_YELLOW);

              tft.setTextSize(1);
              tft.drawCentreString("CO", 174, 25, 4); // Comment out to avoid font 4
              tft.drawRect(134, 15, 80, 90, TFT_YELLOW);
              tft.drawLine(134, 55, 213, 55, TFT_YELLOW);

              tft.setTextSize(1);
              tft.drawCentreString("Suhu", 174, 135, 4); // Comment out to avoid font 4
              tft.drawRect(134, 125, 80, 90, TFT_YELLOW);
              tft.drawLine(134, 165, 213, 165, TFT_YELLOW);

              tft.setTextSize(2);
              tft.drawCentreString("Lembab", 64, 135, 1); // Comment out to avoid font 4
              tft.drawRect(24, 125, 80, 90, TFT_YELLOW);
              tft.drawLine(24, 165, 103, 165, TFT_YELLOW);

              drawButtons();
              
            uint16_t raw_x, raw_y;
            if (tft.getTouch(&raw_y, &raw_x)) {
              Serial.print("Raw touch coordinates: (");
              Serial.print(raw_x);
              Serial.print(", ");
              Serial.print(raw_y);
              Serial.println(")");
            }

            uint16_t touch_x = map(raw_x, 0, 320, 0, 240);  // Adjust these values as needed
            uint16_t touch_y = map(raw_y, 0, 240, 0, 320);  // Adjust these values as needed

            Serial.print("Mapped touch coordinates: (");
            Serial.print(touch_x);
            Serial.print(", ");
            Serial.print(touch_y);
            Serial.println(")");

            tft.fillRoundRect(BUTTON1_X, BUTTON1_Y, BUTTON1_W, BUTTON1_H, 10, TFT_GREEN); // On state
            tft.setTextColor(TFT_BLACK);
            tft.setTextSize(2);
            tft.drawCentreString("ON", BUTTON1_X + BUTTON1_W / 2, BUTTON1_Y + BUTTON1_H / 2 - 15, 2);
            if (touch_x > BUTTON1_X && touch_x < BUTTON1_X + BUTTON1_W && touch_y > BUTTON1_Y && touch_y < BUTTON1_Y + BUTTON1_H) {
              Serial.println("Button 1 (ON/OFF) pressed");
              content.set("fields/powerState/booleanValue", false);
              if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "powerState")){
                Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
              } 
              delay(500);
              tft.fillRoundRect(BUTTON1_X, BUTTON1_Y, BUTTON1_W, BUTTON1_H, 10, TFT_RED); // Off state
              tft.setTextColor(TFT_BLACK);
              tft.setTextSize(2);
              tft.drawCentreString("OFF", BUTTON1_X + BUTTON1_W / 2, BUTTON1_Y + BUTTON1_H / 2 - 15, 2);
            } 

            tft.fillRoundRect(BUTTON2_X, BUTTON2_Y, BUTTON2_W, BUTTON2_H, 10, TFT_BLUE); // Auto mode
            tft.setTextColor(TFT_BLACK);
            tft.setTextSize(2);
            tft.drawCentreString("Mode", BUTTON2_X + BUTTON2_W / 2, BUTTON2_Y + BUTTON2_H / 2 - 15, 2);
            if (touch_x > BUTTON2_X && touch_x < BUTTON2_X + BUTTON2_W && touch_y > BUTTON2_Y && touch_y < BUTTON2_Y + BUTTON2_H) {
              Serial.println("Button 2 (Mode) pressed");
              content.set("fields/autoMode/booleanValue", false);
              if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "autoMode")){
                Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
              }
              delay(500);
              tft.fillRoundRect(BUTTON2_X, BUTTON2_Y, BUTTON2_W, BUTTON2_H, 10, TFT_ORANGE);
              tft.setTextColor(TFT_BLACK);
              tft.setTextSize(2);
              tft.drawCentreString("Mode", BUTTON2_X + BUTTON2_W / 2, BUTTON2_Y + BUTTON2_H / 2 - 15, 2);
            }

            tft.fillRoundRect(BUTTON3_X, BUTTON3_Y, BUTTON3_W, BUTTON3_H, 10, TFT_DARKGREY); // Disabled state
            tft.setTextColor(TFT_LIGHTGREY);
            tft.setTextSize(2);
            tft.drawCentreString("Fan: " + String(documents_0_fields_sliderValue_integerValue), BUTTON3_X + BUTTON3_W / 2, BUTTON3_Y + BUTTON3_H / 2 - 9, 1); // Display fan speed

            delay(10);
            //Delay before the next reading

            Serial.println("MODE : Auto");
            SSCO = 0, SCO = 0, MCO = 0, BCO = 0, SBCO = 0;
            SSPM = 0, SPM = 0, MPM = 0, BPM = 0, SBPM = 0;
            SPKK = 0,PKK = 0,PKK1 = 0,PKK2 = 0,PKK3 = 0,MKK = 0,MKK1 = 0,MKK2 = 0,MKK3 = 0,MKK4 = 0,MKK5 = 0,CKK = 0,CKK1 = 0,CKK2 = 0,CKK3 = 0,CKK4 = 0,CKK5 = 0,CKK6 = 0,CKK7 = 0;
            SCKK = 0,SCKK1 = 0,SCKK2 = 0,SCKK3 = 0,SCKK4 = 0,SCKK5 = 0,SCKK6 = 0,SCKK7 = 0,SCKK8 = 0,SCKK9 = 0;
          }
          else if (documents_0_fields_autoMode_booleanValue == false){
            static unsigned long lasttime = 0;
            if (millis() - lasttime > 10000){
              lasttime = millis();
              
              HAauto = documents_0_fields_sliderValue_integerValue;
              HAauto2 = HAauto * 63.75;
              analogWrite(FAN_OUTPUT_PIN, HAauto2);
            
              uint16_t rpm = _fanMonitor.getSpeed();
              rpm = rpm *2;
              rpm =0;
              counter =0;
            
              if (touch_x > BUTTON1_X && touch_x < BUTTON1_X + BUTTON1_W && touch_y > BUTTON1_Y && touch_y < BUTTON1_Y + BUTTON1_H) {
                Serial.println("Button 1 (ON/OFF) pressed");
                tft.fillRoundRect(BUTTON1_X, BUTTON1_Y, BUTTON1_W, BUTTON1_H, 10, TFT_RED); // Off state
                tft.setTextColor(TFT_BLACK);
                tft.setTextSize(2);
                tft.drawCentreString("OFF", BUTTON1_X + BUTTON1_W / 2, BUTTON1_Y + BUTTON1_H / 2 - 15, 2);
                delay(500);
              }

              tft.setTextColor(TFT_WHITE);
              tft.setTextSize(1);
              tft.drawCentreString("PM2.5", 64, 20, 4); // Comment out to avoid font 4
              tft.drawRect(24, 15, 80, 90, TFT_YELLOW);
              tft.drawLine(24, 55, 103, 55, TFT_YELLOW);

              tft.setTextSize(1);
              tft.drawCentreString("CO", 174, 25, 4); // Comment out to avoid font 4
              tft.drawRect(134, 15, 80, 90, TFT_YELLOW);
              tft.drawLine(134, 55, 213, 55, TFT_YELLOW);

              tft.setTextSize(1);
              tft.drawCentreString("Suhu", 174, 135, 4); // Comment out to avoid font 4
              tft.drawRect(134, 125, 80, 90, TFT_YELLOW);
              tft.drawLine(134, 165, 213, 165, TFT_YELLOW);

              tft.setTextSize(2);
              tft.drawCentreString("Lembab", 64, 135, 1); // Comment out to avoid font 4
              tft.drawRect(24, 125, 80, 90, TFT_YELLOW);
              tft.drawLine(24, 165, 103, 165, TFT_YELLOW);

              tft.fillRect(25,61,78,40,TFT_BLACK);
              tft.fillRect(135,61,78,40,TFT_BLACK);
              tft.fillRect(135,171,78,40,TFT_BLACK);
              tft.fillRect(25,171,78,40,TFT_BLACK);
              tft.fillRect(105,61,28,40,TFT_BLACK);
              tft.fillRect(215,61,40,40,TFT_BLACK);
              tft.fillRect(0,91,23,40,TFT_BLACK);

              tft.setTextColor(TFT_BLUE);
              if (PM25 >= 600 || PM25 == 0){
                tft.setTextSize(3);   
                tft.setCursor(42,75);
                tft.print(PM252); 
              }
              else {
                tft.setTextSize(3);   
                tft.setCursor(42,75);
                tft.print(PM25);
              }
              if (isnan(ppm)){
                tft.setTextSize(3);   
                tft.setCursor(142,75);
                tft.print(ppm);
              }
              else {
                tft.setTextSize(3);   
                tft.setCursor(142,75);
                tft.print(ppm2);
              }
            
              tft.setTextSize(3);   
              tft.setCursor(29,185);
              tft.print(humidity,1);
              
              tft.setTextSize(3);   
              tft.setCursor(139,185);
              tft.print(temperature,1);
            }

            uint16_t raw_x, raw_y;
            if (tft.getTouch(&raw_y, &raw_x)) {
              Serial.print("Raw touch coordinates: (");
              Serial.print(raw_x);
              Serial.print(", ");
              Serial.print(raw_y);
              Serial.println(")");
            }

            uint16_t touch_x = map(raw_x, 0, 320, 0, 240);  // Adjust these values as needed
            uint16_t touch_y = map(raw_y, 0, 240, 0, 320);  // Adjust these values as needed

            Serial.print("Mapped touch coordinates: (");
            Serial.print(touch_x);
            Serial.print(", ");
            Serial.print(touch_y);
            Serial.println(")");
              
            tft.fillRoundRect(BUTTON1_X, BUTTON1_Y, BUTTON1_W, BUTTON1_H, 10, TFT_GREEN); // On state
            tft.setTextColor(TFT_BLACK);
            tft.setTextSize(2);
            tft.drawCentreString("ON", BUTTON1_X + BUTTON1_W / 2, BUTTON1_Y + BUTTON1_H / 2 - 15, 2);
            if (touch_x > BUTTON1_X && touch_x < BUTTON1_X + BUTTON1_W && touch_y > BUTTON1_Y && touch_y < BUTTON1_Y + BUTTON1_H) {
              Serial.println("Button 1 (ON/OFF) pressed");
              content.set("fields/powerState/booleanValue", false);
              if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "powerState")){
                Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
              } 
              delay(500);
              tft.fillRoundRect(BUTTON1_X, BUTTON1_Y, BUTTON1_W, BUTTON1_H, 10, TFT_RED); // Off state
              tft.setTextColor(TFT_BLACK);
              tft.setTextSize(2);
              tft.drawCentreString("OFF", BUTTON1_X + BUTTON1_W / 2, BUTTON1_Y + BUTTON1_H / 2 - 15, 2);
            } 

            tft.fillRoundRect(BUTTON2_X, BUTTON2_Y, BUTTON2_W, BUTTON2_H, 10, TFT_ORANGE);
            tft.setTextColor(TFT_BLACK);
            tft.setTextSize(2);
            tft.drawCentreString("Mode", BUTTON2_X + BUTTON2_W / 2, BUTTON2_Y + BUTTON2_H / 2 - 15, 2);
            if (touch_x > BUTTON2_X && touch_x < BUTTON2_X + BUTTON2_W && touch_y > BUTTON2_Y && touch_y < BUTTON2_Y + BUTTON2_H) {
              Serial.println("Button 2 (Mode) pressed");
              content.set("fields/autoMode/booleanValue", true);
              if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "autoMode")){
                Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
              }
              delay(500);
              tft.fillRoundRect(BUTTON2_X, BUTTON2_Y, BUTTON2_W, BUTTON2_H, 10, TFT_BLUE); // Auto mode
              tft.setTextColor(TFT_BLACK);
              tft.setTextSize(2);
              tft.drawCentreString("Mode", BUTTON2_X + BUTTON2_W / 2, BUTTON2_Y + BUTTON2_H / 2 - 15, 2);
            }
            tft.fillRoundRect(BUTTON3_X, BUTTON3_Y, BUTTON3_W, BUTTON3_H, 10, TFT_GREEN); // Default state
            tft.setTextColor(TFT_BLACK);
            tft.setTextSize(2);
            tft.drawCentreString("Fan: " + String(documents_0_fields_sliderValue_integerValue), BUTTON3_X + BUTTON3_W / 2, BUTTON3_Y + BUTTON3_H / 2 - 9, 1); // Display fan speed  

            // Check if touch is within button3 boundaries (Fan Button)
            if (touch_x > BUTTON3_X && touch_x < BUTTON3_X + BUTTON3_W && touch_y > BUTTON3_Y && touch_y < BUTTON3_Y + BUTTON3_H) {
              Serial.println("Button 3 (Fan) pressed");
              if(documents_0_fields_sliderValue_integerValue == 1){
                content.set("fields/sliderValue/integerValue", 2);
                if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "sliderValue")){
                  Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
                }
                delay(500);
              }
              else if(documents_0_fields_sliderValue_integerValue == 2){
                content.set("fields/sliderValue/integerValue", 3);
                if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "sliderValue")){
                  Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
                }
                delay(500);
              }
              else if(documents_0_fields_sliderValue_integerValue == 3){
                content.set("fields/sliderValue/integerValue", 4);
                if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "sliderValue")){
                  Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
                }
                delay(500);
              }
              else if(documents_0_fields_sliderValue_integerValue == 4){
                content.set("fields/sliderValue/integerValue", 1);
                if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "sliderValue")){
                  Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
                }
                delay(500);
              }
              delay(500);
            }
    
            delay(10);

            Serial.println(HAauto);          
            Serial.println(HAauto2);
            Serial.println("MODE : Manual");
            Serial.println(rpm);
          }
        }
        else{
          Serial.print("alat mati");
          analogWrite(FAN_OUTPUT_PIN, 0);

          uint16_t raw_x, raw_y;
          if (tft.getTouch(&raw_y, &raw_x)) {
            Serial.print("Raw touch coordinates: (");
            Serial.print(raw_x);
            Serial.print(", ");
            Serial.print(raw_y);
            Serial.println(")");
          }

          uint16_t touch_x = map(raw_x, 0, 320, 0, 240);  // Adjust these values as needed
          uint16_t touch_y = map(raw_y, 0, 240, 0, 320);  // Adjust these values as needed

          Serial.print("Mapped touch coordinates: (");
          Serial.print(touch_x);
          Serial.print(", ");
          Serial.print(touch_y);
          Serial.println(")");
          tft.fillRoundRect(BUTTON1_X, BUTTON1_Y, BUTTON1_W, BUTTON1_H, 10, TFT_RED); // Off state
          tft.setTextColor(TFT_BLACK);
          tft.setTextSize(2);
          tft.drawCentreString("OFF", BUTTON1_X + BUTTON1_W / 2, BUTTON1_Y + BUTTON1_H / 2 - 15, 2);
          if (touch_x > BUTTON1_X && touch_x < BUTTON1_X + BUTTON1_W && touch_y > BUTTON1_Y && touch_y < BUTTON1_Y + BUTTON1_H) {
            Serial.println("Button 1 (ON/OFF) pressed");
            delay(500);
            content.set("fields/powerState/booleanValue", true);
            if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "powerState")){
              Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
            } 
            tft.fillRoundRect(BUTTON1_X, BUTTON1_Y, BUTTON1_W, BUTTON1_H, 10, TFT_GREEN); // On state
            tft.setTextColor(TFT_BLACK);
            tft.setTextSize(2);
            tft.drawCentreString("ON", BUTTON1_X + BUTTON1_W / 2, BUTTON1_Y + BUTTON1_H / 2 - 15, 2);
          } 

          tft.fillRoundRect(BUTTON2_X, BUTTON2_Y, BUTTON2_W, BUTTON2_H, 10, TFT_ORANGE);
          tft.setTextColor(TFT_BLACK);
          tft.setTextSize(2);
          tft.drawCentreString("Mode", BUTTON2_X + BUTTON2_W / 2, BUTTON2_Y + BUTTON2_H / 2 - 15, 2);
        }
      }
    }
  }
}

boolean readPMSdata(Stream *s) {
  if (! s->available()) {
    return false;
  }
  
  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }
 
  // Now read all 32 bytes
  if (s->available() < 32) {
    return false;
  }
    
  uint8_t buffer[32];    
  uint16_t sum = 0;
  s->readBytes(buffer, 32);
 
  // get checksum ready
  for (uint8_t i=0; i<30; i++) {
    sum += buffer[i];
  }
  
  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }
 
  // put it into a nice struct :)
  memcpy((void *)&data, (void *)buffer_u16, 30);
 
  if (sum != data.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  // success!
  return true;
}