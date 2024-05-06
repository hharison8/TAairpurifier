#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <DHT.h>  // Include the DHT sensor library
#include <Arduino.h>
#include <FanMonitor.h>
#include "PMS.h"
#include <HardwareSerial.h>
//SoftwareSerial pmsSerial(2, 3);

HardwareSerial pmsSerial(2); // Define Serial2 on UART 2
#define RX_PIN 16 // Define the pin connected to RX on ESP32
#define TX_PIN 17 // Define the pin connected to TX on ESP32

// Define DHT sensor parameters
#define DHTPIN 14
#define DHTTYPE DHT22
int ppm;
int PM25=0;

//fuzzy
float SSCO, SCO, MCO, BCO, SBCO;
float SSPM, SPM, MPM, BPM, SBPM;
float SPKK,PKK,PKK1,PKK2,PKK3,MKK,MKK1,MKK2,MKK3,MKK4,MKK5,CKK,CKK1,CKK2,CKK3,CKK4,CKK5,CKK6,CKK7;
float SCKK,SCKK1,SCKK2,SCKK3,SCKK4,SCKK5,SCKK6,SCKK7,SCKK8,SCKK9,HA;

//Fussyfikasi CO
unsigned char sangatsedikitCO(){
  if (ppm <= 5){SSCO = 1;}
  else if (ppm > 5 && ppm < 10){SSCO = (10-ppm)/5.00;}
  else if (ppm > 10){SSCO = 0;}
  return SSCO;
}
unsigned char sedikitCO(){
  if (ppm == 10){SCO = 1;}
  else if (ppm > 5 && ppm < 10){SCO = (ppm-5)/5.00;}
  else if (ppm > 10 && ppm < 15){SCO = (15-ppm)/5.00;}
  else if (ppm > 15){SCO = 0;}
  return SCO;
  
}
unsigned char mediumCO(){
  if (ppm == 15){MCO = 1;}
  else if (ppm > 10 && ppm < 15){MCO = (ppm-10)/5.00;}
  else if (ppm > 15 && ppm < 20){MCO = (20-ppm)/5.00;}
  else if (ppm > 20){MCO = 0;}
  return MCO;
}
unsigned char banyakCO(){
  if (ppm == 20){BCO = 1;}
  else if (ppm > 15 && ppm < 20){BCO = (ppm-15)/5.00;}
  else if (ppm > 20 && ppm < 25){BCO = (25-ppm)/5.00;}
  else if (ppm > 25){BCO = 0;}
  return BCO;
}
unsigned char sangatbanyakCO(){
  if (ppm >= 25){SBCO = 1;}
  else if (ppm > 20 && ppm < 25){SBCO = (ppm-20)/5.00;}
  else if (ppm < 20){SBCO = 0;}
  return SBCO;
}

//Fussyfikasi PM2.5
unsigned char sangatsedikitPM(){
  if (PM25 <= 25){SSPM = 1;}
  else if (PM25 > 25 && PM25 < 50){SSPM = (50-PM25)/25.00;}
  else if (PM25 > 50){SSPM = 0;}
  return SSPM;
}
unsigned char sedikitPM(){
  if (PM25 == 50){SPM = 1;}
  else if (PM25 > 25 && PM25 < 50){SPM = (PM25-25)/25.00;}
  else if (PM25 > 50 && PM25 < 75){SPM = (75-PM25)/25.00;}
  else if (PM25 > 75){SPM = 0;}
  return SPM;
}
unsigned char mediumPM(){
  if (PM25 == 75){MPM = 1;}
  else if (PM25 > 50 && PM25 < 75){MPM = (PM25-50)/25.00;}
  else if (PM25 > 75 && PM25 < 125){MPM = (125-PM25)/50.00;}
  else if (PM25 > 125){MPM = 0;}
  return MPM;
}
unsigned char banyakPM(){
  if (PM25 == 125){BPM = 1;}
  else if (PM25 > 75 && PM25 < 125){BPM = (PM25-75)/50.00;}
  else if (PM25 > 125 && PM25 < 175){BPM = (175-PM25)/50.00;}
  else if (PM25 > 175){BPM = 0;}
  return BPM;
}
unsigned char sangatbanyakPM(){
  if (PM25 >= 175){SBPM = 1;}
  else if (PM25 > 125 && PM25 < 175){SBPM = (PM25-125)/50.00;}
  else if (PM25 < 125){SBPM = 0;}
  return SBPM;
}

void fuzzifikasi(){
  sangatsedikitCO();
  sedikitCO();
  mediumCO();
  banyakCO();
  sangatbanyakCO();
  sangatsedikitPM();
  sedikitPM();
  mediumPM();
  banyakPM();
  sangatbanyakPM();
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
}

//PWM
const int PWM_CHANNEL = 0;    
const int PWM_FREQ = 500;     
const int PWM_RESOLUTION = 8; 

const int MAX_DUTY_CYCLE = (int)(pow(2, PWM_RESOLUTION) - 1); 

const int FAN_OUTPUT_PIN = 18;

const int DELAY_MS = 100;  // delay between fade increments
const int tachoPin = 19;
volatile unsigned long counter = 0;
int rpm =0;
FanMonitor _fanMonitor = FanMonitor(tachoPin, FAN_TYPE_BIPOLE);

//Sensor CO
#define AO_PIN 33
float RS_gas = 0;
float ratio = 0;
float sensorValue = 0;
float sensor_volt = 0;
float R0 = 7200.0;

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

  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);

  ledcAttachPin(FAN_OUTPUT_PIN, PWM_CHANNEL);

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

void loop() {
  SSCO = 0, SCO = 0, MCO = 0, BCO = 0, SBCO = 0;
  SSPM = 0, SPM = 0, MPM = 0, BPM = 0, SBPM = 0;
  SPKK = 0,PKK = 0,PKK1 = 0,PKK2 = 0,PKK3 = 0,MKK = 0,MKK1 = 0,MKK2 = 0,MKK3 = 0,MKK4 = 0,MKK5 = 0,CKK = 0,CKK1 = 0,CKK2 = 0,CKK3 = 0,CKK4 = 0,CKK5 = 0,CKK6 = 0,CKK7 = 0;
  SCKK = 0,SCKK1 = 0,SCKK2 = 0,SCKK3 = 0,SCKK4 = 0,SCKK5 = 0,SCKK6 = 0,SCKK7 = 0,SCKK8 = 0,SCKK9 = 0,HA = 0;
  int HA100=0,HA255=0;
  
  // Define the path to the Firestore document
  String documentPath = "EspData/DHT11";

  // Create a FirebaseJson object for storing data
  FirebaseJson content;

  // Read temperature and humidity from the DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  sensorValue = analogRead(AO_PIN);
  sensor_volt = sensorValue/1024*5.0;
  RS_gas = (5.0-sensor_volt)/sensor_volt;
  ratio = RS_gas/R0;
  float x = 1538.46 * ratio;
  ppm = pow(x,-1);

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
  PM25 = data.pm25_env;

  // Print temperature and humidity values
  Serial.println(temperature);
  Serial.println(humidity);
  Serial.println(ppm); 
  Serial.println(PM25); 

  fuzzifikasi();
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
    HA =  ((15400.00*SCKK)/(SCKK*7.00));
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
  
  //int dutyCycle = 100; //ganti value dari slider mobile
  //dutyCycle = map(HA1, 0, 100, 0, MAX_DUTY_CYCLE);
  //ledcWrite(PWM_CHANNEL, HA1);
  analogWrite(FAN_OUTPUT_PIN, HA255);

  //delay(DELAY_MS);

  uint16_t rpm = _fanMonitor.getSpeed();
  rpm = rpm *2;
  Serial.println(rpm);
  rpm =0;
  counter =0;

  // Check if the values are valid (not NaN)
  if (!isnan(temperature) && !isnan(humidity) && !isnan(ppm) && !isnan(PM25)) {
    // Set the 'Temperature' and 'Humidity' fields in the FirebaseJson object
    content.set("fields/Temperature/stringValue", String(temperature, 2));
    content.set("fields/Humidity/stringValue", String(humidity, 2));
    content.set("fields/CO/stringValue", String(ppm));
    content.set("fields/PM25/stringValue", String(PM25));

    Serial.print("Update/Add DHT Data... ");

    // Use the patchDocument method to update the Temperature and Humidity Firestore document
    if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "Temperature") && Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "Humidity") && Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "CO") && Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "PM25")) {
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      } else {
            Serial.println(fbdo.errorReason());
          }
        } else {
          Serial.println("Failed to read DHT data.");
        } 

  // Delay before the next reading
  delay(10000);
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
 
  /* debugging
  for (uint8_t i=2; i<32; i++) {
    Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
  }
  Serial.println();
  */
  
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
