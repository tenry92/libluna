#pragma once

#ifdef LUNA_WINDOW_SDL2
#include <SDL2/SDL.h>
#endif

namespace Luna::Input {
  class Keyboard {
    public:
    enum Scancode {
      kA, kB, kC, kD, kE, kF, kG, kH, kI, kJ, kK, kL, kM,
      kN, kO, kP, kQ, kR, kS, kT, kU, kV, kW, kX, kY, kZ,

      kGrave,         // `
      k1, k2, k3, k4, k5, k6, k7, k8, k9, k0,
      kMinus,         // -
      kEquals,        // =
      kBackspace,

      kTab,
      kLeftBracket,   // [
      kRightBracket,  // ]
      kBackslash,     // '\'
      kSemicolon,     // ;
      kApostrophe,    // '
      kEnter,

      kComma,         // ,
      kPeriod,        // .
      kSlash,         // /

      kSpace,

      kEscape,

      kF1, kF2, kF3, kF4, kF5, kF6, kF7, kF8, kF9, kF10, kF11, kF12,

      kPrintScreen,
      kScrollLock,
      kPause,

      kInsert,
      kDelete,
      kHome,
      kEnd,
      kPageUp,
      kPageDown,

      kArrowUp,
      kArrowDown,
      kArrowLeft,
      kArrowRight,

      kCapsLock,

      kNumLock,
      kKeypadDivide,
      kKeypadMultiply,
      kKeypadMinus,
      kKeypadPlus,
      kKeypadEnter,
      kKeypad1,
      kKeypad2,
      kKeypad3,
      kKeypad4,
      kKeypad5,
      kKeypad6,
      kKeypad7,
      kKeypad8,
      kKeypad9,
      kKeypad0,
      kKeypadPeriod,

      kLeftCtrl,
      kLeftShift,
      kLeftAlt,
      kLeftSuper,     // Windows/Command key

      kRightCtrl,
      kRightShift,
      kRightAlt,
      kRightSuper,

      kMenu,

      kUnknown
    };

#ifdef LUNA_WINDOW_SDL2
    static inline Scancode fromSdlScancode(SDL_Scancode scancode) {
      switch (scancode) {
        case SDL_SCANCODE_A: return kA;
        case SDL_SCANCODE_B: return kB;
        case SDL_SCANCODE_C: return kC;
        case SDL_SCANCODE_D: return kD;
        case SDL_SCANCODE_E: return kE;
        case SDL_SCANCODE_F: return kF;
        case SDL_SCANCODE_G: return kG;
        case SDL_SCANCODE_H: return kH;
        case SDL_SCANCODE_I: return kI;
        case SDL_SCANCODE_J: return kJ;
        case SDL_SCANCODE_K: return kK;
        case SDL_SCANCODE_L: return kL;
        case SDL_SCANCODE_M: return kM;
        case SDL_SCANCODE_N: return kN;
        case SDL_SCANCODE_O: return kO;
        case SDL_SCANCODE_P: return kP;
        case SDL_SCANCODE_Q: return kQ;
        case SDL_SCANCODE_R: return kR;
        case SDL_SCANCODE_S: return kS;
        case SDL_SCANCODE_T: return kT;
        case SDL_SCANCODE_U: return kU;
        case SDL_SCANCODE_V: return kV;
        case SDL_SCANCODE_W: return kW;
        case SDL_SCANCODE_X: return kX;
        case SDL_SCANCODE_Y: return kY;
        case SDL_SCANCODE_Z: return kZ;
        case SDL_SCANCODE_GRAVE: return kGrave;
        case SDL_SCANCODE_1: return k1;
        case SDL_SCANCODE_2: return k2;
        case SDL_SCANCODE_3: return k3;
        case SDL_SCANCODE_4: return k4;
        case SDL_SCANCODE_5: return k5;
        case SDL_SCANCODE_6: return k6;
        case SDL_SCANCODE_7: return k7;
        case SDL_SCANCODE_8: return k8;
        case SDL_SCANCODE_9: return k9;
        case SDL_SCANCODE_0: return k0;
        case SDL_SCANCODE_MINUS: return kMinus;
        case SDL_SCANCODE_EQUALS: return kEquals;
        case SDL_SCANCODE_BACKSPACE: return kBackspace;
        case SDL_SCANCODE_TAB: return kTab;
        case SDL_SCANCODE_LEFTBRACKET: return kLeftBracket;
        case SDL_SCANCODE_RIGHTBRACKET: return kRightBracket;
        case SDL_SCANCODE_BACKSLASH: return kBackslash;
        case SDL_SCANCODE_SEMICOLON: return kSemicolon;
        case SDL_SCANCODE_APOSTROPHE: return kApostrophe;
        case SDL_SCANCODE_RETURN: return kEnter;
        case SDL_SCANCODE_COMMA: return kComma;
        case SDL_SCANCODE_PERIOD: return kPeriod;
        case SDL_SCANCODE_SLASH: return kSlash;
        case SDL_SCANCODE_SPACE: return kSpace;
        case SDL_SCANCODE_ESCAPE: return kEscape;
        case SDL_SCANCODE_F1: return kF1;
        case SDL_SCANCODE_F2: return kF2;
        case SDL_SCANCODE_F3: return kF3;
        case SDL_SCANCODE_F4: return kF4;
        case SDL_SCANCODE_F5: return kF5;
        case SDL_SCANCODE_F6: return kF6;
        case SDL_SCANCODE_F7: return kF7;
        case SDL_SCANCODE_F8: return kF8;
        case SDL_SCANCODE_F9: return kF9;
        case SDL_SCANCODE_F10: return kF10;
        case SDL_SCANCODE_F11: return kF11;
        case SDL_SCANCODE_F12: return kF12;
        case SDL_SCANCODE_PRINTSCREEN: return kPrintScreen;
        case SDL_SCANCODE_SCROLLLOCK: return kScrollLock;
        case SDL_SCANCODE_PAUSE: return kPause;
        case SDL_SCANCODE_INSERT: return kInsert;
        case SDL_SCANCODE_DELETE: return kDelete;
        case SDL_SCANCODE_HOME: return kHome;
        case SDL_SCANCODE_END: return kEnd;
        case SDL_SCANCODE_PAGEUP: return kPageUp;
        case SDL_SCANCODE_PAGEDOWN: return kPageDown;
        case SDL_SCANCODE_UP: return kArrowUp;
        case SDL_SCANCODE_DOWN: return kArrowDown;
        case SDL_SCANCODE_LEFT: return kArrowLeft;
        case SDL_SCANCODE_RIGHT: return kArrowRight;
        case SDL_SCANCODE_CAPSLOCK: return kCapsLock;
        case SDL_SCANCODE_NUMLOCKCLEAR: return kNumLock;
        case SDL_SCANCODE_KP_DIVIDE: return kKeypadDivide;
        case SDL_SCANCODE_KP_MULTIPLY: return kKeypadMultiply;
        case SDL_SCANCODE_KP_MINUS: return kKeypadMinus;
        case SDL_SCANCODE_KP_PLUS: return kKeypadPlus;
        case SDL_SCANCODE_KP_ENTER: return kKeypadEnter;
        case SDL_SCANCODE_KP_1: return kKeypad1;
        case SDL_SCANCODE_KP_2: return kKeypad2;
        case SDL_SCANCODE_KP_3: return kKeypad3;
        case SDL_SCANCODE_KP_4: return kKeypad4;
        case SDL_SCANCODE_KP_5: return kKeypad5;
        case SDL_SCANCODE_KP_6: return kKeypad6;
        case SDL_SCANCODE_KP_7: return kKeypad7;
        case SDL_SCANCODE_KP_8: return kKeypad8;
        case SDL_SCANCODE_KP_9: return kKeypad9;
        case SDL_SCANCODE_KP_0: return kKeypad0;
        case SDL_SCANCODE_KP_PERIOD: return kKeypadPeriod;
        case SDL_SCANCODE_LCTRL: return kLeftCtrl;
        case SDL_SCANCODE_LSHIFT: return kLeftShift;
        case SDL_SCANCODE_LALT: return kLeftAlt;
        case SDL_SCANCODE_LGUI: return kLeftSuper;
        case SDL_SCANCODE_RCTRL: return kRightCtrl;
        case SDL_SCANCODE_RSHIFT: return kRightShift;
        case SDL_SCANCODE_RALT: return kRightAlt;
        case SDL_SCANCODE_RGUI: return kRightSuper;
        case SDL_SCANCODE_MENU: return kMenu;
        default: return kUnknown;
      }
    };
#endif
  };
}
