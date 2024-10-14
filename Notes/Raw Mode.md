Raw Mode
========

Overview
--------

-   **Raw Mode**: In this mode, keyboard input is processed immediately, allowing for a more interactive user interface.
-   **Canonical Mode (Cooked Mode)**: Default mode where input is sent to the program only after pressing Enter.

Key Functions and Structures
----------------------------

-   **`read()`**: Reads bytes from standard input. Returns the number of bytes read (1 for successful reads).
-   **`STDIN_FILENO`**: Constant representing the standard input file descriptor.
-   **`struct termios`**: Structure used to configure terminal attributes.
-   **`tcgetattr()` and `tcsetattr()`**: Functions to get and set terminal attributes.
-   **`atexit()`**: Registers a function to be called on program exit.

Terminal Attributes
-------------------

-   **Local Flags (`c_lflag`)**: Includes flags like `ECHO` and `ICANON`.
    -   **`ECHO`**: Controls whether typed characters are displayed.
    -   **`ICANON`**: Enables canonical mode; disable to enter raw mode.

Implementing Raw Mode
---------------------

1.  **Save Original Attributes**: Use `tcgetattr()` to save the current terminal settings.
2.  **Modify Attributes**:
    -   Turn off `ECHO`: `raw.c_lflag &= ~(ECHO)`.
    -   Turn off `ICANON`: `raw.c_lflag &= ~(ICANON)`.
3.  **Set Modified Attributes**: Use `tcsetattr()` to apply changes.

Exiting and Cleaning Up
-----------------------

-   Use `atexit()` to ensure terminal attributes are restored upon program exit via a registered cleanup function.

Reading Input
-------------

-   Loop with `read()` to capture input character-by-character until a specific key (like 'q') is pressed.
-   Print the ASCII value and character representation of the input, distinguishing control characters using `iscntrl()`.

### Key Observations

-   Certain keys generate escape sequences (e.g., arrow keys, Page Up/Down).
-   Control key combinations map to ASCII codes (e.g., Ctrl-A = 1, Ctrl-B = 2).
-   Special keys (like Ctrl-S) may pause output, requiring Ctrl-Q to resume.

Handling Terminal States
------------------------

-   To reset the terminal state after the program exits, a command like `reset` may be needed, or simply restarting the terminal.

Turn Off Ctrl-C and Ctrl-Z Signals
----------------------------------

-   By default, Ctrl-C sends a `SIGINT` signal, terminating the current process, while Ctrl-Z sends a `SIGTSTP` signal, suspending it. To read these signals instead, we disable them by modifying the terminal attributes.

### Implementation

-   **ISIG**: A flag in `struct termios` that controls signal generation from special characters. By clearing this flag, Ctrl-C and Ctrl-Z can be captured as normal input bytes.

Disable Ctrl-S and Ctrl-Q
-------------------------

-   Ctrl-S and Ctrl-Q are used for software flow control, pausing and resuming data transmission to the terminal. We can disable this feature to avoid interference with input processing.

### Implementation

-   **IXON**: An input flag that allows flow control using Ctrl-S and Ctrl-Q. Clearing this flag allows these combinations to be read as regular input.

Disable Ctrl-V
--------------

-   On some systems, Ctrl-V allows users to input the next character literally. Disabling this feature ensures that Ctrl-V behaves as regular input.

### Implementation

-   **IEXTEN**: A flag in `struct termios` that controls the behavior of special input characters. Clearing this flag turns off the literal input feature.

Fix Ctrl-M
----------

-   Ctrl-M was reading as a newline (10) instead of a carriage return (13) due to terminal translation. To address this, we disable the automatic translation of carriage returns to newlines.

### Implementation

-   **ICRNL**: An input flag that controls the conversion of carriage returns to newlines. By clearing this flag, Ctrl-M is read correctly as 13.

Turn Off All Output Processing
------------------------------

-   The terminal translates newlines into carriage return and newline sequences, which may not be desired in raw mode. To avoid this, we disable output processing.

### Implementation

-   **OPOST**: An output flag responsible for processing output. Clearing this flag disables the automatic translation of newlines.

Add Carriage Returns to Output
------------------------------

-   Since we have disabled output processing, newlines will only move the cursor down without returning to the beginning of the line. To handle this properly, we need to manually add carriage returns to our output.

### Implementation

In `printf()` statements, we include `\r\n` to ensure the cursor moves to the start of the line and down to the next line appropriately.

Miscellaneous Flags
-------------------

We can turn off additional terminal flags that may not have observable effects but are part of enabling "raw mode".

### Key Concepts

-   **BRKINT**: When enabled, a break condition sends a SIGINT signal.
-   **INPCK**: Enables parity checking, not applicable to modern terminals.
-   **ISTRIP**: Strips the 8th bit of each input byte, usually turned off by default.
-   **CS8**: Sets the character size to 8 bits per byte.

A Timeout for `read()`
----------------------

Setting a timeout allows `read()` to return if there's no input after a specified period, enabling other operations like screen animations.

### Key Concepts

-   **VMIN**: Minimum number of input bytes needed before `read()` can return.
-   **VTIME**: Maximum time to wait for input, set in tenths of seconds.

Error Handling
--------------

Improving the robustness of our program by adding error handling ensures that issues are properly reported.

### Key Concepts

-   **`die()` Function**: Prints an error message using `perror()` and exits the program.
-   **Error Checking**: Each library call is checked for failure, and `die()` is called if an error occurs.

### Implementation Steps

-   Use `perror()` to display error messages related to system calls.
-   Check the return values of `tcgetattr()`, `tcsetattr()`, and `read()` for error handling, specifically handling the `EAGAIN` error in Cygwin.
