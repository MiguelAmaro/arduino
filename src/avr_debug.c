#include <avr/io.h>

void Debug_ASMToggle(void)
{
    __asm__
        (
         "out 0x10, r3"
         :
         :"r"(PORTB)
         :"r3"
         );
    
    return;
}

void Debug_LEDFlashFuseBits(void)
{
    // NOTE(MIGUEL): lfuse = 0xFF
    //               hfuse = 0xDE
    //               efese = 0xFD
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
    
    return;
}

void Debug_ToggleIOTOMeasureSysFreq(void)
{
    // NOTE(MIGUEL): measuring stratagy is to toggle an io pin repeatedlly.
    //               The instruction to do that should take only one cycle.
    //               Check the resulting assembly code using using godbolt
    //               or some compiler option that'll tranplile ur code to assembly.
    //               Once it looks good. Connect some jumper cables to the IO port u
    //               chose and ground and probe them with the O-scope. Edge trigger on
    //               normal should be sufficient and should give you a reading of the 
    //               frequency. The actually frequency is acturally double that number!
    //               
    //               These lines in C should compile to-
    //               PINB = 0x10;
    //               PINB = 0x10;
    //               PINB = 0x10;
    //               PINB = 0x10;
    //               
    //               this in assembly
    //               out 0x3,r25
    //               out 0x3,r25
    //               out 0x3,r25
    //               out 0x3,r25
    while(1)
    {
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
        PINB = 0x10;
    }
    return;
}