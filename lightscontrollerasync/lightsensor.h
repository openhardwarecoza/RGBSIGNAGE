// Analog Light sensor
int lightthreshold = 400;
static const int MaxCount = 100;
int currentCount = 0;
long sum = 0;
int avg = 0;
bool forceon = true;

void runLightSensor() {
   if(currentCount < MaxCount) {
    currentCount++;
    sum += analogRead(A0);
    delay(10);
  } else {
    avg = sum / MaxCount;
    currentCount = 0;
    sum = 0;   
  } 
}
