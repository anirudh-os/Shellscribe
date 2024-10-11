#include<ctype.h>
#include<stdio.h>
#include<errno.h>
#include <stdlib.h>   // Header file for standard library functions, including atexit()
#include <termios.h>  // Header file for terminal I/O interfaces
#include <unistd.h>   // Header file for POSIX operating system API

// Global variable to hold the original terminal attributes
struct termios orig_termios;

void die(const char *s) {
    perror(s);
    exit(1);
}

// Function to disable raw mode and restore original terminal settings
void disableRawMode() {
    // Restore the original terminal attributes
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
	    die("tcsetattr");
}

// Function to enable raw mode for the terminal
void enableRawMode() {
    // Get the current terminal attributes and store them in 'orig_termios'
    if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");

    // Register the disableRawMode function to be called at program exit
    atexit(disableRawMode);

    // Create a copy of the original terminal attributes to modify
    struct termios raw = orig_termios;

    // ICRNL I stands for "input flag", CR stands for "carriage return", and NL stands for "new line"
    // Modify the c_iflag to turn off XON and XOFF (ctrl+s and ctrl+q)
    raw.c_iflag &= ~(ICRNL | IXON | BRKINT | INPCK | ISTRIP);
    
    // OPOST means "output post processing"
    raw.c_oflag &= ~(OPOST);
    
    raw.c_cflag |= (CS8);
    
    // Modify the 'c_lflag' field to turn off the ECHO feature
    // ECHO flag is used to control whether input characters are printed to the terminal
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);  
    /* Bitwise-NOT ECHO and AND with current flags to disable it
     ICANON flag allows us to turn off canonical mode. This means we will finally be reading 
     input byte-by-byte, instead of line-by-line
     ISIG flag can be used to turn off the terminate and suspend signals (ctrl+c and ctrl+z)
     IEXTEN flag can be used to turn off the feature (ctrl+v) to send a group of characters together (literally)
     */

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    // VMIN and VTIME are the indices of 'c_CC' field.

    // Set the modified terminal attributes back to the terminal
    // TCSAFLUSH waits for all pending output to be written and discards unread input
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

int main() {
    enableRawMode();  // Call function to enable raw mode and disable ECHO
    
    while (1) {
	    char c = '\0';
	    if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
	    if (iscntrl(c)) {
		    printf("%d\r\n", c);
	    } else {
		    printf("%d ('%c')\r\n", c, c);
	    }
	    if (c == 'q') break;
    }

    return 0;  // Exit the program
}

