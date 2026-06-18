#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/kd.h>

#define CONSOLE_DEVICE "/dev/tty0"

// Function to play a raw frequency value for a given duration
void play_tone(int fd, int frequency, int duration_us) {
    if (frequency <= 0) {
        ioctl(fd, KIOCSOUND, 0); // Silence
    } else {
        // Direct calculation: PIT clock (1193182 Hz) / desired frequency
        int clock_ticks = 1193182 / frequency;
        ioctl(fd, KIOCSOUND, clock_ticks);
    }
    usleep(duration_us);
}

// Process raw bytes without filtering or altering the values
void play_raw_bytes(int fd, const unsigned char *bytes, size_t length) {
    for (size_t i = 0; i < length; i++) {
        // Use the raw byte value directly as the frequency (Hz)
        // e.g., 'A' (65) -> 65Hz, 'z' (122) -> 122Hz
        int freq = (int)bytes[i];
        
        // Multiply by a factor if you need it audible, 
        // but for completely unfiltered 1:1 direct passing:

        play_tone(fd, freq, 10); // 100ms per byte
      
    }
}

int main(int argc, char *argv[]) {
    int fd = open(CONSOLE_DEVICE, O_WRONLY);
    if (fd == -1) {
        fd = open("/dev/console", O_WRONLY);
        if (fd == -1) {
            perror("Error: Missing root privileges or console access");
            return 1;
        }
    }

    // Scenario 1: Command line arguments passed directly
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            play_raw_bytes(fd, (unsigned char *)argv[i], strlen(argv[i]));
            
            // Optional: play a space character raw if separating multiple arguments
            if (i < argc - 1) {
                unsigned char space = ' ';
                play_raw_bytes(fd, &space, 1);
            }
        }
    } 
    // Scenario 2: Pipe / Stdin passed directly
    else {
        unsigned char buffer[1024];
        ssize_t bytes_read;
        
        // read raw chunks to preserve exact binary transmission (no fgets newline filtering)
        while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
            play_raw_bytes(fd, buffer, bytes_read);
        }
    }

    // Shut down speaker safely on exit
    ioctl(fd, KIOCSOUND, 0);
    close(fd);
    
    return 0;
}
