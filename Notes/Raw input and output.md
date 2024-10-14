# Raw Input and Output

-   **Control Key Mapping**: Ctrl key combinations correspond to bytes 1-26 in terminal input. For example, Ctrl-Q sends the command to quit.

-   **Character Encoding**: The CTRL_KEY macro strips specific bits from a character, mirroring the terminal's behavior of sending a modified version of the key pressed in combination with Ctrl.

-   **Bit Manipulation**: The upper 3 bits of a character are set to 0 when applying the CTRL_KEY macro.

## Refactor Keyboard Input

-----------------------

### Key Concepts

-   **Function Separation**: Low-level keypress reading and mapping to editor operations are handled by separate functions, improving code organization.

-   **Editor Read Key Function**: Waits for a single keypress and returns it, facilitating future expansions for handling escape sequences and special keys.

-   **Key Processing**: The editor processes keypresses in a dedicated function, which can be expanded to include additional key mappings.

## Clear the Screen

----------------

### Key Concepts

-   **Screen Clearing**: The screen can be cleared using an escape sequence, consisting of an escape character followed by formatting commands.

-   **Escape Sequences**: Used to control text formatting in the terminal, such as clearing the screen or moving the cursor. The sequence `\x1b[2J` clears the entire screen.

-   **Terminal Compatibility**: While direct escape sequences are widely supported, libraries like ncurses can provide greater compatibility across different terminal types by abstracting escape sequence management.

## Reposition the Screen

---------------------

### Key Concepts

-   **Repositioning**: The cursor must be repositioned to the start of the terminal.

-   **Escape Sequences**: `\x1b[H` takes two arguments, the row and column number on the screen. By default, it is 1 for both.

## Global State

**Structs:**

-   **`struct editorConfig`**: This struct holds the configuration for the editor, including terminal attributes and size. It uses another struct, `termios`, to manage terminal settings.

**Functions:**

-   **`die(const char *s)`**: A function intended for error handling. It typically prints an error message and exits the program.
-   **`disableRawMode()`**: Restores the terminal's original settings when the program exits.
-   **`enableRawMode()`**: Configures the terminal for raw input mode, which allows for direct input handling without buffering or interpretation.

**Libraries:**

-   `<termios.h>`: This library provides definitions for the terminal control interfaces. It contains the `termios` struct, which is used to configure terminal attributes.

## Getting Window Size

**Functions:**

-   **`ioctl(int fd, unsigned long request, ... )`**: A system call used to control device parameters. Here, it is used with the `TIOCGWINSZ` request to get the size of the terminal window.
-   **`getWindowSize(int *rows, int *cols)`**: This function attempts to retrieve the current terminal size. It fills in the provided pointers with the number of rows and columns.

**Structs:**

-   **`struct winsize`**: This struct holds the dimensions of the terminal window, including rows and columns.

**Constants:**

-   **`TIOCGWINSZ`**: A request code used with `ioctl` to get the window size.

## Editor Initialization

**Structs:**

-   The updated `struct editorConfig` now includes `screenrows` and `screencols`, representing the dimensions of the terminal for the editor display.

**Functions:**

-   **`initEditor()`**: Initializes the editor configuration, specifically by calling `getWindowSize()` to set the screen dimensions.

### Drawing Rows on the Screen

**Functions:**

-   **`editorDrawRows()`**: This function is responsible for drawing the rows on the terminal, utilizing the number of rows stored in `screenrows`.

### Window Size (Fallback Method)

**Escape Sequences:**

-   **Escape sequences** are special character sequences that control cursor movement and terminal behavior. In this case, sequences like `"\x1b[999C"` (cursor forward) and `"\x1b[999B"` (cursor down) are used to move the cursor to the bottom-right corner of the terminal.
-   **Device Status Report**: Sending the command `"\x1b[6n"` (where `6` is the argument for cursor position) requests the cursor's current position from the terminal.

## Summary of Escape Sequences:

-   **Cursor Movement**:
    -   `C` (Cursor Forward): Moves the cursor right by the specified number of spaces.
    -   `B` (Cursor Down): Moves the cursor down by the specified number of lines.
-   **Device Status Report (DSR)**: Requests information from the terminal, such as the cursor's position.
