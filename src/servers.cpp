#include <WiFi.h>
#include <LittleFS.h>             // Or FFat.h or/and SD.h
#include <threadSafeFS.h>         // Include thread-safe wrapper since LittleFS, FFat and SD file systems are not thread safe
#include <ntpClient.h>            // Setting the time is not really necessary, only if you want to see the correct file creation times

#define HOSTNAME "Esp32Server"    // Choose your server's name - this is how FTP server would introduce itself to the clients

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

// 1️⃣ Choose which built-in Telnet commands will be included
#define TELNET_CLEAR_COMMAND 1      // 0=exclude, 1=include, clear is included by default
#define TELNET_UNAME_COMMAND 0      // 0=exclude, 1=include, uname is included by default
#define TELNET_FREE_COMMAND 0       // 0=exclude, 1=include, free is included by default
#define TELNET_NOHUP_COMMAND 0      // 0=exclude, 1=include, nohup is included by default
#define TELNET_REBOOT_COMMAND 0     // 0=exclude, 1=include, reboot is included by default
#define TELNET_DMESG_COMMAND 0      // 0=exclude, 1=include, dmesg is included by default
#define TELNET_QUIT_COMMAND 1       // 0=exclude, 1=include, quit is included by default
#define TELNET_UPTIME_COMMAND 1     // 0=exclude, 1=include, date is included by default
#define TELNET_DATE_COMMAND 1       // 0=exclude, 1=include, date is included by default
#define TELNET_NTPDATE_COMMAND 1    // 0=exclude, 1=include, ntpdate is included by default
#define TELNET_CRONTAB_COMMAND 0    // 0=exclude, 1=include, crontab is included by default
#define TELNET_PING_COMMAND 0       // 0=exclude, 1=include, ping is included by default
#define TELNET_IFCONFIG_COMMAND 0   // 0=exclude, 1=include, ifconfig is included by default
#define TELNET_IW_COMMAND 0         // 0=exclude, 1=include, iw is included by default
#define TELNET_NETSTAT_COMMAND 0    // 0=exclude, 1=include, netstat is included by default
#define TELNET_KILL_COMMAND 0       // 0=exclude, 1=include, kill is included by default
#define TELNET_CURL_COMMAND 0       // 0=exclude, 1=include, curl is included by default
#define TELNET_SENDMAIL_COMMAND 0   // 0=exclude, 1=include, sendmail is included by default
#define TELNET_LS_COMMAND 1         // 0=exclude, 1=include, ls is included by default
#define TELNET_TREE_COMMAND 1       // 0=exclude, 1=include, tree is included by default
#define TELNET_MKDIR_COMMAND 1      // 0=exclude, 1=include, mkdir is included by default
#define TELNET_RMDIR_COMMAND 1      // 0=exclude, 1=include, rmdir is included by default
#define TELNET_CD_COMMAND 1         // 0=exclude, 1=include, cd is included by default
#define TELNET_PWD_COMMAND 1        // 0=exclude, 1=include, pwd is included by default
#define TELNET_CAT_COMMAND 1        // 0=exclude, 1=include, cat is included by default
#define TELNET_VI_COMMAND 1         // 0=exclude, 1=include, vi is included by default
#define TELNET_CP_COMMAND 1         // 0=exclude, 1=include, cp is included by default
#define TELNET_RM_COMMAND 1         // 0=exclude, 1=include, rm is included by default
#define TELNET_LSOF_COMMAND 1       // 0=exclude, 1=include, lsof is included by default

#define SWAP_DEL_AND_BACKSPACE 0    // seto to 1 to swap the meaning of these keys - this would be suitable for Putty and Linux Telnet clients

// 1️⃣ Provide help text for user-defined commands
#define USER_DEFINED_TELNET_HELP_TEXT   "\r\n  LED commands:" \
                                        "\r\n      turn led on" \
                                        "\r\n      turn led off" \
                                        "\r\n      led state"

#include <telnetServer.h>
#include <ftpServer.h>


// 1️⃣ Crete thread-safe wrapper arround LittleFS (or FFat or SD)
threadSafeFS::FS TSFS (LittleFS);

// 2️⃣ Use thread-safe wrapper for all file operations form now on in your code
using File = threadSafeFS::File;

telnetServer_t *telnetServer = NULL;
ftpServer_t    *ftpServer    = NULL;

//--------------------------------------------------------------------------------------------
// 2️⃣ Provide callback function that would handle user-defined commands

String telnetCommandHandlerCallback (int argc, char *argv [], telnetServer_t::telnetConnection_t *tcn)
{
    // Must be reentrant !!!

    #define argv0is(X) (argc > 0 && !strcmp (argv[0], X))  
    #define argv1is(X) (argc > 1 && !strcmp (argv[1], X))
    #define argv2is(X) (argc > 2 && !strcmp (argv[2], X))   

    // 3️⃣ short-running functions should return the text the Telnet server will send to the client as a response to the command
    if (argv0is ("turn") && argv1is ("led") && argv2is ("on")) {
            digitalWrite (LED_BUILTIN, HIGH);
            return "Led is on";
    } else if (argv0is ("turn") && argv1is ("led") && argv2is ("off")) {
            digitalWrite (LED_BUILTIN, LOW);
            return "Led is off";
    }


    // 4️⃣ long-running functions should provide a mechanism to break the loop 
    else if (argv0is ("led") && argv1is ("state")) {
            for (int i = 0; i < 1000; i++) {
                char buf [6];
                sprintf (buf, "%s", digitalRead (LED_BUILTIN) ? "on\r\n" : "off\r\n");
                if (tcn->sendString (buf) <= 0)
                  return "\r";
                delay (250); 

                if (tcn->peekChar ()) {
                    tcn->recvChar ();
                    return "\r"; // break the loop and return something different than "" to let the telnet server function know that the command has been processed
                }
            }
            return "\r"; // return something different than "" to let the telnet server function know that the command has been processed
    }


    // 5️⃣ Unhandeled - let the Telnet server try to handle the command itself
    return "";
}

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
  #include "wifi_secrets.h"
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
