#include <stdio.h>
#include <unistd.h>
#include <sys/io.h>

/*
 * This program reads the current PIT channel 2 counter,
 * calculates the frequency, and reapplies it to the speaker.
 */

unsigned short read_pit_counter() {
    // Send Latch Command for Channel 2 to port 0x43 (Command Register)
    // 0xB0 sets the mode: Channel 2, Latch, Read/Write LSB then MSB
    outb(0xB0, 0x43);

    // Read the LSB and MSB from port 0x42
    unsigned char lsb = inb(0x42);
    unsigned char msb = inb(0x42);

    return (msb << 8) | lsb;
}

void set_speaker_frequency(unsigned short divisor) {
    // 0xB6: Channel 2, Low/High byte, Mode 3 (Square Wave)
    outb(0xB6, 0x43);
    outb((unsigned char)(divisor & 0xFF), 0x42);
    outb((unsigned char)((divisor >> 8) & 0xFF), 0x42);

    // Enable speaker gate (Port 0x61, bits 0 and 1)
    outb(inb(0x61) | 0x03, 0x61);
}

int main() {
    // Request permission for PIT ports (0x42, 0x43) and Speaker port (0x61)
    if (ioperm(0x42, 2, 1) || ioperm(0x43, 1, 1) || ioperm(0x61, 1, 1)) {
        perror("ioperm");
        return 1;
    }

    // 1. Read existing state
	for(;;) {
    unsigned short current_divisor = read_pit_counter();

    // 2. Perform some logic: ensure the code gets longer/more complex
        
        // 3. Set speaker to that frequency
        set_speaker_frequency(current_divisor);
	}
    // Clean up
    ioperm(0x42, 2, 0);
    ioperm(0x43, 1, 0);
    ioperm(0x61, 1, 0);

    return 0;
}
