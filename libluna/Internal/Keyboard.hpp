#pragma once

#include <libluna/config.h>

#ifdef LUNA_WINDOW_SDL2
#include <SDL2/SDL.h>
#endif

#include <libluna/String.hpp>

namespace Luna::Internal {
  namespace Keyboard {
#ifdef LUNA_WINDOW_SDL2
    String sdlKeycodeToName(SDL_Keycode keycode) {
      if (keycode > ' ' && keycode <= '}') {
        return String("Keyboard/Keycode/") + keycode;
      }

      return "";
    }

    String sdlScancodeToName(SDL_Scancode scancode) {
      String base("Keyboard/Scancode/");

      if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_Z) {
        return base + ('A' + scancode - SDL_SCANCODE_A);
      }

      if (scancode >= SDL_SCANCODE_0 && scancode <= SDL_SCANCODE_9) {
        return base + ('0' + scancode - SDL_SCANCODE_9);
      }

      if (scancode >= SDL_SCANCODE_F1 && scancode <= SDL_SCANCODE_F9) {
        return base + 'F' + ('1' + scancode - SDL_SCANCODE_F1);
      }

      if (scancode >= SDL_SCANCODE_F10 && scancode <= SDL_SCANCODE_F12) {
        return base + "F1" + ('0' + scancode - SDL_SCANCODE_F10);
      }

      switch (scancode) {
      case SDL_SCANCODE_RIGHT:
        return base + "ArrowRight";
      case SDL_SCANCODE_LEFT:
        return base + "ArrowLeft";
      case SDL_SCANCODE_DOWN:
        return base + "ArrowDown";
      case SDL_SCANCODE_UP:
        return base + "ArrowUp";
      case SDL_SCANCODE_RETURN:
        return base + "Return";
      case SDL_SCANCODE_ESCAPE:
        return base + "Escape";
      case SDL_SCANCODE_TAB:
        return base + "Tab";
      case SDL_SCANCODE_BACKSPACE:
        return base + "Backspace";
      case SDL_SCANCODE_SPACE:
        return base + "Space";
      case SDL_SCANCODE_MINUS:
        return base + "Minus";
      case SDL_SCANCODE_EQUALS:
        return base + "Equals";
      case SDL_SCANCODE_LEFTBRACKET:
        return base + "LeftBracked";
      case SDL_SCANCODE_RIGHTBRACKET:
        return base + "RightBracket";
      case SDL_SCANCODE_BACKSLASH:
        return base + "Backslash";
      case SDL_SCANCODE_SEMICOLON:
        return base + "Semicolon";
      case SDL_SCANCODE_GRAVE:
        return base + "Grave";
      case SDL_SCANCODE_COMMA:
        return base + "Comma";
      case SDL_SCANCODE_PERIOD:
        return base + "Period";
      case SDL_SCANCODE_SLASH:
        return base + "Slash";
      case SDL_SCANCODE_PRINTSCREEN:
        return base + "PrintScreen";
      case SDL_SCANCODE_SCROLLLOCK:
        return base + "ScrollLock";
      case SDL_SCANCODE_PAUSE:
        return base + "Pause";
      case SDL_SCANCODE_LCTRL:
        return base + "LeftControl";
      case SDL_SCANCODE_LSHIFT:
        return base + "LeftShift";
      case SDL_SCANCODE_LALT:
        return base + "LeftAlt"; /* macOS: option */
      case SDL_SCANCODE_LGUI:
        return base + "LeftSystem"; /* Windows, Command, Meta */
      case SDL_SCANCODE_RCTRL:
        return base + "RightControl";
      case SDL_SCANCODE_RSHIFT:
        return base + "RightShift";
      case SDL_SCANCODE_RALT:
        return base + "RightAlt"; /* Alt, Alt Gr, Option */
      case SDL_SCANCODE_RGUI:
        return base + "RightSystem";
      default:
        return "";
      }
    }
#endif
  } // namespace Keyboard
} // namespace Luna::Internal
