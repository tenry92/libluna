#include <libluna/Console.hpp>

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

#ifdef N64
#include <libdragon.h>
#endif

#ifdef _WIN32
static WORD originalConsoleAttributes;
static bool hasOriginalConsoleAttributes{false};
#endif

using namespace Luna;

void Console::init() {
#ifdef _NDS
  consoleDemoInit();
#endif
#ifdef __3DS__
  consoleInit(GFX_BOTTOM, NULL);
#endif
#ifdef __SWITCH__
  consoleInit(NULL);
#endif
#ifdef N64
  console_init();
  console_set_render_mode(RENDER_MANUAL);
#endif
}

void Console::update() {
#ifdef __SWITCH__
  consoleUpdate(NULL);
#endif
#ifdef N64
  console_render();
#endif
}

void Console::quit() {
#ifdef __SWITCH__
  consoleExit(NULL);
#endif
}

void Console::write(const String &message) {
#ifdef N64
  printf("%s", message.c_str());
#else
  std::cout << message.s_str();
#endif
}

void Console::writeLine(const String &message) {
#ifdef N64
  printf("%s\n", message.c_str());
#else
  std::cout << message.s_str() << std::endl;
#endif
}

void Console::setColor(
    MAYBE_UNUSED Color fgColor, MAYBE_UNUSED Color bgColor
) {
#ifdef UNIX_COLORS
  // see man Console-colors.d
  // 0: reset all color
  // 1: bright, bold
  // 30-37: foreground
  // 40-47: background

  write("\033[");

  switch (fgColor) {
  default:
  case black:
    write("0;30");
    break;
  case red:
    write("0;31");
    break;
  case brightRed:
    write("1;31");
    break;
  case green:
    write("0;32");
    break;
  case brightGreen:
    write("1;32");
    break;
  case yellow:
    write("0;33");
    break;
  case brightYellow:
    write("1;33");
    break;
  case blue:
    write("0;34");
    break;
  case brightBlue:
    write("1;34");
    break;
  case purple:
    write("0;35");
    break;
  case brightPurple:
    write("1;35");
    break;
  case cyan:
    write("0;36");
    break;
  case brightCyan:
    write("1;36");
    break;
  case gray:
    write("0;37");
    break;
  case brightGray:
    write("1;37");
    break;
  }

  switch (bgColor) {
  // case black: std::cout << ";40m"; break; <- actually is darkGray
  default:
  case black:
    write("m");
    break;
  case red:
  case brightRed:
    write(";41m");
    break;
  case green:
  case brightGreen:
    write(";42m");
    break;
  case yellow:
  case brightYellow:
    write(";43m");
    break;
  case blue:
  case brightBlue:
    write(";44m");
    break;
  case purple:
  case brightPurple:
    write(";45m");
    break;
  case cyan:
  case brightCyan:
    write(";46m");
    break;
  case gray:
  case brightGray:
    write(";47m");
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

void Console::resetColor() {
#ifdef UNIX_COLORS
  write("\033[0m");
#elif _WIN32
  if (hasOriginalConsoleAttributes) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, originalConsoleAttributes);
  }
#endif
}
