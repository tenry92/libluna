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
  run_as_root apt-get install -y libglm-dev libsdl2-dev libfmt-dev
  unset DEBIAN_FRONTEND
elif command_exists yum; then
  # CentOS 6 and earlier, RHEL 6 and earlier
  echo "Using yum to install development libraries."
  run_as_root yum install -y glm-devel SDL2-devel fmt-devel
elif command_exists dnf; then
  # Fedora, CentOS 7 and later, RHEL 7 and later
  echo "Using dnf to install development libraries."
  run_as_root dnf install -y glm-devel SDL2-devel fmt-devel
elif command_exists pacman; then
  # Arch Linux, Manjaro, ArcoLinux
  echo "Using pacman to install development libraries."
  run_as_root pacman -S --noconfirm glm utf8cpp sdl2 fmt
elif command_exists zypper; then
  # openSUSE, SUSE Linux Enterprise
  echo "Using zypper to install development libraries."
  run_as_root zypper install -y glm-devel libSDL2-devel libfmt-devel
else
  echo "No supported package manager found. Please install the development libraries manually."
  exit 1
fi

echo "Development libraries installed successfully."
