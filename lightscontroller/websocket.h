// Parsing the input String to get the X value and intent // to be replaced with an if then sort of UI
String inputString;
char inputCharString [100];
char inputChar;
int stringIndex = 0; // String stringIndexing int;
String stringX;
double x;



void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
                // send message to client
                webSocket.sendTXT(num, "Connected");
            }
            break;
        case WStype_TEXT:
            // USE_SERIAL.printf("[%u] we got Text: %s\n", num, payload);
            inputString = String((char*) payload);
            if (inputString.indexOf("s:") == 0) {
              USE_SERIAL.printf("[%u] we got Text: %s\n", num, payload) ;
              inputString = String((char*) payload);
              String newseq = inputString.substring(2);
              char newseqval[newseq.length() + 1]; //determine size of the array
              newseq.toCharArray(newseqval, sizeof(newseqval)); //put readStringinto an array
              sequence = atof(newseqval); //convert the array into an Integer
              saveConfig();
            }            
            break;
    }
}

