byte green[4] = {5, 7, 10, 12};
byte red[4] = {6, 9, 11, A0};
byte orange[2] = {8, 13};
byte input[2] = {2, 3}; // switch, button

/*byte pin[2][5] = {{ 5,  6, 12, 13, A0},
  {10, 11,  7,  8, 9}
};*/

String text;
byte nb_step = 0;
bool right = false;
bool control = false;
bool button = false;
//                foot carO carR pause
byte interval[4] = {25, 5, 1, 5};

// Varialble for the timer
unsigned long previous = 0;
unsigned long current = 0;

const bool _step[4][5] = {
  // foot    car
  {1, 0, 1, 0, 0},  // G    G
  {0, 1, 1, 0, 0},  // R    G
  {0, 1, 0, 1, 0},  // R    O
  {0, 1, 0, 0, 1}   // R    R
};


void setup() {
  // Initialise communication
  Serial.begin(115200);

  // Initialise pins
  for (byte i = 0; i < 4; i++) {
    pinMode(green[i], OUTPUT);
    pinMode(red[i], OUTPUT);
  }
  for (byte i = 0; i < 2; i++) {
    pinMode(orange[i], OUTPUT);
  }
  for (byte i = 0; i < 2; i++) {
    pinMode(input[i], INPUT_PULLUP);
  }

  Serial.println("This is the serial of the traffic lights, enter command:");
  Serial.print('/');

  _stop();
  write_step(0, false);

}

void loop() {
  if (!control) {
    if (digitalRead(input[0])) { // Automatic step
      write_step(nb_step, right);
      current =  millis() / 1000;
      if (current - previous >= interval[nb_step]) {
        previous = current;
        nb_step++;
        if (nb_step == 4) {
          nb_step = 0;
          right = !right;
        }
      }
    } else { // Manual step
      if (!digitalRead(input[1]) && !button) {
        button = true;
        write_step(nb_step, right);
        nb_step++;
        if (nb_step == 4) {
          nb_step = 0;
          right = !right;
        }
      }
      if (digitalRead(input[1]) && button) {
        button = false;
        delay(100);
      }

    }
  }

  if (Serial.available()) {
    control = true;
    text = Serial.readString();
    while (Serial.available()) {
      Serial.read();
    }
    Serial.println(text);
    if (text == "off") {
      _off();
      Serial.println("All lights are off");
      Serial.print("\n/");
    } else if (text == "stop") {
      _stop();
      Serial.println("Only red lights are on");
      Serial.print("\n/");
    } else if (text == "end") {
      control = false;
      _stop();
      Serial.println("End the control with serial");
      Serial.print("\n/");
    } else if (text == "step") {
      Serial.println("Control light's step write '1' if right or '0' if left then the step(from 0 to 3)");
      Serial.println("To finish this program send something else");
      Serial.println("\tfoot car");
      Serial.println("\tG R G O R");
      Serial.println("\t1 0 1 0 0 // Step 0");
      Serial.println("\t0 1 1 0 0 // Step 1");
      Serial.println("\t0 1 0 1 0 // Step 2");
      Serial.println("\t0 1 0 0 1 // Step 3");
      while (1) {
        while (!Serial.available());
        char lire = Serial.read();
        if (isDigit(lire)) {
          if (String(lire).toInt() < 4) {
            write_step(String(lire).toInt(), String(Serial.read()).toInt());
          } else {
            break;
          }
        } else {
          break;
        }
      }
      while (Serial.available()) {
        Serial.read();
      }
      Serial.println("End of control by step");
      Serial.println("To restore the traffic cycle use /end");
      Serial.print("\n/");
    } else if (text == "time") {
      control = false;
      Serial.println("Time between two traffic lights step:");
      Serial.println("\tG to R for people:    " + String(interval[0]));
      Serial.println("\tG to O for cars:      " + String(interval[1]));
      Serial.println("\tO to R for cars:      " + String(interval[2]));
      Serial.println("\tR to G for everybody: " + String(interval[3]));
      Serial.print("\n\tCycle time: ");
      Serial.println(interval[0] + interval[1] + interval[2] + interval[3]);
      Serial.print("\n/");
    } else if (text == "set") {
      control = false;
      Serial.println("Set the time between two traffic lights step:");
      Serial.println("\tG to R for people:    " + String(interval[0]));
      Serial.println("\tG to O for cars:      " + String(interval[1]));
      Serial.println("\tO to R for cars:      " + String(interval[2]));
      Serial.println("\tR to G for everybody: " + String(interval[3]));
      Serial.print("\n\tCycle time: ");
      Serial.println(interval[0] + interval[1] + interval[2] + interval[3]);

      for(byte i = 0; i< 4;i++){
        Serial.println("Enter a time to set interval "+ String(i));
        while(!Serial.available());
        interval[i] = Serial.readString().toInt();
      }
      
      Serial.print("\n/");
    }

    else if (text == "help") {
      Serial.println();
      Serial.println("Mode I: automatic traffic cycle");
      Serial.println("Mode II: manual traffic cycle");
      Serial.println();
      Serial.println("/help\t\t It shows this");
      Serial.println("/off\t\t It turns off all lights");
      Serial.println("/end\t\t It restores the automatic traffic cycle");
      Serial.println("/step\t\t It controls lights by step");
      Serial.println("/stop\t\t It turns on only red lights");
      Serial.println("/time\t\t It shows the time between two step");
      Serial.println("/set\t\t It sets the time between two step");
      Serial.print("\n/");
    } else {
      Serial.println("Unknow command");
      Serial.println("Use /help to see commands");
      Serial.print("\n/");
    }
  }
}

void _stop() {
  // Turn on red lights
  for (byte i = 0; i < 4; i++) {
    digitalWrite(red[i], HIGH);
  }

  // Turn off the others
  for (byte i = 0; i < 4; i++) {
    digitalWrite(green[i], LOW);
  }
  for (byte i = 0; i < 2; i++) {
    digitalWrite(orange[i], LOW);
  }
}

void _off() {
  // Turn off all lights
  for (byte i = 0; i < 4; i++) {
    digitalWrite(red[i], LOW);
    digitalWrite(green[i], LOW);
  }
  for (byte i = 0; i < 2; i++) {
    digitalWrite(orange[i], LOW);
  }
}

void write_step(byte stepState, bool _right) {
  for (byte i = 0; i < 5; i++) {
    digitalWrite(pin[_right][i], _step[stepState][i]);
  }
}
