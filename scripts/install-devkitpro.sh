#!/usr/bin/env bash

run_as_root() {
  if [ "$(id -u)" -ne 0 ]; then
    sudo "$@"
  else
    "$@"
  fi
}

# download official devkitpro installer script
wget https://apt.devkitpro.org/install-devkitpro-pacman

# auto-accept all changes
sed -i 's/apt-get install /apt-get install -y /g' install-devkitpro-pacman

# run script
chmod +x ./install-devkitpro-pacman

run_as_root ./install-devkitpro-pacman

rm install-devkitpro-pacman

# for some mysterious reason, /etc/mtab is missing during docker build, but available during docker run
# pacman is failing if it can not read /etc/mtab
if [ ! -e /etc/mtab ]; then
  run_as_root ln -sf /proc/self/mounts /etc/mtab
fi

run_as_root /opt/devkitpro/pacman/bin/pacman -S --noconfirm gba-dev nds-dev 3ds-dev gamecube-dev wii-dev wiiu-dev switch-dev

run_as_root /opt/devkitpro/pacman/bin/pacman -S --noconfirm \
  3ds-bzip2 3ds-flac 3ds-freetype 3ds-libjson-c 3ds-libogg 3ds-libogg 3ds-libpng 3ds-opusfile \
  nds-bzip2 nds-libpng nds-zlib \
  switch-bzip2 switch-ffmpeg switch-flac switch-freetype switch-glad switch-glfw switch-glm \
  switch-libdrm_nouveau switch-libgd switch-libjson-c switch-libmodplug switch-libogg \
  switch-libopus switch-libpng switch-libvorbis switch-libwebp switch-mesa switch-opusfile \
  switch-sdl2 switch-sdl2_image switch-sdl2_mixer switch-sdl2_net switch-sdl2_ttf \
  switch-zlib

run_as_root /opt/devkitpro/pacman/bin/pacman -S --noconfirm 3ds-tinyxml2 switch-tinyxml2
