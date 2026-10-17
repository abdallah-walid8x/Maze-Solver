// Global Variables - Motor Control Pins
const int ENA = 13; // PWM pin Left Motor
const int IN1 = 12; // Direction Left Motor
const int IN2 = 11; // Direction Left Motor
const int IN3 = 10; // Direction Right Motor
const int IN4 = 9;  // Direction Right Motor
const int ENB = 8;  // PWM pin Right Motor

// 5-Sensor Pin Layout (0 = Black Line, 1 = White Floor)
const int SENSOR_FAR_LEFT  = 31;
const int SENSOR_LEFT      = 33;
const int SENSOR_CENTER    = 35;
const int SENSOR_RIGHT     = 37;
const int SENSOR_FAR_RIGHT = 39;

// Speed Control
const int CRUISE_SPEED = 70;
const int SHARP_SPEED  = 140;

// Internal Representation of the Track
char pathMemory[100]; // Stores choices: 'L' (Left), 'S' (Straight), 'R' (Right), 'B' (Back/U-turn)
int junctionCount = 0;

// --- MOTOR BASIC MOVEMENTS ---
void forward(int leftSpeed, int rightSpeed) {
  analogWrite(ENA, leftSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENB, rightSpeed);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void spinRight(int speed) {
  analogWrite(ENA, speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENB, speed);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void spinLeft(int speed) {
  analogWrite(ENA, speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENB, speed);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void stopRobot() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// --- LOGGING FUNCTION ---
void logTrackRepresentation(char action) {
  if (junctionCount < 100) {
    pathMemory[junctionCount] = action;
    junctionCount++;
  }
}

void setup() {
   Serial.begin(9600);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  
  pinMode(SENSOR_FAR_LEFT, INPUT_PULLUP);
  pinMode(SENSOR_LEFT, INPUT_PULLUP);
  pinMode(SENSOR_CENTER, INPUT_PULLUP);
  pinMode(SENSOR_RIGHT, INPUT_PULLUP);
  pinMode(SENSOR_FAR_RIGHT, INPUT_PULLUP);
}
void printPathFormatted() {
  Serial.print("Path Taken [");
  Serial.print(junctionCount);
  Serial.print("]: ");
  
  for (int i = 0; i < junctionCount; i++) {
    Serial.print(pathMemory[i]);
    if (i < junctionCount - 1) {
      Serial.print(" -> "); // Visual separation between turns
    }
  }
  Serial.println(); // New line at the end
}
void loop() {
  // 0 = Black Line, 1 = White Floor
  int sFarL = digitalRead(SENSOR_FAR_LEFT);
  int sL    = digitalRead(SENSOR_LEFT);
  int sC    = digitalRead(SENSOR_CENTER);
  int sR    = digitalRead(SENSOR_RIGHT);
  int sFarR = digitalRead(SENSOR_FAR_RIGHT);
// PRIORITY 1: FULL INTERSECTIONS / T-JUNCTIONS
  // Handled entirely on its own at the top level
  if (sFarL == 1 && sL == 1 && sC == 0 && sR == 1 && sFarR == 1) {
    forward(CRUISE_SPEED, CRUISE_SPEED); 
  }
  // --- START OF THE BIG ELSE ENCLOSING ALL OTHER CONDITIONS ---
  else {
    if (sFarL == 0 && sL == 0 && sC == 0 && sR == 0 && sFarR == 0) {
     //forward(CRUISE_SPEED, CRUISE_SPEED); 
    handleMazeJunction();
  }
    // PRIORITY 2: SHARP BENDS (Hard Pivots)
    else if ((sFarL == 1 && sL == 1 && sC == 0 && sR == 0 && sFarR == 0) || (sFarL == 1 && sL == 1 && sC == 1 && sR == 0 && sFarR == 0) || (sFarL == 1 && sL == 0 && sC == 0 && sR == 0 && sFarR == 0) || (sFarL == 0 && sL == 1 && sC == 1 && sR == 1 && sFarR == 0) ) {
     
      spinRight(SHARP_SPEED); 
       
    }
    else if ((sFarL == 0 && sL == 0 && sC == 0 && sR == 1 && sFarR == 1) || (sFarL == 0 && sL == 0 && sC == 1 && sR == 1 && sFarR == 1) || (sFarL == 0 && sL == 0 && sC == 0 && sR == 0 && sFarR == 1)) {
      
      spinLeft(SHARP_SPEED); 
       
    }

    // PRIORITY 3: SLIGHT DRIFT & ADJUSTMENTS (Smooth differential steering)
    else if ((sC == 1 && sR == 0) || (sL == 1 && sC == 0)) {
      forward(CRUISE_SPEED, CRUISE_SPEED / 1.5); 
    }
    else if ((sC == 1 && sL == 0) || (sR == 1 && sC == 0)) {
      forward(CRUISE_SPEED / 1.5, CRUISE_SPEED); 
    }

    // PRIORITY 4: TRACK CENTERED
     // else if (sC == 1) {
   //   forward(CRUISE_SPEED, CRUISE_SPEED); 
   // }

    // PRIORITY 5: TRACK LOST / + JUNCTION
    else if ((sFarL == 1 && sL == 1 && sC == 1 && sR == 1 && sFarR == 1)) {
      spinLeft(SHARP_SPEED);
      //handleDeadEnd(); 
    }
    
  } // --- END OF THE BIG ELSE ---
}

// --- MAZE JUNCTION NAVIGATION & INTERNAL REPRESENTATION ---
void handleMazeJunction() {
  //stopRobot();
  //delay(100); // Stabilize

  // Read initial layout choices
  int leftAvailable  = !digitalRead(SENSOR_FAR_LEFT);  // True if black line detected
  int rightAvailable = !digitalRead(SENSOR_FAR_RIGHT); // True if black line detected

  // Inch forward slightly to check if a straight path exists past the crossline
  forward(CRUISE_SPEED, CRUISE_SPEED);
  //delay(80); // Adjust based on chassis speed to overlay the pivot axis on the junction
  //stopRobot();

  int straightAvailable = !digitalRead(SENSOR_CENTER);

  // Apply Left-Hand Rule and log decisions ('L', 'S', 'R') to build internal track map
  if (leftAvailable) {
    logTrackRepresentation('L');
    // Pivot Left until center re-acquires line
    spinLeft(SHARP_SPEED);
  //  while (digitalRead(SENSOR_CENTER) == 1);
  //  stopRobot();
  } 
  else if (straightAvailable) {
    logTrackRepresentation('S');
    forward(CRUISE_SPEED, CRUISE_SPEED);
  } 
  else if (rightAvailable) {
    logTrackRepresentation('R');
    // Pivot Right until center re-acquires line
    spinRight(SHARP_SPEED);
 //   while (digitalRead(SENSOR_CENTER) == 1);
  //  stopRobot();
   }
}

// --- DEAD END HANDLER ---
void handleDeadEnd() {
 // stopRobot();
 // delay(100);
  
  logTrackRepresentation('B'); // Log Backtrack/U-Turn action

  // Execute 180-degree pivot until line is found again
  spinLeft(SHARP_SPEED);
 // delay(200); // Give it time to clear the current spot before checking sensor
 // while (digitalRead(SENSOR_CENTER) == 1);
 // stopRobot();
}
