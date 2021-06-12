#include "Keyboard.h"

    bool IsLeftShiftPressed;
    bool IsRightShiftPressed;

void HandleKeyboard(uint8_t Scancode){

    switch (Scancode){
        case LeftShift:
            IsLeftShiftPressed = true;
            return;
        case LeftShift + 0x80:
            IsLeftShiftPressed = false;
            return;
        case RightShift:
            IsRightShiftPressed = true;
            return;
        case RightShift + 0x80:
            IsRightShiftPressed = false;
            return;
        case Enter:
            GlobalRenderer->Next();
            return;
        case Spacebar:
            GlobalRenderer->PutChar(' ');
            return;
        case BackSpace:
           GlobalRenderer->ClearChar();
           return;
    }

    char ASCII = QWERTYKeyboard::Translate(Scancode, IsLeftShiftPressed | IsRightShiftPressed);

    if (ASCII != 0){
        GlobalRenderer->PutChar(ASCII);
    }

}