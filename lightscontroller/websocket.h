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
            if (inputString.indexOf("c:") == 0) {
              USE_SERIAL.printf("[%u] we got Text: %s\n", num, payload) ;
              inputString = String((char*) payload);
              int commaIndex = inputString.indexOf(',');
              //  Search for the next comma just after the first
              int secondCommaIndex = inputString.indexOf(',', commaIndex + 1);
              String red = inputString.substring(2, commaIndex);
              String green = inputString.substring(commaIndex + 1, secondCommaIndex);
              String blue = inputString.substring(secondCommaIndex + 1); // To the end of the string
              
              char chred[red.length() + 1]; //determine size of the array
              red.toCharArray(chred, sizeof(chred)); //put readStringinto an array
              redval = atof(chred); //convert the array into an Integer

              char chgreen[green.length() + 1]; //determine size of the array
              green.toCharArray(chgreen, sizeof(chgreen)); //put readStringinto an array
              greenval = atof(chgreen); //convert the array into an Integer

              char chblue[blue.length() + 1]; //determine size of the array
              blue.toCharArray(chblue, sizeof(chblue )); //put readStringinto an array
              blueval = atof(chblue); //convert the array into an Integer

              USE_SERIAL.println("");
              USE_SERIAL.print(redval);
              USE_SERIAL.print(','); 
              USE_SERIAL.print(greenval);
              USE_SERIAL.print(','); 
              USE_SERIAL.print(blueval);
              USE_SERIAL.println("");
              USE_SERIAL.println("");
              analogWrite(12, redval);
              analogWrite(13, greenval);
              analogWrite(14, blueval);
              sequence = 9;
              saveConfig();
            }
            if (inputString.indexOf("s:") == 0) {
              USE_SERIAL.printf("[%u] we got Text: %s\n", num, payload) ;
              inputString = String((char*) payload);
              String newseq = inputString.substring(2);
              char newseqval[newseq.length() + 1]; //determine size of the array
              newseq.toCharArray(newseqval, sizeof(newseqval)); //put readStringinto an array
              sequence = atof(newseqval); //convert the array into an Integer
            }            
            break;
    }
}

