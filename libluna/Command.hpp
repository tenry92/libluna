#pragma once

namespace Luna {
  class Command {
    public:
    virtual ~Command();
    virtual void execute() const = 0;
  };
} // namespace Luna
