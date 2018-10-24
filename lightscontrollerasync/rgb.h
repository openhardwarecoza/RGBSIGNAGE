void setColourRgb(unsigned int red, unsigned int green, unsigned int blue) {
  analogWrite(12, red);
  analogWrite(13, green);
  analogWrite(14, blue);
}

void setupRGB() {
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  setColourRgb(0, 0, 0);
}

void runProgram(unsigned int id) {
  switch (id) {
    case 1:
      setColourRgb(1024, 0, 0);
      break;
    case 2:
      setColourRgb(1024, 1024, 0);
      break;
    case 3:
      setColourRgb(0, 1024, 0);
      break;
    case 4:
      setColourRgb(0, 1024, 1024);
      break;
    case 5:
      setColourRgb(0, 0, 1024);
      break;
    case 6: 
      setColourRgb(1024, 0, 1024);
      break;
    case 7:  // Pulsing Heartbeat
      setColourRgb(1024, 0, 0);
      if (digitalRead(pin_btn2) == 0 || digitalRead(pin_btn0) == 0) {
          break;
        }
      delay(200);
      setColourRgb(100, 0, 0);
      if (digitalRead(pin_btn2) == 0 || digitalRead(pin_btn0) == 0) {
          break;
        }
      delay(200);
      setColourRgb(1024, 0, 0);
      if (digitalRead(pin_btn2) == 0 || digitalRead(pin_btn0) == 0) {
          break;
        }
      delay(200);
      for(int fadeValue = 1024 ; fadeValue >= 100; fadeValue -=5){   ///fades the LED
        setColourRgb(fadeValue, 0, 0);
        if (digitalRead(pin_btn2) == 0 || digitalRead(pin_btn0) == 0) {
          break;
        }
        delay(3);
      }
      break;
    case 8: // Rainbow Cycle
      unsigned int rgbColour[3];
      rgbColour[0] = 255;
      rgbColour[1] = 0;
      rgbColour[2] = 0;  

      // Choose the colours to increment and decrement.
      for (int decColour = 0; decColour < 3; decColour += 1) {
        int incColour = decColour == 2 ? 0 : decColour + 1;

        // cross-fade the two colours.
        for(int i = 0; i < 255; i += 1) {
          rgbColour[decColour] -= 1;
          rgbColour[incColour] += 1;
          setColourRgb(rgbColour[0]*4, rgbColour[1]*4, rgbColour[2]*4);
          if (digitalRead(pin_btn2) == 0 || digitalRead(pin_btn0) == 0) {
            break;
          }
          delay(5);
        }
      }
      default: // Fallback to all-on
        // do nothing
        break;   
    }
}
