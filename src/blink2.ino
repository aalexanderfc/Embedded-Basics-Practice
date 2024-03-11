const int buttonPin1 = 2; //GPIO1
const int buttonPin2 = 3; //GPIO2

const int ledPinR = 13; //Diod-R
const int ledPinG = 12; //Diod-G
const int ledPinB = 11; //Diod-B

int potentioMeter = A0;

bool buttonState1 = false;
bool lastButtonState1 = false;
unsigned long lastDebounceTime1 = 0;

bool buttonState2 = false;
bool lastButtonState2 = false;
unsigned long lastDebounceTime2 = 0;

unsigned long debounceDelay = 50;

int lastPotValue = 0;
bool potentiometerEnabled = false;

void setup() {
  Serial.begin(9600);
  pinMode(potentioMeter, INPUT); // Potentiometern ska läsas av, så sätt den till INPUT
  pinMode(buttonPin1, INPUT_PULLUP); // Aktivera den inbyggda pull-up motståndet
  pinMode(buttonPin2, INPUT_PULLUP); // Aktivera den inbyggda pull-up motståndet
  pinMode(ledPinR, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  pinMode(ledPinB, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    executeCommand(command);
  }

  if (potentiometerEnabled) {
    int val = analogRead(potentioMeter);
    val = map(val, 0, 1023, 0, 255); // Kartlägg potentiometeravläsningen till intervallet för PWM

    if (val != lastPotValue) {
      analogWrite(ledPinB, val);
      Serial.print("Blue LED power set to ");
      Serial.println(val);
      lastPotValue = val;
    }
  }

  int reading1 = digitalRead(buttonPin1);//för att läsa tillståndet av knapp 1
  if (reading1 != lastButtonState1) {
    lastDebounceTime1 = millis();
  }

  if ((millis() - lastDebounceTime1) > debounceDelay) {
    if (reading1 != buttonState1) {
      buttonState1 = reading1;

      if (buttonState1 == LOW) {
        digitalWrite(ledPinR, !digitalRead(ledPinR));
      }
    }
  }

  lastButtonState1 = reading1;

  int reading2 = digitalRead(buttonPin2);//för att läsa tillståndet av knapp 2
  if (reading2 != lastButtonState2) {
    lastDebounceTime2 = millis();
  }

  if ((millis() - lastDebounceTime2) > debounceDelay) {
    if (reading2 != buttonState2) {
      buttonState2 = reading2;

      if (buttonState2 == LOW) {
        digitalWrite(ledPinG, !digitalRead(ledPinG));
      }
    }
  }

  lastButtonState2 = reading2;

  delay(100);
}

void executeCommand(String command) {
  if (command.startsWith("enable button 1")) {
    pinMode(buttonPin1, INPUT_PULLUP);
    Serial.println("Button 1 enabled.");
  } else if (command.startsWith("disable button 1")) {
    pinMode(buttonPin1, INPUT);
    Serial.println("Button 1 disabled.");
  } else if (command.startsWith("enable button 2")) {
    pinMode(buttonPin2, INPUT_PULLUP);
    Serial.println("Button 2 enabled.");
  } else if (command.startsWith("disable button 2")) {
    pinMode(buttonPin2, INPUT);
    Serial.println("Button 2 disabled.");
  } else if (command.startsWith("ledon 1")) {
    digitalWrite(ledPinR, HIGH);
    Serial.println("LED 1 turned on.");
  } else if (command.startsWith("ledoff 1")) {
    digitalWrite(ledPinR, LOW);
    Serial.println("LED 1 turned off.");
  } else if (command.startsWith("ledon 2")) {
    digitalWrite(ledPinG, HIGH);
    Serial.println("LED 2 turned on.");
  } else if (command.startsWith("ledoff 2")) {
    digitalWrite(ledPinG, LOW);
    Serial.println("LED 2 turned off.");
  } else if (command.startsWith("ledpower ")) {
    int powerValue = command.substring(9).toInt();
    if (powerValue == -1) {
      potentiometerEnabled = true; // Aktivera potentiometern
      Serial.println("Potentiometer enabled.");
    } else {
      potentiometerEnabled = false; // Stäng av potentiometern
      analogWrite(ledPinB, powerValue);
      Serial.println("Blue LED power set to " + String(powerValue));
    }
  }
}

