#pragma once

namespace Luna {
  class Material {
    public:
    Material();
    ~Material();

    void setDiffuseTexture(int slot);
    int getDiffuseTexture() const;

    void setNormalTexture(int slot);
    int getNormalTexture() const;

    private:
    int mDiffuseTexture{0};
    int mNormalTexture{0};
  };
} // namespace Luna
