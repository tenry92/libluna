#pragma once

#include <memory>

namespace Luna {
  class ImmediateGuiImpl;

  class ImmediateGui {
    public:
    ImmediateGui();
    virtual ~ImmediateGui();

    ImmediateGuiImpl *getImpl() const;

    protected:
    friend class ImmediateGuiImpl;
    virtual void render() = 0;

    private:
    std::unique_ptr<ImmediateGuiImpl> mImpl;
  };
} // namespace Luna
