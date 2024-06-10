#pragma once

#include <memory>
#include <vector>

#include <libluna/String.hpp>

namespace Luna {
  class ArgParser {
    bool parse(int argc, char **argv);

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
