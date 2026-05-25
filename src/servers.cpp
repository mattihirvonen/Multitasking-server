#include <WiFi.h>
#include <LittleFS.h>             // Or FFat.h or/and SD.h
#include <threadSafeFS.h>         // Include thread-safe wrapper since LittleFS, FFat and SD file systems are not thread safe
#include <ntpClient.h>            // Setting the time is not really necessary, only if you want to see the correct file creation times

#define HOSTNAME "Esp32Server"    // Choose your server's name - this is how FTP server would introduce itself to the clients

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

#define SWAP_DEL_AND_BACKSPACE 0    // seto to 1 to swap the meaning of these keys - this would be suitable for Putty and Linux Telnet clients

#include <ftpServer.h>
#include "telnetConfig.h"
#include <telnetServer.h>
#include "telnetCallback.h"

// 1️⃣ Crete thread-safe wrapper arround LittleFS (or FFat or SD)
threadSafeFS::FS TSFS (LittleFS);

// 2️⃣ Use thread-safe wrapper for all file operations form now on in your code
using File = threadSafeFS::File;

telnetServer_t *telnetServer = NULL;
ftpServer_t    *ftpServer    = NULL;

//--------------------------------------------------------------------------------------------

// Write your own "wifi_secrets.h" file to contain local secrets (or #undef WIFI_SECRETS) !!!
#define  WIFI_SECRETS


void setup ()
{
  Serial.begin (115200);

  // 3️⃣ Start LittleFS (or FFat or SD)
  LittleFS.begin (true);

  // 4️⃣ Start WiFi connection to WiFi router
  #ifdef    WIFI_SECRETS
  #include "wifiSecrets.h"
  WiFi.begin ( ROUTER_SSID,   ROUTER_PASSWORD );
  #else
  WiFi.begin ("ROUTER_SSID", "ROUTER_PASSWORD");
  #endif


  // 5️⃣ Create Telnet server instance that would use thread-safe wrapper arround LittleFS (or FFat or SD)
  telnetServer = new (std::nothrow) telnetServer_t (TSFS, NULL, telnetCommandHandlerCallback, 23);
                                                            // optional arguments:
                                                            // Cstring<255> (*getUserHomeDirectory) (const Cstring<64>& userName, const Cstring<64>& password) = NULL
                                                            // String (*telnetCommandHandlerCallback) (int argc, char *argv [], telnetConnection_t *tcn) = NULL
                                                            // int serverPort = 23
                                                            // bool (*firewallCallback) (char *clientIP, char *serverIP) = NULL
                                                            // bool runListenerInItsOwnTask = true


  // 6️⃣ Check if Telnet server instance is created && Telnet server is running
  if (telnetServer && *telnetServer)
    Serial.println ("Telnet server started");
  else
    Serial.println ("Telnet server did not start");


  // 5️⃣ Create FTP server instance that would use thread-safe wrapper arround LittleFS (or FFat or SD)
  ftpServer = new (std::nothrow) ftpServer_t (TSFS);  // optional arguments:
                                                      //    Cstring<255> (*getUserHomeDirectory) (const Cstring<64>& userName, const Cstring<64>& password) = NULL
                                                      //    int serverPort = 21
                                                      //    bool (*firewallCallback) (char *clientIP, char *serverIP) = NULL
                                                      //    bool runListenerInItsOwnTask = true

  // 6️⃣ Check if FTP server instance is created && FTP server is running
  if (ftpServer && *ftpServer)
    Serial.println ("FTP server started");
  else
    Serial.println ("FTP server did not start");


  // 7️⃣ Use Telnet and FTP clients to connect to ESP32's IP address
  while (WiFi.localIP () == IPAddress (0, 0, 0, 0)) { // wait until we get IP from router's DHCP
      delay (1000); 
      Serial.println ("   ."); 
  } 
  Serial.print ("Got IP addess: "); Serial.println (WiFi.localIP ());


  // 8️⃣ Setting the time is not really necessary, only if you want to see the correct file creation times
  setenv ("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1); // or select another (POSIX) time zones: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
  ntpClient_t ntpClient ("1.si.pool.ntp.org", "2.si.pool.ntp.org", "3.si.pool.ntp.org");
  ntpClient.syncTime ();

  // 9️⃣ Use thread-safe wrapper as you would use LittleFS in your code, for example:
  File f = TSFS.open ("/test.txt", "w");
  if (f) {
    f.print ("This is a test file.");
    f.close ();
  }

  pinMode (LED_BUILTIN, OUTPUT);
}


void loop ()
{
  // Formal dummy loop
  static int initialized = 0;

  if ( !initialized ) {
    Serial.printf ("Loop started on core %d\r\n", xPortGetCoreID() );
    initialized = 1;
  }
}
