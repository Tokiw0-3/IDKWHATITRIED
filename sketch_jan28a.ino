#include <Servo.h>
// ==== Physicall parts ====
// Create servo objects for 5 fingers
Servo thumb;
Servo index;
Servo middle;
Servo ring;
Servo pinky;

// Servo pins
const int THUMB_PIN = 3;
const int INDEX_PIN = 4;
const int MIDDLE_PIN = 5;
const int RING_PIN = 6;
const int PINKY_PIN = 7;

// Force sensor pins 
const int THUMB_SENSOR = A1;
const int INDEX_SENSOR = A2;
const int MIDDLE_SENSOR = A3;
const int RING_SENSOR = A4;
const int PINKY_SENSOR = A5;

// Position constants in microseconds
const int OPEN_POS = 1000;      // 0 degrees - fully open
const int CLOSED_POS = 2000;    // 180 degrees - fully closed

// ===== EASY CONFIGURATION =====
const unsigned long FULL_MOVEMENT_TIME = 1200; // Change this value (in milliseconds)
// Examples: 800 = fast, 1200 = medium, 2000 = slow

const bool debugMode = true; // Set to false to disable debug outputs

// Force sensor thresholds 
int thumbThreshold = 900;
int indexThreshold = 900;
int middleThreshold = 900;
int ringThreshold = 900;
int pinkyThreshold = 900;

// Force lock flags
bool thumbLocked = false;
bool indexLocked = false;
bool middleLocked = false;
bool ringLocked = false;
bool pinkyLocked = false;

// Movement timing - tracks when fingers are moving
bool thumbMoving = false;
bool indexMoving = false;
bool middleMoving = false;
bool ringMoving = false;
bool pinkyMoving = false;

unsigned long thumbMoveStart = 0;
unsigned long indexMoveStart = 0;
unsigned long middleMoveStart = 0;
unsigned long ringMoveStart = 0;
unsigned long pinkyMoveStart = 0;

// Track remaining movement time (for partial movements)
unsigned long thumbRemainingTime = 0;
unsigned long indexRemainingTime = 0;
unsigned long middleRemainingTime = 0;
unsigned long ringRemainingTime = 0;
unsigned long pinkyRemainingTime = 0;

// Track current finger position state
bool thumbOpen = true;
bool indexOpen = true;
bool middleOpen = true;
bool ringOpen = true;
bool pinkyOpen = true;

// Track servo attachment state
bool thumbAttached = false;
bool indexAttached = false;
bool middleAttached = false;
bool ringAttached = false;
bool pinkyAttached = false;

void setup()
{
    // Initialize serial
    Serial.begin(9600);
    Serial.println("=== Robot Hand Controller ===");
    Serial.print("Movement time: ");
    Serial.print(FULL_MOVEMENT_TIME);
    Serial.println("ms");
    Serial.println("\n--- PRESET GESTURES ---");
    Serial.println("  o - Open all fingers");
    Serial.println("  c - Close all fingers (fist)");
    Serial.println("  f - Close all EXCEPT middle finger");
    Serial.println("  p - Pinch grip (thumb + index)");
    Serial.println("\n--- INDIVIDUAL FINGERS ---");
    Serial.println("  1 - Toggle Pinky");
    Serial.println("  2 - Toggle Ring");
    Serial.println("  3 - Toggle Middle");
    Serial.println("  4 - Toggle Index");
    Serial.println("  5 - Toggle Thumb");
    Serial.println("\n--- UTILITY ---");
    Serial.println("  u - Unlock all fingers");
    Serial.println("  r - Read force sensors");
    Serial.println("  s - Stop all (detach servos)");

    // Start with all servos detached (no signal)
    Serial.println("\nInitializing with servos detached (no signal)...");
    
    delay(500);
    Serial.println("Ready!");
}

void loop()
{
    // Check movement timers and auto-return to neutral
    checkMovementTimers();

    // Maintain servo positions
    maintainServoPositions();

    // Check force sensors
    checkForceSensors();

    // Process serial commands
    if (Serial.available() > 0)
    {
        processSerialCommand();
    }
}

// Check if movement duration has elapsed and return to neutral
void checkMovementTimers()
{
    unsigned long currentTime = millis();

    if (thumbMoving && (currentTime - thumbMoveStart >= thumbRemainingTime))
    {
        thumbMoving = false;
        thumbRemainingTime = FULL_MOVEMENT_TIME;
        if (debugMode) Serial.println("Thumb returned to neutral (detached)");
    }

    if (indexMoving && (currentTime - indexMoveStart >= indexRemainingTime))
    {
        indexMoving = false;
        indexRemainingTime = FULL_MOVEMENT_TIME;
        if (debugMode) Serial.println("Index returned to neutral (detached)");
    }

    if (middleMoving && (currentTime - middleMoveStart >= middleRemainingTime))
    {
        middleMoving = false;
        middleRemainingTime = FULL_MOVEMENT_TIME;
        if (debugMode) Serial.println("Middle returned to neutral (detached)");
    }

    if (ringMoving && (currentTime - ringMoveStart >= ringRemainingTime))
    {
        ringMoving = false;
        ringRemainingTime = FULL_MOVEMENT_TIME;
        if (debugMode) Serial.println("Ring returned to neutral (detached)");
    }

    if (pinkyMoving && (currentTime - pinkyMoveStart >= pinkyRemainingTime))
    {
        pinkyMoving = false;
        pinkyRemainingTime = FULL_MOVEMENT_TIME;
        if (debugMode) Serial.println("Pinky returned to neutral (detached)");
    }
}

// Continuously write positions based on state or detach when static
void maintainServoPositions()
{
    if (!thumbLocked)
    {
        if (thumbMoving)
        {
            if (!thumbAttached)
            {
                thumb.attach(THUMB_PIN);
                thumbAttached = true;
            }
            thumb.writeMicroseconds(thumbOpen ? OPEN_POS : CLOSED_POS);
        }
        else
        {
            if (thumbAttached)
            {
                thumb.detach();
                thumbAttached = false;
            }
        }
    }

    if (!indexLocked)
    {
        if (indexMoving)
        {
            if (!indexAttached)
            {
                index.attach(INDEX_PIN);
                indexAttached = true;
            }
            index.writeMicroseconds(indexOpen ? OPEN_POS : CLOSED_POS);
        }
        else
        {
            if (indexAttached)
            {
                index.detach();
                indexAttached = false;
            }
        }
    }

    if (!middleLocked)
    {
        if (middleMoving)
        {
            if (!middleAttached)
            {
                middle.attach(MIDDLE_PIN);
                middleAttached = true;
            }
            middle.writeMicroseconds(middleOpen ? OPEN_POS : CLOSED_POS);
        }
        else
        {
            if (middleAttached)
            {
                middle.detach();
                middleAttached = false;
            }
        }
    }

    if (!ringLocked)
    {
        if (ringMoving)
        {
            if (!ringAttached)
            {
                ring.attach(RING_PIN);
                ringAttached = true;
            }
            ring.writeMicroseconds(ringOpen ? OPEN_POS : CLOSED_POS);
        }
        else
        {
            if (ringAttached)
            {
                ring.detach();
                ringAttached = false;
            }
        }
    }

    if (!pinkyLocked)
    {
        if (pinkyMoving)
        {
            if (!pinkyAttached)
            {
                pinky.attach(PINKY_PIN);
                pinkyAttached = true;
            }
            pinky.writeMicroseconds(pinkyOpen ? OPEN_POS : CLOSED_POS);
        }
        else
        {
            if (pinkyAttached)
            {
                pinky.detach();
                pinkyAttached = false;
            }
        }
    }
}

// Check all force sensors
void checkForceSensors()
{
    if (!thumbLocked && thumbMoving && analogRead(THUMB_SENSOR) > thumbThreshold)
    {
        thumbLocked = true;
        thumbMoving = false;
        Serial.println("THUMB LOCKED - Stopped!");
    }

    if (!indexLocked && indexMoving && analogRead(INDEX_SENSOR) > indexThreshold)
    {
        indexLocked = true;
        indexMoving = false;
        Serial.println("INDEX LOCKED - Stopped!");
    }

    if (!middleLocked && middleMoving && analogRead(MIDDLE_SENSOR) > middleThreshold)
    {
        middleLocked = true;
        middleMoving = false;
        Serial.println("MIDDLE LOCKED - Stopped!");
    }

    if (!ringLocked && ringMoving && analogRead(RING_SENSOR) > ringThreshold)
    {
        ringLocked = true;
        ringMoving = false;
        Serial.println("RING LOCKED - Stopped!");
    }

    if (!pinkyLocked && pinkyMoving && analogRead(PINKY_SENSOR) > pinkyThreshold)
    {
        pinkyLocked = true;
        pinkyMoving = false;
        Serial.println("PINKY LOCKED - Stopped!");
    }
}

// Process serial commands
void processSerialCommand()
{
    char cmd = Serial.read();

    // Skip whitespace
    if (cmd == ' ' || cmd == '\n' || cmd == '\r') return;

    if (debugMode)
    {
        Serial.print("Command: ");
        Serial.println(cmd);
    }

    switch (cmd)
    {
        // PRESET GESTURES
        case 'o':
        case 'O':
            openAll();
            break;

        case 'c':
        case 'C':
            closeAll();
            break;

        case 'f':
        case 'F':
            closeAllExceptMiddle();
            break;

        case 'p':
        case 'P':
            pinchGrip();
            break;

        // INDIVIDUAL FINGER TOGGLES
        case '1':
            togglePinky();
            break;

        case '2':
            toggleRing();
            break;

        case '3':
            toggleMiddle();
            break;

        case '4':
            toggleIndex();
            break;

        case '5':
            toggleThumb();
            break;

        // UTILITY COMMANDS
        case 'u':
        case 'U':
            unlockAllFingers();
            break;

        case 'r':
        case 'R':
            readAllSensors();
            break;

        case 's':
        case 'S':
            stopAll();
            break;

        default:
            Serial.println("Unknown command!");
            break;
    }
}

// Helper function to start a finger movement with partial movement tracking
void startFingerMovement(bool &isMoving, bool &targetOpen, bool newTargetOpen, 
                        unsigned long &moveStart, unsigned long &remainingTime)
{
    unsigned long now = millis();
    
    if (isMoving)
    {
        // Calculate how much time has elapsed in current movement
        unsigned long elapsed = now - moveStart;
        
        // Clamp elapsed to remainingTime to avoid overflow
        if (elapsed > remainingTime) elapsed = remainingTime;
        
        unsigned long timeLeft = remainingTime - elapsed;
        
        if (targetOpen == newTargetOpen)
        {
            // Same direction: continuing movement
            // New time = Total time - Elapsed time
            remainingTime = FULL_MOVEMENT_TIME - elapsed;
            if (debugMode)
            {
                Serial.print("  [Same direction: ");
                Serial.print(elapsed);
                Serial.print("ms elapsed, ");
                Serial.print(remainingTime);
                Serial.println("ms remaining]");
            }
        }
        else
        {
            // Opposite direction: reversing movement
            // New time = Total time - Remaining time of previous movement
            remainingTime = FULL_MOVEMENT_TIME - timeLeft;
            if (debugMode)
            {
                Serial.print("  [Reverse direction: ");
                Serial.print(timeLeft);
                Serial.print("ms left to undo, need ");
                Serial.print(remainingTime);
                Serial.println("ms to return]");
            }
        }
    }
    else
    {
        // Starting fresh movement
        remainingTime = FULL_MOVEMENT_TIME;
        if (debugMode)
        {
            Serial.print("  [Fresh movement: ");
            Serial.print(remainingTime);
            Serial.println("ms]");
        }
    }
    
    targetOpen = newTargetOpen;
    isMoving = true;
    moveStart = now;
}

// ===== PRESET GESTURES =====

void openAll()
{
    if (debugMode) Serial.println("Opening all fingers...");

    if (!thumbLocked && !thumbOpen)
    {
        startFingerMovement(thumbMoving, thumbOpen, true, thumbMoveStart, thumbRemainingTime);
    }
    if (!indexLocked && !indexOpen)
    {
        startFingerMovement(indexMoving, indexOpen, true, indexMoveStart, indexRemainingTime);
    }
    if (!middleLocked && !middleOpen)
    {
        startFingerMovement(middleMoving, middleOpen, true, middleMoveStart, middleRemainingTime);
    }
    if (!ringLocked && !ringOpen)
    {
        startFingerMovement(ringMoving, ringOpen, true, ringMoveStart, ringRemainingTime);
    }
    if (!pinkyLocked && !pinkyOpen)
    {
        startFingerMovement(pinkyMoving, pinkyOpen, true, pinkyMoveStart, pinkyRemainingTime);
    }
}

void closeAll()
{
    if (debugMode) Serial.println("Closing all fingers (fist)...");

    if (!thumbLocked && thumbOpen)
    {
        startFingerMovement(thumbMoving, thumbOpen, false, thumbMoveStart, thumbRemainingTime);
    }
    if (!indexLocked && indexOpen)
    {
        startFingerMovement(indexMoving, indexOpen, false, indexMoveStart, indexRemainingTime);
    }
    if (!middleLocked && middleOpen)
    {
        startFingerMovement(middleMoving, middleOpen, false, middleMoveStart, middleRemainingTime);
    }
    if (!ringLocked && ringOpen)
    {
        startFingerMovement(ringMoving, ringOpen, false, ringMoveStart, ringRemainingTime);
    }
    if (!pinkyLocked && pinkyOpen)
    {
        startFingerMovement(pinkyMoving, pinkyOpen, false, pinkyMoveStart, pinkyRemainingTime);
    }
}

void closeAllExceptMiddle()
{
    if (debugMode) Serial.println("Closing all except middle finger...");

    if (!thumbLocked && thumbOpen)
    {
        startFingerMovement(thumbMoving, thumbOpen, false, thumbMoveStart, thumbRemainingTime);
    }
    if (!indexLocked && indexOpen)
    {
        startFingerMovement(indexMoving, indexOpen, false, indexMoveStart, indexRemainingTime);
    }
    if (!middleLocked && !middleOpen)
    {
        startFingerMovement(middleMoving, middleOpen, true, middleMoveStart, middleRemainingTime);
    }
    if (!ringLocked && ringOpen)
    {
        startFingerMovement(ringMoving, ringOpen, false, ringMoveStart, ringRemainingTime);
    }
    if (!pinkyLocked && pinkyOpen)
    {
        startFingerMovement(pinkyMoving, pinkyOpen, false, pinkyMoveStart, pinkyRemainingTime);
    }
}

void pinchGrip()
{
    if (debugMode) Serial.println("Pinch grip (thumb + index)...");

    if (!thumbLocked && thumbOpen)
    {
        startFingerMovement(thumbMoving, thumbOpen, false, thumbMoveStart, thumbRemainingTime);
    }
    if (!indexLocked && indexOpen)
    {
        startFingerMovement(indexMoving, indexOpen, false, indexMoveStart, indexRemainingTime);
    }
    if (!middleLocked && !middleOpen)
    {
        startFingerMovement(middleMoving, middleOpen, true, middleMoveStart, middleRemainingTime);
    }
    if (!ringLocked && !ringOpen)
    {
        startFingerMovement(ringMoving, ringOpen, true, ringMoveStart, ringRemainingTime);
    }
    if (!pinkyLocked && !pinkyOpen)
    {
        startFingerMovement(pinkyMoving, pinkyOpen, true, pinkyMoveStart, pinkyRemainingTime);
    }
}

// ===== INDIVIDUAL FINGER CONTROL =====

void toggleThumb()
{
    if (thumbLocked)
    {
        if (debugMode) Serial.println("Thumb is LOCKED! Use 'u' to unlock.");
        return;
    }

    bool newTarget = !thumbOpen;
    startFingerMovement(thumbMoving, thumbOpen, newTarget, thumbMoveStart, thumbRemainingTime);
    if (debugMode)
    {
        Serial.print("Thumb -> ");
        Serial.println(thumbOpen ? "OPEN" : "CLOSED");
    }
}

void toggleIndex()
{
    if (indexLocked)
    {
        if (debugMode) Serial.println("Index is LOCKED! Use 'u' to unlock.");
        return;
    }

    bool newTarget = !indexOpen;
    startFingerMovement(indexMoving, indexOpen, newTarget, indexMoveStart, indexRemainingTime);
    if (debugMode)
    {
        Serial.print("Index -> ");
        Serial.println(indexOpen ? "OPEN" : "CLOSED");
    }
}

void toggleMiddle()
{
    if (middleLocked)
    {
        if (debugMode) Serial.println("Middle is LOCKED! Use 'u' to unlock.");
        return;
    }

    bool newTarget = !middleOpen;
    startFingerMovement(middleMoving, middleOpen, newTarget, middleMoveStart, middleRemainingTime);
    if (debugMode)
    {
        Serial.print("Middle -> ");
        Serial.println(middleOpen ? "OPEN" : "CLOSED");
    }
}

void toggleRing()
{
    if (ringLocked)
    {
        if (debugMode) Serial.println("Ring is LOCKED! Use 'u' to unlock.");
        return;
    }

    bool newTarget = !ringOpen;
    startFingerMovement(ringMoving, ringOpen, newTarget, ringMoveStart, ringRemainingTime);
    if (debugMode)
    {
        Serial.print("Ring -> ");
        Serial.println(ringOpen ? "OPEN" : "CLOSED");
    }
}

void togglePinky()
{
    if (pinkyLocked)
    {
        if (debugMode) Serial.println("Pinky is LOCKED! Use 'u' to unlock.");
        return;
    }

    bool newTarget = !pinkyOpen;
    startFingerMovement(pinkyMoving, pinkyOpen, newTarget, pinkyMoveStart, pinkyRemainingTime);
    if (debugMode)
    {
        Serial.print("Pinky -> ");
        Serial.println(pinkyOpen ? "OPEN" : "CLOSED");
    }
}

// ===== UTILITY FUNCTIONS =====

void unlockAllFingers()
{
    thumbLocked = false;
    indexLocked = false;
    middleLocked = false;
    ringLocked = false;
    pinkyLocked = false;
    stopAll();
    if (debugMode) Serial.println("All fingers UNLOCKED");
}

void readAllSensors()
{
    Serial.println("\n=== Force Sensors ===");
    Serial.print("Thumb:  "); Serial.println(analogRead(THUMB_SENSOR));
    Serial.print("Index:  "); Serial.println(analogRead(INDEX_SENSOR));
    Serial.print("Middle: "); Serial.println(analogRead(MIDDLE_SENSOR));
    Serial.print("Ring:   "); Serial.println(analogRead(RING_SENSOR));
    Serial.print("Pinky:  "); Serial.println(analogRead(PINKY_SENSOR));
    Serial.println();
}

void stopAll()
{
    thumbMoving = false;
    indexMoving = false;
    middleMoving = false;
    ringMoving = false;
    pinkyMoving = false;
    
    // Reset remaining times to default
    thumbRemainingTime = FULL_MOVEMENT_TIME;
    indexRemainingTime = FULL_MOVEMENT_TIME;
    middleRemainingTime = FULL_MOVEMENT_TIME;
    ringRemainingTime = FULL_MOVEMENT_TIME;
    pinkyRemainingTime = FULL_MOVEMENT_TIME;

    if (debugMode) Serial.println("All servos detached (no signal)");
}
