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
