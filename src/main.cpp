#include <Arduino.h>
#include <ESP8266WiFi.h>

// input
#define POWER_SWITCH  D2
#define RESET_BUTTON  D7
#define RPI_OFF_PIN   D6

// output
#define RELAY_PIN         D1
#define RPI_SHUTDOWN_PIN  D0
#define RPI_REBOOT_PIN    D5
#define POWER_LED         D4

// timings
#define POWER_SWITCH_DEBOUNCE 2000
#define RESET_BUTTON_DEBOUNCE 500
#define POWER_OFF_DELAY       5000
#define POWER_ON_DELAY        7000


typedef enum power_state
{
  initialPowerOn,
  rPiOnDelay,
  rPiStartup,
  rPiRunning,
  rPiReboot,
  rPiShutdown,
  rPiOffDelay,
  rPiOff
}
PowerState;

PowerState powerState;

time_t ledTimestamp;
time_t powerOffDelayTimestamp;
time_t powerOnDelayTimestamp;
time_t currentTimestamp;
time_t powerSwitchDebounceTimestamp;
time_t resetButtonDebounceTimestamp;

void setup()
{
  Serial.begin(74880);

  // input pins
  pinMode( POWER_SWITCH, INPUT_PULLUP );
  pinMode( RESET_BUTTON, INPUT_PULLUP );
  pinMode( RPI_OFF_PIN, INPUT_PULLUP );

  // output pins
  pinMode( RELAY_PIN, OUTPUT );
  pinMode( RPI_SHUTDOWN_PIN, INPUT_PULLUP ); // initial as input pin
  pinMode( RPI_REBOOT_PIN, INPUT_PULLUP ); // initial as input pin
  pinMode( POWER_LED, OUTPUT );

  // initial pin setup
  digitalWrite( RELAY_PIN, LOW );
  digitalWrite( POWER_LED, HIGH );

  //
  powerState = rPiOff;
  ledTimestamp = 0;
  powerOffDelayTimestamp = 0;

  // disable WiFi
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.setOutputPower(0.0f);

  // wait for serial monitor
  delay(3000);
  Serial.println("\n\n");
  Serial.println("\n\n");
  Serial.println( "ESP8266 RaspberryPi PowerHandler for Sony Playstation 1 case." );
  Serial.println( "Version 0.3.3 by Dr. Thorsten Ludewig <t.ludewig@gmail.com>" );
  Serial.println( "Build date: " __DATE__ " " __TIME__  );
}

void blinkPowerLED()
{
  if (( currentTimestamp - ledTimestamp ) > 500 )
  {
    digitalWrite( POWER_LED, !digitalRead(POWER_LED));
    ledTimestamp = currentTimestamp;
  }
}

void printSwitchState( const char* state )
{
  Serial.printf( "(%lu) Switch to state: %s.\n", currentTimestamp, state );
}

void loop()
{
  currentTimestamp = millis();

  switch( powerState )
  {

  case initialPowerOn:

    if ( digitalRead(POWER_SWITCH) == LOW && ( currentTimestamp - powerSwitchDebounceTimestamp) > POWER_SWITCH_DEBOUNCE )
    {
      printSwitchState("rPiOnDelay");
      pinMode( RPI_SHUTDOWN_PIN, INPUT_PULLUP ); // initial as input pin
      pinMode( RPI_REBOOT_PIN, INPUT_PULLUP ); // initial as input pin
      powerState = rPiOnDelay;
      digitalWrite( RELAY_PIN, HIGH );
      powerSwitchDebounceTimestamp = currentTimestamp;
      powerOnDelayTimestamp = currentTimestamp;
    }

    break;

  case rPiOnDelay:
    blinkPowerLED();

    if (( currentTimestamp - powerOnDelayTimestamp ) > POWER_ON_DELAY )
    {
      printSwitchState("rPiStartup");
      pinMode( RPI_REBOOT_PIN, OUTPUT );
      pinMode( RPI_SHUTDOWN_PIN, OUTPUT );
      digitalWrite( RPI_REBOOT_PIN, HIGH );
      digitalWrite( RPI_SHUTDOWN_PIN, HIGH );
      powerState = rPiStartup;
    }

    break;

  case rPiStartup:
    blinkPowerLED();

    if ( digitalRead( RPI_OFF_PIN ) == LOW )
    {
      printSwitchState("rPiRunning");
      digitalWrite( POWER_LED, LOW );
      powerState = rPiRunning;
    }

    break;

  case rPiRunning:

    if( digitalRead( RESET_BUTTON ) == LOW && ( currentTimestamp - resetButtonDebounceTimestamp) > RESET_BUTTON_DEBOUNCE )
    {
      printSwitchState("rPiReboot");
      powerState = rPiReboot;
      digitalWrite( RPI_REBOOT_PIN, LOW );
      resetButtonDebounceTimestamp = currentTimestamp;
    }

    if( digitalRead( POWER_SWITCH ) == HIGH && ( currentTimestamp - powerSwitchDebounceTimestamp) > POWER_SWITCH_DEBOUNCE )
    {
      printSwitchState("rPiShutdown");
      powerState = rPiShutdown;
      digitalWrite( RPI_SHUTDOWN_PIN, LOW );
      powerSwitchDebounceTimestamp = currentTimestamp;
    }

    if ( digitalRead( RPI_OFF_PIN ) == HIGH )
    {
      printSwitchState("rPiOffDelay");
      powerState = rPiOffDelay;
      powerOffDelayTimestamp = currentTimestamp;
    }

    break;

  case rPiReboot:
    blinkPowerLED();

    if ( digitalRead( RPI_OFF_PIN ) == HIGH )
    {
      printSwitchState("rPiStartup");
      pinMode( RPI_REBOOT_PIN, INPUT_PULLUP ); // initial as input pin
      powerState = rPiStartup;
    }

    break;

  case rPiShutdown:
    blinkPowerLED();

    if ( digitalRead( RPI_OFF_PIN ) == HIGH )
    {
      printSwitchState("rPiOffDelay");
      pinMode( RPI_SHUTDOWN_PIN, INPUT_PULLUP ); // initial as input pin
      powerState = rPiOffDelay;
      powerOffDelayTimestamp = currentTimestamp;
    }

    break;

  case rPiOffDelay:
    blinkPowerLED();

    if ( digitalRead(POWER_SWITCH) == HIGH && ( currentTimestamp - powerOffDelayTimestamp ) > POWER_OFF_DELAY )
    {
      printSwitchState("rPiOff");
      powerState = rPiOff;
      powerSwitchDebounceTimestamp = currentTimestamp;
    }

    break;

  case rPiOff:
    digitalWrite( RELAY_PIN, LOW );
    pinMode( RPI_SHUTDOWN_PIN, INPUT_PULLUP ); // initial as input pin
    pinMode( RPI_REBOOT_PIN, INPUT_PULLUP ); // initial as input pin
    digitalWrite( POWER_LED, HIGH );
    printSwitchState("initialPowerOn");
    powerState = initialPowerOn;
    break;
  }

  yield();
}
