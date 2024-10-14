#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>   // Header file for standard library functions, including atexit()
#include <sys/ioctl.h>
#include <termios.h>  // Header file for terminal I/O interfaces
#include <unistd.h>   // Header file for POSIX operating system API

/*** definitions ***/
#define CTRL_KEY(k) ((k) & 0x1f) // Macro to define CTRL key combinations

/*** data ***/
// Global variable to hold the original terminal attributes
struct editorConfig {
    int screenrows;        // Number of rows in the terminal
    int screencols;        // Number of columns in the terminal
    struct termios orig_termios; // Struct to store original terminal attributes
};

struct editorConfig E; // Global instance of editorConfig

/*** terminal ***/
// Function to handle fatal errors
void die(const char *s) {
    write(STDOUT_FILENO, "\x1b[2J", 4); // Clear the screen
    write(STDOUT_FILENO, "\x1b[H", 3);  // Reposition the cursor to the top-left
    
    perror(s); // Print the error message
    exit(1);   // Exit the program
}

// Function to disable raw mode and restore original terminal settings
void disableRawMode() {
    // Restore the original terminal attributes
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
        die("tcsetattr"); // Error handling
}

// Function to enable raw mode for the terminal
void enableRawMode() {
    // Get the current terminal attributes and store them in 'orig_termios'
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) 
        die("tcgetattr");

    // Register the disableRawMode function to be called at program exit
    atexit(disableRawMode);

    // Create a copy of the original terminal attributes to modify
    struct termios raw = E.orig_termios;

    // Modify input flags to disable certain processing features
    raw.c_iflag &= ~(ICRNL | IXON | BRKINT | INPCK | ISTRIP);
    
    // Disable output post-processing
    raw.c_oflag &= ~(OPOST);
    
    // Set character size to 8 bits
    raw.c_cflag |= (CS8);
    
    // Modify the 'c_lflag' field to turn off ECHO and canonical mode
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);  
    /* ECHO: disable echoing of input characters
       ICANON: disable canonical mode (read input byte-by-byte)
       ISIG: disable signals for interrupts (Ctrl+C) and suspensions (Ctrl+Z)
       IEXTEN: disable extended input processing (Ctrl+V) */

    // Set minimum number of characters to read and timeout
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    // Set the modified terminal attributes back to the terminal
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) 
        die("tcsetattr");
}

/**
 * Reads a single keypress from the terminal in raw mode.
 * This function will wait until a key is pressed and return the character.
 * If an error occurs while reading, it will call die() to handle the error.
 * 
 * return: The character read from the terminal.
 */
char editorReadKey() {
    int nread;
    char c;
    // Loop until a single character is read
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) 
            die("read"); // Error handling
    }
    return c; // Return the character read
}

/**
 * Retrieves the size of the terminal window (rows and columns).
 * 
 * Parameters:
 * - int *rows: Pointer to store the number of rows.
 * - int *cols: Pointer to store the number of columns.
 * 
 * return: 0 on success, -1 on failure.
 */
int getWindowSize(int *rows, int *cols) {
    struct winsize ws; // Struct to hold window size information
    
    // Try to get the window size using ioctl
    if (1 || ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        // Move the cursor to the bottom-right corner if ioctl fails
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)  
            return -1; // Error if write fails

        editorReadKey(); // Wait for a keypress to observe results
        return -1; // Return failure
    } else {
        // Set the output parameters
        *cols = ws.ws_col; // Get number of columns
        *rows = ws.ws_row; // Get number of rows
        return 0; // Return success
    }
}

/*** output ***/
/**
 * Draws the editor interface by outputting tildes (~) for the number of rows defined.
 */
void editorDrawRows() {
    int y;
    for (y = 0; y < E.screenrows; y++) { // Loop for each row
        write(STDOUT_FILENO, "~\r\n", 3); // Print a tilde followed by a newline
    }
}

/**
 * Refreshes the screen by clearing it and redrawing the editor interface.
 */
void editorRefreshScreen() {
    write(STDOUT_FILENO, "\x1b[2J", 4); // Clear the screen
    write(STDOUT_FILENO, "\x1b[H", 3);  // Reposition the cursor to the top-left
    
    editorDrawRows(); // Draw the editor rows

    write(STDOUT_FILENO, "\x1b[H", 3); // Reposition the cursor again
}

/*** input ***/
/**
 * Processes a keypress from the user.
 * This function calls editorReadKey() to get the key and checks if it matches 
 * a specific command (e.g., quitting the editor with Ctrl-Q).
 */
void editorProcessKeyPress() {
    char c = editorReadKey(); // Read a keypress
    switch (c) {
        case CTRL_KEY('q'): // Check for Ctrl-Q to quit
            write(STDOUT_FILENO, "\x1b[2J", 4); // Clear the screen
            write(STDOUT_FILENO, "\x1b[H", 3);  // Reposition the cursor
            exit(0); // Exit the program
            break;
    }
}

/*** main + initialization ***/
/**
 * Initializes the editor by getting the terminal window size.
 */
void initEditor() {
    if (getWindowSize(&E.screenrows, &E.screencols) == -1) 
        die("getWindowSize"); // Error handling
}

/**
 * Main function to set up the editor and enter the main loop.
 */
int main() {
    enableRawMode();  // Enable raw mode and disable ECHO
    initEditor();     // Initialize the editor with terminal size
    
    // Main loop to refresh the screen and process key presses
    while (1) {
        editorRefreshScreen(); // Refresh the screen
        editorProcessKeyPress(); // Process user input
    }

    return 0;  // Exit the program
}
