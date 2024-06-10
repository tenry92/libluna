#include <libluna/Terminal.hpp>

#include <list>

#if defined(__linux__)
#define UNIX_COLORS
#endif

#ifdef UNIX_COLORS
#include <iostream>
#elif _WIN32
#include <windows.h>
#define BG(NAME) BACKGROUND_##NAME
#define FG(NAME) BACKGROUND_##NAME
#endif

#ifdef __GNUC__
#define MAYBE_UNUSED [[maybe_unused]]
#else
#define MAYBE_UNUSED
#endif

#ifdef _NDS
#include <nds.h>
#endif

#ifdef __3DS__
#include <3ds.h>
#endif

#ifdef __SWITCH__
#include <switch.h>
#endif

#ifdef _WIN32
static WORD originalConsoleAttributes;
static bool hasOriginalConsoleAttributes{false};
#endif

using namespace Luna;

void Terminal::init() {
#ifdef _NDS
  consoleDemoInit();
#endif
#ifdef __3DS__
  consoleInit(GFX_BOTTOM, NULL);
#endif
#ifdef __SWITCH__
  consoleInit(NULL);
#endif
}

void Terminal::update() {
#ifdef __SWITCH__
  consoleUpdate(NULL);
#endif
}

void Terminal::quit() {
#ifdef __SWITCH__
  consoleExit(NULL);
#endif
}

void Terminal::setColor(
    MAYBE_UNUSED Color fgColor, MAYBE_UNUSED Color bgColor
) {
#ifdef UNIX_COLORS
  // see man terminal-colors.d
  // 0: reset all color
  // 1: bright, bold
  // 30-37: foreground
  // 40-47: background

  std::cout << "\033[";

  switch (fgColor) {
  default:
  case black:
    std::cout << "0;30";
    break;
  case red:
    std::cout << "0;31";
    break;
  case brightRed:
    std::cout << "1;31";
    break;
  case green:
    std::cout << "0;32";
    break;
  case brightGreen:
    std::cout << "1;32";
    break;
  case yellow:
    std::cout << "0;33";
    break;
  case brightYellow:
    std::cout << "1;33";
    break;
  case blue:
    std::cout << "0;34";
    break;
  case brightBlue:
    std::cout << "1;34";
    break;
  case purple:
    std::cout << "0;35";
    break;
  case brightPurple:
    std::cout << "1;35";
    break;
  case cyan:
    std::cout << "0;36";
    break;
  case brightCyan:
    std::cout << "1;36";
    break;
  case gray:
    std::cout << "0;37";
    break;
  case brightGray:
    std::cout << "1;37";
    break;
  }

  switch (bgColor) {
  // case black: std::cout << ";40m"; break; <- actually is darkGray
  default:
  case black:
    std::cout << "m";
    break;
  case red:
  case brightRed:
    std::cout << ";41m";
    break;
  case green:
  case brightGreen:
    std::cout << ";42m";
    break;
  case yellow:
  case brightYellow:
    std::cout << ";43m";
    break;
  case blue:
  case brightBlue:
    std::cout << ";44m";
    break;
  case purple:
  case brightPurple:
    std::cout << ";45m";
    break;
  case cyan:
  case brightCyan:
    std::cout << ";46m";
    break;
  case gray:
  case brightGray:
    std::cout << ";47m";
    break;
  }
#elif _WIN32
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

  if (!hasOriginalConsoleAttributes) {
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    originalConsoleAttributes = consoleInfo.wAttributes;
    hasOriginalConsoleAttributes = true;
  }

  WORD attributes = 0;

  switch (bgColor) {
  default:
  case black:
    attributes = 0;
    break;
  case red:
    attributes = BG(RED);
    break;
  case brightRed:
    attributes = BG(INTENSITY) | BG(RED);
    break;
  case green:
    attributes = BG(GREEN);
    break;
  case brightGreen:
    attributes = BG(INTENSITY) | BG(GREEN);
    break;
  case yellow:
    attributes = BG(RED) | BG(GREEN);
    break;
  case brightYellow:
    attributes = BG(INTENSITY) | BG(RED) | BG(GREEN);
    break;
  case blue:
    attributes = BG(BLUE);
    break;
  case brightBlue:
    attributes = BG(INTENSITY) | BG(BLUE);
    break;
  case purple:
    attributes = BG(RED) | BG(BLUE);
    break;
  case brightPurple:
    attributes = BG(INTENSITY) | BG(RED) | BG(BLUE);
    break;
  case cyan:
    attributes = BG(GREEN) | BG(BLUE);
    break;
  case brightCyan:
    attributes = BG(INTENSITY) | BG(GREEN) | BG(BLUE);
    break;
  case gray:
    attributes = BG(RED) | BG(GREEN) | BG(BLUE);
    break;
  case brightGray:
    attributes = BG(INTENSITY) | BG(RED) | BG(GREEN) | BG(BLUE);
    break;
  }

  switch (fgColor) {
  default:
  case black:
    attributes |= 0;
    break;
  case red:
    attributes |= FG(RED);
    break;
  case brightRed:
    attributes |= FG(INTENSITY) | FG(RED);
    break;
  case green:
    attributes |= FG(GREEN);
    break;
  case brightGreen:
    attributes |= FG(INTENSITY) | FG(GREEN);
    break;
  case yellow:
    attributes |= FG(RED) | FG(GREEN);
    break;
  case brightYellow:
    attributes |= FG(INTENSITY) | FG(RED) | FG(GREEN);
    break;
  case blue:
    attributes |= FG(BLUE);
    break;
  case brightBlue:
    attributes |= FG(INTENSITY) | FG(BLUE);
    break;
  case purple:
    attributes |= FG(RED) | FG(BLUE);
    break;
  case brightPurple:
    attributes |= FG(INTENSITY) | FG(RED) | FG(BLUE);
    break;
  case cyan:
    attributes |= FG(GREEN) | FG(BLUE);
    break;
  case brightCyan:
    attributes |= FG(INTENSITY) | FG(GREEN) | FG(BLUE);
    break;
  case gray:
    attributes |= FG(RED) | FG(GREEN) | FG(BLUE);
    break;
  case brightGray:
    attributes |= FG(INTENSITY) | FG(RED) | FG(GREEN) | FG(BLUE);
    break;
  }

  SetConsoleTextAttribute(hConsole, attributes);
#endif
}

void Terminal::resetColor() {
#ifdef UNIX_COLORS
  std::cout << "\033[0m";
#elif _WIN32
  if (hasOriginalConsoleAttributes) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, originalConsoleAttributes);
  }
#endif
}
