///////////////////////////////////////////////////////
// Project based on https://github.com/jamescoxon/Eurus
///////////////////////////////////////////////////////

#include <Plan13.h>
#include <Time.h>
#include <EEPROM.h>

#define ONEPPM 1.0e-6

Plan13 p13;

char * elements[2][3] = {
  {
    "2015-049G",
    "1 40905U 15049G   16001.24636142  .00002241  00000-0  13262-3 0  9999",
    "2 40905  97.4495  11.8956 0014927 254.5248 166.2747 15.12369814 15617"
  }
};

long uxtm = 1451743687;
byte ELold = 0;
byte ELmin = 0;
byte ELmax = 0;
byte ALanz = 0;
byte maxPred = 2;

boolean AOS = false;
boolean LOS = false;
boolean predCalc = false;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup () {


  for (int i = 0; i < 255; i++) {
    EEPROM.write(i, i);
  };

  inputString.reserve(200);

  Serial.begin(115200);

  setTime((uxtm));

  p13.setFrequency(137100000, 137100000);
  p13.setLocation(8.70, 53.30, 8);
  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);

  Serial.println("Menü: ");
  Serial.println("p für Vorhersage ");
}

void loop() {
  //showPasses();

  if (stringComplete) {
    if (inputString == "p\n") {
      //      Serial.println("ljljl");
      showPasses ();
      // clear the string:
      inputString = "";

      stringComplete = false;
    } else
    {
      inputString = "";
      stringComplete = false;
    }
  }
}

void showPasses () {
  long uxtm = 1451743687;
  byte ELold = 0;
  byte ELmin = 0;
  byte ELmax = 0;
  byte ALanz = 0;
  byte maxPred = 2;

  setTime((uxtm));

  Serial.println("");
  Serial.print("Kalkuliere ");
  Serial.print(maxPred);
  Serial.println(" Überflüge des Satellits 2015-049G. Bitte warten...");
  Serial.println("");
  while (ALanz != maxPred) {
    time_t t = now();
    p13.setTime(year(t), month(t), day(t), hour(t), minute(t), second(t));

    readElements(0);
    p13.calculate(); //crunch the numbers

    if ((p13.EL >= 15) & (AOS == false) & (ALanz < maxPred)) {
      Serial.print("AOS: ");  if (day(t) < 10) {
        Serial.print("0");
      }; Serial.print(day(t)); Serial.print("."); if (month(t) < 10) {
        Serial.print("0");
      }; Serial.print(month(t)); Serial.print("."); Serial.print(year(t));
      Serial.print("   "); if (hour(t) < 10) {
        Serial.print("0");
      }; Serial.print(hour(t)); Serial.print(":"); if (minute(t) < 10) {
        Serial.print("0");
      }; Serial.print(minute(t)); Serial.println(" UTC");
      AOS = true;
      LOS = true;
      //  p13.printdata();
      //Serial.println();

      if (p13.EL > ELmax) {
        ELmax = p13.EL;
      }
    }
    if ((p13.EL < 15) & (LOS == true) & (AOS == true)  & (ALanz < maxPred) ) {
      Serial.print("LOS: "); if (day(t) < 10) {
        Serial.print("0");
      }; Serial.print(day(t)); Serial.print("."); if (month(t) < 10) {
        Serial.print("0");
      }; Serial.print(month(t)); Serial.print("."); Serial.print(year(t));
      Serial.print("   "); if (hour(t) < 10) {
        Serial.print("0");
      }; Serial.print(hour(t)); Serial.print(":"); if (minute(t) < 10) {
        Serial.print("0");
      }; Serial.print(minute(t)); Serial.println(" UTC");

      Serial.println("");

      LOS = false;
      AOS = false;
      ELmax = 0;
      ALanz += 1;
      if (ALanz == maxPred) {
        predCalc = true;
      };
    }

    if (predCalc == true) {
      Serial.print("Fertig.");
      predCalc = false;
    };

    uxtm += 60;
    setTime((uxtm));
  }
}

double getElement(char *gstr, int gstart, int gstop)
{
  double retval;
  int    k, glength;
  char   gestr[80];

  glength = gstop - gstart + 1;

  for (k = 0; k <= glength; k++)
  {
    gestr[k] = gstr[gstart + k - 1];
  }

  gestr[glength] = '\0';
  retval = atof(gestr);
  return (retval);
}

void readElements(int x)
{
  p13.setElements(getElement(elements[x][1], 19, 20) + 2000, getElement(elements[x][1], 21, 32), getElement(elements[x][2], 9, 16),
                  getElement(elements[x][2], 18, 25), getElement(elements[x][2], 27, 33) * 1.0e-7, getElement(elements[x][2], 35, 42), getElement(elements[x][2], 44, 51), getElement(elements[x][2], 53, 63),
                  getElement(elements[x][1], 34, 43), (getElement(elements[x][2], 64, 68) + ONEPPM), 0);
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}


