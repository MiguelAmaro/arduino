#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#define true  1
#define false 0

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef float    f32;
typedef float    f64;


#define USE_ONBOARD_LED true

#define LED_BUILTIN  PINB5 // (Port B PIN 5)


/// Globals
u16 ledPin = PINB4; // (PORT B | PIN 4) aka D12


/// Our Routines
u16 getBitValue(u16 mask)
{
    return _BV(mask);
}

void delay(u16 millis)
{
    for(u16 current_milli = 0;
        current_milli < millis; current_milli++)
    {
        _delay_ms(1); // NOTE(MIGUEL): cant pass millis directly in
    }
    
    return;
}

void blinkLed(u16 pin)
{
    PORTB |=  getBitValue(pin); //digitalWrite(pin, HIGH); // turn the LED on (HIGH is the voltage level)
    delay(1000);                //delay(1000);             // wait for a second aka do useless work to occupy the processor
    PORTB &= ~getBitValue(pin); //digitalWrite(pin, LOW);  // turn the LED off by making the voltage LOW
    delay(1000);                //delay(1000);             // wait for a second aka do useless work to occupy the processor
    
	return;
}


int main(void)
{
    /// setup
    // TODO(MIGUEL): configure UART for serial comm
    DDRB |= getBitValue(LED_BUILTIN); // or (1 << PINB5) or 0x08
    DDRB |= getBitValue(ledPin);      // or (1 << PINB4) or 0x10
    
    /// loop
    while(true)
    {
        // TODO(MIGUEL): transmit status to host pc via serial comm (UART)
        
#if USE_ONBOARD_LED
        blinkLed(LED_BUILTIN);
#else
        blinkLed(ledPin);
#endif
    }
    
    return 0;
}
