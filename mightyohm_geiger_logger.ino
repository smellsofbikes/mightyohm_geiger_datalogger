#include <SD.h>
#include <SPI.h>

// attach: gnd on arduino to gnd on mightyohm board
// attach: D2 on arduino to signal out (middle) pin on mightyohm board
const uint8_t SDpin=10, nInterrupt=0, interruptPin=2;
uint16_t count=0;
uint32_t millis_start,millis_total;
File logfile;
bool flag;

void setup() {
  Serial.begin(9600);
  Serial.println();
  if (!SD.begin(SDpin))
  {
    Serial.println("Card failed.");
    delay(1000);
    exit(0);
  }
  pinMode(interruptPin,INPUT_PULLUP);     // built-in resistor.
  attachInterrupt(nInterrupt,isr,RISING); // register interrupt service routine
  millis_start=millis();
  char namebuffer[] = "GMlog00.txt";      // buffer for sdcard name
  for (uint8_t i = 0; i < 100; i++)       // neat bit of code from adafruit to make unique names rather than overwriting
  {
    namebuffer[5] = i/10 + '0';
    namebuffer[6] = i%10 + '0';
    if (! SD.exists(namebuffer))
    {
      Serial.print("Datalog name: ");
      Serial.println(namebuffer);
      break;
    }
  }  
  logfile = SD.open(namebuffer, FILE_WRITE);
  if (!logfile) {
    Serial.println("Couldn't create file.");
    delay(1000);
    exit(0);
  }
  logfile.println("milliseconds, CPM");
}
void loop()
{
  if (flag != 0)
  {
    Serial.print("logging ");
    Serial.print(count);
    Serial.println(" CPM");
    logfile.print(millis());
    logfile.print(", ");
    logfile.println(count); // counts per minute
    logfile.flush();
    flag = 0;                               // clear 1 minute flag
    count = 0;                              // and clear counter
  }
}

void isr() { // interrupt service routine
  count++;
  millis_total=millis();
    if ((millis_total-millis_start)>60000L)     // kinda lousy way to check for 1 minute intervals
  {
    flag = 1;                                 // set 1 minute flag
    millis_start=millis_total;
  }
}
