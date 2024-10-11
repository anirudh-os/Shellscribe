# Shellscribe

Shellscribe is a simple yet powerful terminal-based text editor designed for quick and efficient editing of text files. Built in C, it aims to provide a lightweight alternative for users who prefer working directly in the terminal.

## Features

- **Lightweight**: Minimalistic design for fast performance.
- **Syntax Highlighting**: Supports syntax highlighting for various programming languages.
- **Basic Editing Functions**: Includes essential features such as cut, copy, paste, undo, and redo.
- **File Management**: Easily open, save, and close files.
- **Configurable Key Bindings**: Customize key bindings to suit your workflow.
- **Cross-Platform**: Works on various Unix-like operating systems.

## Requirements

- GCC or any compatible C compiler
- `ncurses` library (for terminal handling)

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/shellscribe.git
   cd shellscribe
   ```
2. Compile the source code using the provided MAKEFILE:
   ```bash
   make -f MAKEFILE
   ```
3. Run the editor:
   ```bash
   ./shellscribe
   ```
