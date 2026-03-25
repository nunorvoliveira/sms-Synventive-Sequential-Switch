 /* SYNVENTIVE SEQUENTIAL SWITCH

  This sketch reads the inputs from a rotary switch
  and open and close buttons and turns on and off
  the corresponding mosfets.

  In addition, there are some leds that display the
  status the program is currently in.

  When the device turns on, if the Open button is
  pressed, the buzzer usage for the Open mosfet is
  activated.

  The device starts working only when the rotary
  switch is in the off position and both buttons
  are depressed.

*/

// Pin definitions
constexpr int LED_OFF           = 2;
constexpr int LED_MAN           = 3;
constexpr int LED_05            = 4;
constexpr int LED_10            = 5;
constexpr int LED_20            = 6;
constexpr int LED_35            = 7;
constexpr int LED_50            = 8;
constexpr int LED_95            = 9;
constexpr int BUZZER            = 10;
constexpr int SW_OPEN           = 11;
constexpr int SW_CLOSE          = 12;
constexpr int ROTARY            = A0;
constexpr int MF_CLOSE          = A1;
constexpr int MF_OPEN           = A2;

// Variables
int           LastState         = 0;     // last program state
int           CurrState         = 0;     // new program state
bool          StateChanged      = true;  // Sets current state as changed to force initial update
unsigned long StartTime         = 0;     // time new state started
unsigned long CurrTime          = 0;     // duration of current state
int           CurrDelay         = 0;     // Duration of current alternating rate
bool          ActiveBuzzer      = false; // Defines the default usage for the buzzer as off
bool          OpenMosfet        = false; // Set mosfet desired state to off
bool          CloseMosfet       = false; // Set mosfet desired state to off

// States
constexpr int STATE_OFF         = 1;
constexpr int STATE_MAN         = 2;
constexpr int STATE_05          = 3;
constexpr int STATE_10          = 4;
constexpr int STATE_20          = 5;
constexpr int STATE_35          = 6;
constexpr int STATE_50          = 7;
constexpr int STATE_95          = 8;

// Times
constexpr int TIME_OFF          = 0;
constexpr int TIME_05           = 500;
constexpr int TIME_10           = 1000;
constexpr int TIME_20           = 2000;
constexpr int TIME_35           = 3500;
constexpr int TIME_50           = 5000;
constexpr int TIME_95           = 9500;

void setup() {
  
  // Pin initializations
  pinMode(LED_OFF,              OUTPUT);
  pinMode(LED_MAN,              OUTPUT);
  pinMode(LED_05,               OUTPUT);
  pinMode(LED_10,               OUTPUT);
  pinMode(LED_20,               OUTPUT);
  pinMode(LED_35,               OUTPUT);
  pinMode(LED_50,               OUTPUT);
  pinMode(LED_95,               OUTPUT);
  pinMode(SW_OPEN,              INPUT_PULLUP);
  pinMode(SW_CLOSE,             INPUT_PULLUP);
  pinMode(BUZZER,               OUTPUT);
  pinMode(MF_OPEN,              OUTPUT);
  pinMode(MF_CLOSE,             OUTPUT);
  pinMode(ROTARY,               INPUT);

  // Pin default states
  digitalWrite(LED_OFF,         LOW);
  digitalWrite(LED_MAN,         LOW);
  digitalWrite(LED_05,          LOW);
  digitalWrite(LED_10,          LOW);
  digitalWrite(LED_20,          LOW);
  digitalWrite(LED_35,          LOW);
  digitalWrite(LED_50,          LOW);
  digitalWrite(LED_95,          LOW);
  digitalWrite(BUZZER,          LOW);
  digitalWrite(MF_OPEN,         LOW);
  digitalWrite(MF_CLOSE,        LOW);

  // Reads buzzer activation
  if ( digitalRead(SW_OPEN) == LOW ) {
    ActiveBuzzer = true;
  }
  // Wait for state to be OFF and both buttons depressed
  do {
    CurrState = ReadNewState();
  } while ( CurrState != STATE_OFF || digitalRead(SW_OPEN) == LOW || digitalRead(SW_CLOSE) == LOW);
  // Set both new and last state as OFF
  CurrState  = STATE_OFF;
  LastState = STATE_OFF;
  // Force update
  StateChanged = true;
  
}

void loop() {
  
  // Read new state and check for changes
  CurrState = ReadNewState();
  if ( CurrState != LastState ) {
    // Force update
    StateChanged = true;
    // Update time
    StartTime = millis();
    // Update state
    LastState = CurrState;
  }

  // Process current state leds and define delay if needed
  if ( StateChanged == true ) {
    // Turn all LEDs off
    TurnStateLEDsOff();
    // Turn on correct LED and update new delay
    switch (CurrState) {
      case STATE_OFF:
        digitalWrite(LED_OFF,  HIGH);
        CurrDelay = TIME_OFF;
        break;
      case STATE_MAN:
        digitalWrite(LED_MAN,  HIGH);
        CurrDelay = TIME_OFF;
        break;
      case STATE_05:
        digitalWrite(LED_05,   HIGH);
        CurrDelay = TIME_05;
        break;
      case STATE_10:
        digitalWrite(LED_10,   HIGH);
        CurrDelay = TIME_10;
        break;
      case STATE_20:
        digitalWrite(LED_20,   HIGH);
        CurrDelay = TIME_20;
        break;
      case STATE_35:
        digitalWrite(LED_35,   HIGH);
        CurrDelay = TIME_35;
        break;
      case STATE_50:
        digitalWrite(LED_50,   HIGH);
        CurrDelay = TIME_50;
        break;
      case STATE_95:
        digitalWrite(LED_95,   HIGH);
        CurrDelay = TIME_95;
        break;
    }
    StateChanged = false;
  }

  // Process current state and time open/close desired output
  if ( true ) {
    switch (CurrState) {
      case STATE_OFF:
        // Off
        OpenMosfet = false;
        CloseMosfet = false;
        break;
      case STATE_MAN:
        // Manual. Read buttons
        OpenMosfet = !digitalRead(SW_OPEN);
        CloseMosfet = !digitalRead(SW_CLOSE);
        break;
      case STATE_05:
      case STATE_10:
      case STATE_20:
      case STATE_35:
      case STATE_50:
      case STATE_95:
        // Calculate current time in this state
        CurrTime = millis() - StartTime;
        // Calculate if mosfet should open or close
        if  ( ( 1 + ( CurrTime / CurrDelay ) % 2 ) == 1 ) {
          // Open
          OpenMosfet = true;
          CloseMosfet = false;
        } else {
          // Close
          OpenMosfet = false;
          CloseMosfet = true;
        }
      break;
    }
    // Check for manual override if not in OFF state
    if ( CurrState != STATE_OFF ) {
      if ( !digitalRead(SW_OPEN) == HIGH ) {
        // Open is pressed
        OpenMosfet = true;
      }
      if ( !digitalRead(SW_CLOSE) == HIGH ) {
        // Close is pressed
        CloseMosfet = true;
      }
    }
    // Check for duplicated instructions and fix if needed
    if ( OpenMosfet = true && CloseMosfet == true ) {
      // Both buttons pressed, default to close
      OpenMosfet = false;
    }
  }

  // Update mosfets status starting by turning off first
  if ( true ) {
    if ( OpenMosfet == false ) {
      // Turn off open mosfet
      digitalWrite(MF_OPEN, LOW);
      // Turn off buzzer
      digitalWrite(BUZZER, LOW);
    }
    if ( CloseMosfet == false ) {
      // Turn off close mosfet
      digitalWrite(MF_CLOSE, LOW);
    }
    if ( OpenMosfet == true ) {
      // Turn on open mosfet
      digitalWrite(MF_OPEN, HIGH);
      // Turn on buzzer
      if ( ActiveBuzzer = true ) {
        digitalWrite(BUZZER, HIGH);
      }
    }
    if ( CloseMosfet == true ) {
      // Turn off close mosfet
      digitalWrite(MF_CLOSE, HIGH);
    }
  }

}

void TurnStateLEDsOff() {
  // Turn off all state LEDs
  digitalWrite(LED_OFF,  LOW);
  digitalWrite(LED_MAN,  LOW);
  digitalWrite(LED_05,   LOW);
  digitalWrite(LED_10,   LOW);
  digitalWrite(LED_20,   LOW);
  digitalWrite(LED_35,   LOW);
  digitalWrite(LED_50,   LOW);
  digitalWrite(LED_95,   LOW);
}

int ReadNewState() {
  int SelectedMode;
  // Read analog pin
  SelectedMode = analogRead(ROTARY);
  // Process it by shifting half value and divide by interval.
  // Returns 1 from 64 to 191
  // Returns 2 from 192 to 319
  // Returns 3 from 320 to 447
  // Returns 4 from 448 to 575
  // Returns 5 from 576 to 703
  // Returns 6 from 704 to 831
  // Returns 7 from 832 to 959
  // Returns 8 from 960 to 1087 // Stops at 1023...
  SelectedMode = ( SelectedMode + 64 ) / 128;
  // Return current state
  return SelectedMode;
}

/*
  RESISTOR LADDER FOR ROTARY SWITCH
  5v - 4.7k - 4.7k - 4.7k - 4.7k - 4.7k - 4.7k - 4.7k - 4.7k - GND
*/