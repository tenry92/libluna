#!/usr/bin/env bash

command_exists() {
  command -v "$1" >/dev/null 2>&1
}

run_as_root() {
  if [ "$(id -u)" -ne 0 ]; then
    sudo "$@"
  else
    "$@"
  fi
}

if command_exists apt-get; then
  # Debian, Ubuntu, Linux Mint, elementary OS
  echo "Using apt-get to install development libraries."
  export DEBIAN_FRONTEND=noninteractive
  run_as_root apt-get update
  run_as_root apt-get install -y wget zlib1g-dev libpng-dev libopus-dev libopusfile-dev libglm-dev libsdl2-dev
  unset DEBIAN_FRONTEND
elif command_exists yum; then
  # CentOS 6 and earlier, RHEL 6 and earlier
  echo "Using yum to install development libraries."
  run_as_root yum install -y wget zlib-devel libpng-devel opus-devel opusfile-devel glm-devel SDL2-devel
elif command_exists dnf; then
  # Fedora, CentOS 7 and later, RHEL 7 and later
  echo "Using dnf to install development libraries."
  run_as_root dnf install -y wget zlib-devel libpng-devel opus-devel opusfile-devel glm-devel SDL2-devel
elif command_exists pacman; then
  # Arch Linux, Manjaro, ArcoLinux
  echo "Using pacman to install development libraries."
  run_as_root pacman -S --noconfirm wget zlib libpng opus opusfile glm utf8cpp sdl2
elif command_exists zypper; then
  # openSUSE, SUSE Linux Enterprise
  echo "Using zypper to install development libraries."
  run_as_root zypper install -y wget zlib-devel libpng16-devel libopus-devel libopusfile-devel glm-devel libSDL2-devel
else
  echo "No supported package manager found. Please install the development libraries manually."
  exit 1
fi

run_as_root wget -O /usr/include/utf8.h https://raw.githubusercontent.com/sheredom/utf8.h/master/utf8.h

echo "Development libraries installed successfully."
