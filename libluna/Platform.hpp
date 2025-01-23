#pragma once

#include <libluna/config.h>

#include <vector>

#include <libluna/String.hpp>
#include <libluna/Vector.hpp>

namespace Luna {
  /**
   * @brief Get information about the current platform.
   */
  class Platform {
    public:
    static constexpr inline bool isLinux() {
#ifdef __linux__
      return true;
#else
      return false;
#endif
    }

    static constexpr inline bool isWindows() {
#ifdef _WIN32
      return true;
#else
      return false;
#endif
    }

    static constexpr inline bool isEmscripten() {
#ifdef __EMSCRIPTEN__
      return true;
#else
      return false;
#endif
    }

    static constexpr inline bool isPc() { return isLinux() || isWindows(); }

    static constexpr inline bool isNds() {
#ifdef _NDS
      return true;
#else
      return false;
#endif
    }

    static constexpr inline bool is3ds() {
#ifdef __3DS__
      return true;
#else
      return false;
#endif
    }

    static constexpr inline bool isSwitch() {
#ifdef __SWITCH__
      return true;
#else
      return false;
#endif
    }

    static constexpr inline bool isN64() {
#ifdef N64
      return true;
#else
      return false;
#endif
    }

    static constexpr inline bool isDebug() {
#ifdef NDEBUG
      return false;
#else
      return true;
#endif
    }

    class NintendoDs {
      public:
      static inline Vector2i getScreenResolution() {
        return Vector2i(256, 192); // 4:3
      }
    };

    class Nintendo3ds {
      public:
      static inline Vector2i getTopResolution() {
        return Vector2i(400, 240); // 15:9
      }

      static inline Vector2i getBottomResolution() {
        return Vector2i(320, 240); // 4:3
      }
    };

    class Switch {
      public:
      static inline Vector2i getHalfResolution() {
        return Vector2i(960, 540); // 16:9
      }

      static inline Vector2i getFullResolution() {
        return Vector2i(1920, 1080); // 16:9
      }
    };

    static std::vector<String> getCompilerInfo();

    static inline const char* getSystemName() {
      if (Platform::isLinux()) {
        return "Linux";
      } else if (Platform::isWindows()) {
        return "Windows";
      } else if (Platform::isEmscripten()) {
        return "Web (Emscripten)";
      } else if (Platform::isNds()) {
        return "Nintendo DS";
      } else if (Platform::is3ds()) {
        return "Nintendo 3DS";
      } else if (Platform::isSwitch()) {
        return "Nintendo Switch";
      } else if (Platform::isN64()) {
        return "Nintendo 64";
      } else {
        return "Unknown";
      }
    }
  };
} // namespace Luna
