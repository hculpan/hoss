#include "keyboard.h"
#include "../cpu/ports.h"
#include "../cpu/isr.h"
#include "../libc/string.h"
#include "../libc/function.h"
#include "../kernel/kernel.h"

#define BACKSPACE 0x0E
#define ENTER 0x1C

#define LEFT_SHIFT      42
#define RIGHT_SHIFT     54
#define CTRL            29
#define ALT             56
#define CAPS_LOCK       186

#define ON  1;
#define OFF 0;

static char shiftPressed = OFF;

static char ctrlPressed = OFF;

static char altPressed = OFF;

static char capsLock = OFF;

static unsigned char last_scancode = 0;

static unsigned char last_ascii = 0;

#define SC_MAX 57
const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6", 
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E", 
        "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl", 
        "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", 
        "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", 
        "/", "RShift", "Keypad *", "LAlt", "Spacebar"};
const char sc_ascii[] = { '\a', '\a', '1', '2', '3', '4', '5', '6',     
        '7', '8', '9', '0', '-', '=', '\a', '\a', 'q', 'w', 'e', 'r', 't', 'y', 
        'u', 'i', 'o', 'p', '[', ']', '\a', '\a', 'a', 's', 'd', 'f', 'g', 
        'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v', 
        'b', 'n', 'm', ',', '.', '/', '\a', '\a', '\a', ' '};

const char sc_ascii_shifted[] = { '\a', '\a', '!', '@', '#', '$', '%', '^',     
        '&', '*', '(', ')', '_', '+', '\a', '\a', 'Q', 'W', 'E', 'R', 'T', 'Y', 
        'U', 'I', 'O', 'P', '{', '}', '\a', '\a', 'A', 'S', 'D', 'F', 'G', 
        'H', 'J', 'K', 'L', ':', '"', '~', '\a', '|', 'Z', 'X', 'C', 'V', 
        'B', 'N', 'M', '<', '>', '?', '\a', '\a', '\a', ' '};

static char scancodeToChar() {
    char result = '\0';

    unsigned char scancode = last_scancode;
    if (last_scancode >= 128) {
        scancode -= 128;
    }

    if (scancode <= SC_MAX) {
        if (scancode == BACKSPACE) {
            return '\b';
        } else if (scancode == ENTER) {
            return '\r';
        } else if (!shiftPressed && !capsLock) {
            return sc_ascii[(int)scancode];
        } else if (!capsLock) {
            return sc_ascii_shifted[(int)scancode];
        } else {  // capsLock == ON
            char letter = sc_ascii_shifted[(int)scancode];
            if (!shiftPressed && letter >= 'A' && letter <= 'Z') {
                return letter;
            } else if (shiftPressed && letter >= 'A' && letter <= 'Z') {
                return sc_ascii[(int)scancode];
            } else if (shiftPressed) {
                return sc_ascii_shifted[(int)scancode];
            } else {
                return sc_ascii[(int)scancode];
            }
        }
    }
    return (result == '\a' ? '\0' : result);
}

char waitForAscii() {
    char result = 0;
    while (!result) {
        result = last_ascii;
        last_ascii = 0;
        last_scancode = 0;
    }
    return result;
}

unsigned char waitForScancode() {
    unsigned char result = 0;
    while (!result) {
        result = last_scancode;
        last_scancode = 0;
        last_ascii = 0;
    }
    return result;
}

static void keyboard_callback(registers_t regs) {
    /* The PIC leaves us the scancode in port 0x60 */
    last_scancode = port_byte_in(0x60);
    
    if (last_scancode == LEFT_SHIFT || last_scancode == RIGHT_SHIFT) {
        shiftPressed = ON;
    } else if (last_scancode == LEFT_SHIFT + 128 || last_scancode == RIGHT_SHIFT + 128) {
        shiftPressed = OFF;
    } else if (last_scancode == CTRL) {
        ctrlPressed = ON;
    } else if (last_scancode == CTRL + 128) {
        ctrlPressed = OFF;
    } else if (last_scancode == ALT) {
        altPressed = ON;
    } else if (last_scancode == ALT + 128) {
        altPressed = OFF;
    } else if (last_scancode == CAPS_LOCK) {
        capsLock = !capsLock;
    } else if (last_scancode < 128) {
        char letter = scancodeToChar();
        if (letter != '\a') {
            last_ascii = letter;
        }
    }
    UNUSED(regs);
}

unsigned char isCtrlPressed() {
    return ctrlPressed;
}

unsigned char isAltPressed() {
    return altPressed;
}

unsigned char isShiftPressed() {
    return shiftPressed;
}

void init_keyboard() {
   register_interrupt_handler(IRQ1, keyboard_callback); 
}
