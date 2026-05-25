
// NOTE(s):
// - Implementation of telnetServer make difficult to implement
//   telnetCommandHandlerCallback() function as own ".cpp" source code module


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
