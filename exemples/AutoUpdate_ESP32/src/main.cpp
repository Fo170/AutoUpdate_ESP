#include <Arduino.h>
/*
AutoUpdate_ESP by Olivier FOURNET : https://github.com/Fo170/AutoUpdate_ESP

Dependency Graph
|-- HTTPClient @ 2.0.0
|-- Update @ 2.0.0
|-- AutoUpdate_ESP
|-- HTTPUpdate @ 2.0.0
|-- WiFi @ 2.0.0
*/
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h> // Ensure the HTTPClient library is installed and included
#include <Update.h>
#include <HTTPUpdate.h>

const char* ssid = "ssid";
const char* password = "pass";

#define _STATUS_LED_CONNEXION_ON_
#define _HOSTNAME_ "TEST_Auto_Update"
//---------------------------------------------------------------
// Exemple avec des fichiers héberger sur le site de free.fr
#define FIRMWARE_URL_BIN   "http://site.free.fr/path_firmware/firmware.bin"
#define FIRMWARE_URL_TXT   "http://site.free.fr/path_firmware/firmware.txt"

#define FIRMWARE_VERSION "0.9"

#define LED_BUILTIN 2 // GPIO2 (D4) on ESP8266, GPIO2 (D2) on ESP32

#include <AutoUpdate_ESP.h>

//---------------------------------------------------------------
byte mac[6];
char MAC_str[20];
String s_ChipId;
String s_version;
String s_hostname;
String ipStr, gatewayIP_Str, dnsStr;
IPAddress ip;
int status_connexion;
float rssi_moy;
int connexion_error = 0, nb_wait = 0;

//------------------------------------------
// t_fct : Temp de fonctionnement en secondes
unsigned long t0_fct;
float t_fct;

void INIT_Temp_fct()
{
 t0_fct = millis();	
}

void Calcule_Temp_fct() // Temp de fonctionnement en secondes
{
 t_fct = 0.001 * (float)(millis() - t0_fct); 
}
//------------------------------------------
float Moyenne_10(float Moy, float val);

// Gestion des évènements du Wifi
void OnConnected(WiFiEvent_t event, WiFiEventInfo_t info);
void OnGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
void OnDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);

// RSSI
void INIT_RSSI(void);
void Calcule_RSSI(void);

// Initialisation connexion WIFI
void Init_WIFI(void);
void status_connexion_WIFI(void);

// Force signal
String Acceptable_Signal(float rssi_val);

//WiFiClient  client;

int etat_update = 0;

void Separateur_Simple_Serial(void)
{
  Serial.println("--------------");
}

float Moyenne_10(float Moy, float val)
{
 Moy = 0.1 * (9.0 * Moy + val);  

 return Moy;
}

// Gestion des évènements du Wifi
void OnConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
 status_connexion = 1;
 Serial.println("\nWifi Connected"); 
}

void OnGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
 ip = WiFi.localIP();
 ipStr = WiFi.localIP().toString(); 
 gatewayIP_Str = WiFi.gatewayIP().toString();
 dnsStr = WiFi.dnsIP().toString();
 WiFi.macAddress(mac);
 
 snprintf(MAC_str, sizeof MAC_str, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

 Serial.print("\n\rMAC : "); 
 Serial.println( MAC_str ); 
 Serial.println("Adresse IP : " + ipStr ); 
 Serial.println("Passerelle IP : " + gatewayIP_Str ); 
 Serial.println("DNS IP : " + dnsStr ); 
 Serial.print("Puissance de réception : ");
 Calcule_RSSI();
 Serial.print( rssi_moy );
 Serial.println("dBm");
}

void OnDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
 Serial.println("\nWifi Disconnected");
 status_connexion = 0;
}

void status_connexion_WIFI(void)
{
  // la LED_BUILTIN correspond au D4 du GPIO2
  switch (WiFi.status())
  {
    case WL_NO_SSID_AVAIL:
      // Configured SSID cannot be reached
      status_connexion = 0;
      Serial.println("No SSID available");
      break;

    case WL_CONNECTED:
      // Connection successfully established
      status_connexion = 1;
      #ifdef _STATUS_LED_CONNEXION_ON_
      digitalWrite(LED_BUILTIN, LOW); // Connected, LED on
      #endif
      Serial.println("WiFi connected");
      break;

    case WL_CONNECT_FAILED:
      // Connection failed
      status_connexion = 0;
      #ifdef _STATUS_LED_CONNEXION_ON_
      digitalWrite(LED_BUILTIN, HIGH); // Not connected, LED off
      #endif
      Serial.println("WiFi connection failed");
      break;

    case WL_NO_SHIELD:
      // No WiFi shield is present
      status_connexion = 0;
      Serial.println("No WiFi shield present");
      break;

    case WL_IDLE_STATUS:
      // WiFi is idle
      status_connexion = 0;
      Serial.println("WiFi idle");
      break;

    case WL_SCAN_COMPLETED:
      // WiFi scan completed
      status_connexion = 0;
      Serial.println("WiFi scan completed");
      break;

    case WL_CONNECTION_LOST:
      // Connection lost
      status_connexion = 0;
      Serial.println("WiFi connection lost");
      break;

    // Removed WL_WRONG_PASSWORD case as it is not defined for ESP32

    case WL_DISCONNECTED:
      // Disconnected
      status_connexion = 0;
      Serial.println("WiFi disconnected");
      break;

    default:
      // Handle any other unexpected status
      status_connexion = 0;
      Serial.println("Unknown WiFi status");
      break;
  }
}


void INIT_RSSI(void)
{
	float rssi = WiFi.RSSI();
    if(isnan(rssi))
    {
     Serial.println("Failed to read from rssi!");
	   rssi = -120.0;
     return;
    }
    else  rssi_moy = rssi; // Moyenne sur 10 valeurs
}

void Calcule_RSSI(void)
{
	float rssi = WiFi.RSSI();
    if(isnan(rssi))
    {
     Serial.println("Failed to read from rssi!");
     return;
    }
    else  rssi_moy = Moyenne_10(rssi_moy, rssi); // Moyenne sur 10 valeurs
}

// Initialisation connexion WIFI
void Init_WIFI(void)
{
 #ifdef _STATUS_LED_CONNEXION_ON_
 pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
 #endif
 //wdt_reset(); 	
Serial.print("Chip ID: 0x");
uint64_t chipId = ESP.getEfuseMac();
s_ChipId = String((uint16_t)(chipId >> 32), HEX) + String((uint32_t)chipId, HEX);

 Serial.println(s_ChipId);

 s_version = String(__DATE__)+ "-" + String(__TIME__);
  
 // Set Hostname.
 s_hostname = String(_HOSTNAME_);
 
 //s_hostname += "-" + s_ChipId;// + "-" + s_version;
 WiFi.setHostname(s_hostname.c_str());

 //wdt_reset();
 
 // Print hostname.
 Serial.println("Hostname: " + s_hostname);
 Serial.println(WiFi.getHostname());
  
 WiFi.mode(WIFI_STA);
 WiFi.begin(ssid, password);

 #ifdef _IP_FIXE_
 WiFi.config(ip, gateway, subnet);
 #endif	
 
 WiFi.macAddress(mac);
 
 snprintf(MAC_str, sizeof MAC_str, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

 Serial.print("\n\rMAC : "); 
 Serial.println( MAC_str ); 

 WiFi.onEvent(OnConnected, ARDUINO_EVENT_WIFI_STA_CONNECTED);
 WiFi.onEvent(OnGotIP, ARDUINO_EVENT_WIFI_STA_GOT_IP);
 WiFi.onEvent(OnDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

 Serial.println("");
 // Wait for connection ----------------------------
 
 while( (WiFi.status() != WL_CONNECTED) && (nb_wait <= 100) ) // sort de la boucle si ça dure plus que 100x 
 {
  #ifdef _STATUS_LED_CONNEXION_ON_
  digitalWrite(LED_BUILTIN, LOW);   // Connecter, LED allumé / Turn the LED on (Note that LOW is the voltage level
  #endif

  delay(50);
  #ifdef _STATUS_LED_CONNEXION_ON_
  digitalWrite(LED_BUILTIN, HIGH);  // Non Connecter, LED éteinte / Turn the LED off by making the voltage HIGH
  #endif
 
  delay(50);
  nb_wait++;

  Serial.print("nb_wait : ");
  Serial.print(nb_wait);
  Serial.print(" / MAC : "); 
  Serial.println( MAC_str ); 
 }
 
 //The ESP8266 tries to reconnect automatically when the connection is lost
 WiFi.setAutoReconnect(true);
 WiFi.persistent(true);
  
 status_connexion_WIFI();

 INIT_RSSI();
}
//-------------------------------------------

void setup() 
{
  Serial.begin(115200);
  //   Serial.setDebugOutput(true);
 
  Serial.println();
  Serial.println();
  Serial.println();
 
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  Serial.print(F("Firmware Version actual : "));
  Serial.println(FirmwareVersionLocal);

  Separateur_Simple_Serial();

  Init_WIFI();
}
 
float t = 0.0;

void STATUS_IOT(void)
{
   Serial.print(t);
   Serial.print(" secondes");

   Serial.print(" / Puissance de réception : ");
   Calcule_RSSI();
   Serial.print( rssi_moy );
   Serial.print("dBm");

   if(status_connexion == 1)  Serial.println(" / CONNECTED");
   else Serial.println(" / NOT CONNECTED");
}

void loop() 
{
  STATUS_IOT();
   
  if(status_connexion == 1)
  {  
    AutoUpdate();
  }

  delay(10000); // Wait a second and restart
  t += 1.0;
  Serial.println("==============================================================");
    // Délai de 1 heure avant la prochaine mise à jour
  //delay(3600000);
}
