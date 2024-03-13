const int buttonPin1 = 2; // GPIO1
const int buttonPin2 = 3; // GPIO2

const int ledPinR = 13; // Diod-R
const int ledPinG = 12; // Diod-G
const int ledPinB = 11; // Diod-B

int potentioMeter = A0;

bool button1Enabled = true; // Variabel för att hålla koll på om knapp 1 är aktiverad
bool button2Enabled = true; // Variabel för att hålla koll på om knapp 2 är aktiverad

bool buttonState1 = false;
bool lastButtonState1 = false;
unsigned long lastDebounceTime1 = 0;
unsigned long debounceDelay1 = 50; // Debounce-fördröjning i millisekunder

bool buttonState2 = false;
bool lastButtonState2 = false;
unsigned long lastDebounceTime2 = 0;
unsigned long debounceDelay2 = 50;

int lastPotValue = 0;
unsigned long lastPotReadTime = 0;
unsigned long potDebounceDelay = 50; // Debounce-fördröjning för potentiometern i millisekunder

bool potentiometerEnabled = false;

void setup() {
  Serial.begin(9600);
  pinMode(potentioMeter, INPUT); // Potentiometern ska läsas av, så sätt den till INPUT
  pinMode(buttonPin1, INPUT_PULLUP); // Aktivera den inbyggda pull-up motståndet
  pinMode(buttonPin2, INPUT_PULLUP); // Aktivera den inbyggda pull-up motståndet
  pinMode(ledPinR, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  pinMode(ledPinB, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(buttonPin1), handleButton1Interrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPin2), handleButton2Interrupt, FALLING);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    executeCommand(command);
  }

  if (potentiometerEnabled && (millis() - lastPotReadTime >= potDebounceDelay)) {
    int val = analogRead(potentioMeter);
    val = map(val, 0, 1023, 0, 255); // Kartlägg potentiometeravläsningen till ett värde mellan 0 och 255

    if (val != lastPotValue) {
      analogWrite(ledPinB, val);
      Serial.print("Blue LED power set to ");
      Serial.println(val);
      lastPotValue = val;
    }
    lastPotReadTime = millis();
  }

  delay(10); // Låt en kort paus för att undvika flera avbrott
}

void handleButton1Interrupt() {
  if ((millis() - lastDebounceTime1) > debounceDelay1) {
    if (button1Enabled) {
      buttonState1 = !buttonState1;
      digitalWrite(ledPinR, buttonState1 ? HIGH : LOW);
    }
    lastDebounceTime1 = millis();
  }
}

void handleButton2Interrupt() {
  if ((millis() - lastDebounceTime2) > debounceDelay2) {
    if (button2Enabled) {
      buttonState2 = !buttonState2;
      digitalWrite(ledPinG, buttonState2 ? HIGH : LOW);
    }
    lastDebounceTime2 = millis();
  }
}

void executeCommand(String command) {
  if (command.startsWith("enable button 1")) {
    button1Enabled = true;
    Serial.println("Button 1 enabled.");
  } else if (command.startsWith("disable button 1")) {
    button1Enabled = false;
    Serial.println("Button 1 disabled.");
  } else if (command.startsWith("enable button 2")) {
    button2Enabled = true;
    Serial.println("Button 2 enabled.");
  } else if (command.startsWith("disable button 2")) {
    button2Enabled = false;
    Serial.println("Button 2 disabled.");
  } else if (command.startsWith("ledon 1")) {
    if (button1Enabled) {
      digitalWrite(ledPinR, HIGH);
      Serial.println("LED 1 turned on.");
    } else {
      Serial.println("Button 1 is disabled.");
    }
  } else if (command.startsWith("ledoff 1")) {
    if (button1Enabled) {
      digitalWrite(ledPinR, LOW);
      Serial.println("LED 1 turned off.");
    } else {
      Serial.println("Button 1 is disabled.");
    }
  } else if (command.startsWith("ledon 2")) {
    if (button2Enabled) {
      digitalWrite(ledPinG, HIGH);
      Serial.println("LED 2 turned on.");
    } else {
      Serial.println("Button 2 is disabled.");
    }
  } else if (command.startsWith("ledoff 2")) {
    if (button2Enabled) {
      digitalWrite(ledPinG, LOW);
      Serial.println("LED 2 turned off.");
    } else {
      Serial.println("Button 2 is disabled.");
    }
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
