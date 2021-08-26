#include <avr/io.h>
#include <avr/boot.h>
#include <stdint.h>
#include <stdio.h>
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

struct __FILE
{
    int handle;
};

FILE __stdout;  //Use with printf
FILE __stdin ;  //use with fget/sscanf, or scanfb

#define USE_ONBOARD_LED false
#define SYSTEM_OSC_FREQUENCY 8000000UL

#define LED_BUILTIN  PINB5 // (Port B PIN 5)

#ifdef DEBUG_BUILD
#define DEBUG(statement) statement
#else
#define DEBUG(statement)
#endif

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

void LED_Blink(u16 pin, u16 millis)
{
    PORTB |=  getBitValue(pin); //digitalWrite(pin, HIGH); // turn the LED on (HIGH is the voltage level)
    delay(millis);              //delay(1000);             // wait for a second aka do useless work to occupy the processor
    PORTB &= ~getBitValue(pin); //digitalWrite(pin, LOW);  // turn the LED off by making the voltage LOW
    delay(millis);              //delay(1000);             // wait for a second aka do useless work to occupy the processor
    
    return;
}

void LED_BlinkNumberBinary(u16 value, u16 data_size_in_bits)
{
    /// 3 flashes to indicate a print
    LED_Blink(ledPin, 1000);
    LED_Blink(ledPin, 1000);
    LED_Blink(ledPin, 1000);
    delay(20000);
    
    /// blink bits left to right (1: ledPin | 0: LED_BUILTIN)
    for(u16 bit = 0; bit < data_size_in_bits; bit++)
    {
        LED_Blink(((value >> (data_size_in_bits - 1)) & 0x1)? ledPin:LED_BUILTIN
                  , 2000);
        
        value = (value << 1);
    }
    
    /// 3 flashes to indicate print has finished
    delay(20000);
    LED_Blink(ledPin, 1000);
    LED_Blink(ledPin, 1000);
    LED_Blink(ledPin, 1000);
    delay(2000);
    delay(2000);
    
    return;
}

#define USBS0_MASK    (1 << USBS0)
#define RXEN0_MASK    (1 << RXEN0)
#define TXEN0_MASK    (1 << TXEN0)
#define UCSZ00_SET(x) (x << UCSZ00)

void
USART0_init(u16 baud_rate)
{
    // TODO(MIGUEL): set baud rate
    // TODO(MIGUEL): set frame format
    // TODO(MIGUEL): enable transmitter
    // TODO(MIGUEL): clear global interrupt flag
    
    // NOTE(MIGUEL): pg.146 calc buad value for desired baud rate
    //u16 baud = (SYSTEM_OSC_FREQUENCY / (16 * baud_rate)) - 1;
    u32 baud = ((SYSTEM_OSC_FREQUENCY / (4 * baud_rate)) - 1) / 2;
    
    // 0000 1111    1111 1111
    UBRR0H = (u8)((baud & 0x0F00) >> 8);
    UBRR0L = (u8) (baud & 0x00FF);
    
    /// Enable transmitter and reciever
    UCSR0B = RXEN0_MASK | TXEN0_MASK;
    
    /// Sef frame format
    UCSR0C =   UCSZ00_SET(3);
    UCSR0C &= ~USBS0_MASK;
    
    return;
}

void
USART0_transmit(u8 data)
{
    while(!(UCSR0A & (1 << UDRE0)));
    
    UDR0 = data;
    
    return;
}

u8
USART0_receive(void)
{
    u8 data;
    
    while(!(UCSR0A & (1 << RXC0)));
    
    data = UDR0;
    
    return data;
}

//Retarget the fputc method to use the USART0
s16 
fputc(s16 byte, FILE *f)
{
    USART0_transmit(byte);
    
    return byte;
}

//Retarget the fgetc method to use the USART0
s16 
fgetc(FILE *f)
{
    s16 data = 0;
    
    data = USART0_receive();
    
    return data & 0xFF;
}

int main(void)
{
    /// setup
    // TODO(MIGUEL): configure UART for serial comm
    USART0_init(9600);
    // TODO(MIGUEL): configure ADC to read lm35
    
    
    DDRB |= getBitValue(LED_BUILTIN); // or (1 << PINB5) or 0x08
    DDRB |= getBitValue(ledPin);      // or (1 << PINB4) or 0x10
    
    /// loop
    while(true)
    {
        // TODO(MIGUEL): transmit status to host pc via serial comm (UART)
        printf("blinking...");
        u8 efuse = boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS);
        u8 hfuse = boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);
        u8 lfuse = boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);
        delay(64000);
        LED_BlinkNumberBinary(lfuse, 8);
        delay(10000);
        LED_BlinkNumberBinary(hfuse, 8);
        delay(10000);
        LED_BlinkNumberBinary(efuse, 8);
        delay(10000);
        
        delay(64000);
        
        
#if USE_ONBOARD_LED
        //LED_blink(LED_BUILTIN);
#else
        //LED_blink(ledPin);
#endif
    }
}
