#pragma once

#include <memory>
#include <vector>

#include <libluna/String.hpp>

namespace Luna {
  /**
   * @brief Helper for parsing command-line arguments.
   *
   * @todo Implement ArgParser.
   */
  class ArgParser {
    bool parse(int argc, char **argv);

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
