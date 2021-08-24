// Project Link: https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink

/// Macros
#define USE_ONBOARD_LED false

/// Globals
int ledPin = 12;

/// Our Routines
void blinkLed(int pin)
{
  digitalWrite(pin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second aka do useless work to occupy the processor
  digitalWrite(pin, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);               // wait for a second aka do useless work to occupy the processor

  return;
}

/// Arduino IDE Default Routines 
void setup(void)
{
	//NOTE(): Configures GPIO(General Purpose Input Output) pin
	pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
	//NOTE(): make sure computer your connected to
	//        uses the same baud rate(9600).
	Serial.begin(9600);
}

void loop(void)
{
	// put your main code here, to run repeatedly:
	Serial.println("blinking...");
  #if USE_ONBOARD_LED
  blinkLed(LED_BUILTIN);
  #else
  blinkLed(ledPin);
  #endif
}
