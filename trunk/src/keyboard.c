//
// keyboard.c -- Defines the functions related to the keyboard driver.
//          Written for JamesM's kernel development tutorials.
//

#include "keyboard.h"

//Define your keyboard layouts here:

keymap_t us = {
  //normal keys
  {
    /* first row - indices 0 to 14 */
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',

    /* second row - indices 15 to 28 */
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', //Enter key

    /* 29 = Control, 30 - 41: third row */
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',

    /* fourth row, indices 42 to 54, zeroes are shift-keys*/
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,

    '*',

    /* Special keys */

    0, //ALT - 56
    ' ', // Space - 57
    0, //Caps lock - 58
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F1 to F10 - 59 to 68
    0, //Num lock - 69
    0, //Scroll lock - 70
    0, //Home - 71
    0, //Up arrow - 72
    0, //Page up - 73
    '-',
    0, //Left arrow - 75
    0,
    0, //Right arrow -77
    '+',
    0, //End - 79
    0, //Dowm arrow - 80
    0, //Page down - 81
    0, //Insert - 82
    0, //Delete - 83
    0, 0, 0,
    0, //F11 - 87
    0, //F12 - 88
    0, //All others undefined
  },
  //caps
  {
    /* first row - indices 0 to 14 */
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',

    /* second row - indices 15 to 28 */
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', //Enter key

    /* 29 = Control, 30 - 41: third row */
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',

    /* fourth row, indices 42 to 54, zeroes are shift-keys*/
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,

    '*',

    /* Special keys */

    0, //ALT - 56
    ' ', // Space - 57
    0, //Caps lock - 58
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F1 to F10 - 59 to 68
    0, //Num lock - 69
    0, //Scroll lock - 70
    0, //Home - 71
    0, //Up arrow - 72
    0, //Page up - 73
    '-',
    0, //Left arrow - 75
    0,
    0, //Right arrow -77
    '+',
    0, //End - 79
    0, //Dowm arrow - 80
    0, //Page down - 81
    0, //Insert - 82
    0, //Delete - 83
    0, 0, 0,
    0, //F11 - 87
    0, //F12 - 88
    0, //All others undefined
  },
  //control_map
  { 29, 56, 0, 42, 54, 58, 70, 69 },
  0
};

//Here begins the functionality
keymap_t *current_layout;

void init_keyboard_driver() {
  register_interrupt_handler(IRQ1, &keyboard_handler);
  switch_layout(&us);
}

void switch_layout(keymap_t *layout) {
  current_layout = layout;
}

interrupt_handler_t keyboard_handler(registers_t regs) {
  
  uint8_t scancode = inb(0x60);
  
  // Has the key been released? Check bit no. 7
  if(scancode & 0x80) {
    //We only need to check the first five ones,
    //because they're the only ones doing anything
    //on release
    uint32_t i;
    for(i = 0; i < 5; i++) {
      if(current_layout->control_map[i] == (scancode & 0x7F)) {
        //releasing the key always disables its function
        current_layout->controls &= (~(1 << i));
        return;
      }
    }
  }
  else
  {
    //check if pressed key was a control key and
    //invert its bit in the status map
    uint32_t i;
    for(i = 0; i < 8; i++) {
      if(current_layout->control_map[i] == scancode) {
        //if bit is set, delete it
        if(current_layout->controls & (1 << i))
        {
          current_layout->controls &= (~(1 << i));
        }
        //if not, set it
        else
        {
          current_layout->controls |= (1 << i);
        }
        return;
      }
    }

    //if it was a non-control key, just print it upper or lowercase version
    //depending on the status of the control keys
    if((current_layout->controls & 0x38) && !(current_layout->controls & 0x1))
    {
      monitor_put(current_layout->shift_scancodes[scancode]);
    }
    else
    {
      monitor_put(current_layout->scancodes[scancode]);
    }
  }
}