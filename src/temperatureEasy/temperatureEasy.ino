/// Pg. 52 "Getting Started with Sensors Measure the World with Electronics, Arduino, and Raspberry Pi"

/// Globals
int heatSensorPin = A0;

/// Our Routines
float tempInCelcius(void)
{
	float result  = 0.0f;
	
	// Sensor Model: Texas Instruments lm35
	//NOTE(): The Analog To Digital Converter maps the voltage
	//        to a 10 bit integer. 1023.0f is the max value of
	//        a 10 bit integer 2^10.
	// 5+ volts = 100% = 1023.0f
	// ... 
	// 0  volts =   0% =    0.0f
	
	float raw     = analogRead(heatSensorPin);
	float percent = raw / 1023.0f;
	float volts   = percent * 5.0f;
	
	result = 100.0f * volts;
	
	return result;
}


/// Arduino IDE Default Routines 
void setup(void)
{
	// put your setup code here, to run once:
	
	//NOTE(): make sure computer your connected to
	//        uses the same baud rate(9600).
	Serial.begin(9600);  
}

void loop(void)
{
	// put your main code here, to run repeatedly:
	Serial.println(tempInCelcius());
	delay(200);
}
