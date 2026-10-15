// Global Variables - Motor Control Pins
const int ENA = 13; // PWM pin Left Motor
const int IN1 = 12; // Direction Left Motor
const int IN2 = 11; // Direction Left Motor
const int IN3 = 10; // Direction Right Motor
const int IN4 = 9;  // Direction Right Motor
const int ENB = 8;  // PWM pin Right Motor

// 5-Sensor Pin Layout
const int SENSOR_FAR_LEFT  = 31; 
const int SENSOR_LEFT      = 33;
const int SENSOR_CENTER    = 35; 
const int SENSOR_RIGHT     = 37; 
const int SENSOR_FAR_RIGHT = 39;

// Speed Control
const int CRUISE_SPEED = 70; 
const int SHARP_SPEED  = 140; 

char pathMemory[100]; // Stores up to 100 choices
int junctionCount = 0;
  
void setup() {
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
     forward(CRUISE_SPEED, CRUISE_SPEED); 
    //handleMazeJunction();
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
void forward(int leftSpeed, int rightSpeed) {
  analogWrite(ENA, leftSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  analogWrite(ENB, rightSpeed);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
// CORRECTED MOTOR DIRECTIONS BELOW:
void spinRight(int speed) {
  // Pivot left: Left motor goes BACKWARD, Right motor goes FORWARD
  analogWrite(ENA, speed);
  digitalWrite(IN1, HIGH); 
  digitalWrite(IN2, LOW);
  
  analogWrite(ENB, speed);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void spinLeft(int speed) {
  // Pivot right: Left motor goes FORWARD, Right motor goes BACKWARD
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


// --- MAZE JUNCTION NAVIGATION (Left-Hand Rule Exploration) ---
void handleMazeJunction() {
  // 1. Brake immediately to cancel momentum
  stopRobot();
  delay(100); 

  // 2. Read the initial state of the turn options before inching forward
  int leftAvailable = digitalRead(SENSOR_FAR_LEFT);
  int rightAvailable = digitalRead(SENSOR_FAR_RIGHT);

  // 3. Move forward slightly to put the wheel-pivot-axis directly over the junction
  // and see if a straight path exists past the intersection
  forward(CRUISE_SPEED, CRUISE_SPEED);
  delay(60); // Adjust this delay so the center sensor crosses past the crossline
  stopRobot();

  int straightAvailable = digitalRead(SENSOR_CENTER);

  // 4. APPLY THE EXPLORATION PRIORITY (Left -> Straight -> Right)
  
  if (leftAvailable == 0) {
    // A left path is open, always take it first to follow the wall
    // Spin until the center sensor locks back onto the new line
    while (digitalRead(SENSOR_CENTER) == 1) {
      spinLeft(SHARP_SPEED);
    }
    stopRobot();
    delay(50);
  }
  else if (straightAvailable == 0) {
    // No left turn, but we can keep going straight
    forward(CRUISE_SPEED, CRUISE_SPEED);
  }
  else if (rightAvailable == 0) {
    // No left, no straight, it's a forced right turn
    while (digitalRead(SENSOR_CENTER) == 1) {
      spinRight(SHARP_SPEED);
    }
    stopRobot();
    delay(50);
  }
}

// --- DEAD END HANDLER ---
void handleDeadEnd() {
  stopRobot();
  delay(100);
  
  // Pivot 180 degrees using your working spinLeft until the line is re-acquired
  while (digitalRead(SENSOR_CENTER) == 1) {
    spinLeft(SHARP_SPEED);
  }
  stopRobot();
  delay(50);
}
