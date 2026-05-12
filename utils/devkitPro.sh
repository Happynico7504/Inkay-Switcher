#!/bin/bash

# MSYS2 - MINGW64
# NT

set -e
set -u
set -o pipefail

DEVKITPRO=/opt/devkitpro
DEVKITPPC="$DEVKITPRO/devkitPPC"

export DEVKITPRO="$DEVKITPRO"
export DEVKITPPC="$DEVKITPPC"

mkdir -p "$DEVKITPRO"
mkdir -p "$DEVKITPPC"

echo | pacman-key --recv BC26F752D25B92CE272E0F44F7FD5492264BB9D0 --keyserver keyserver.ubuntu.com
echo | pacman-key --lsign BC26F752D25B92CE272E0F44F7FD5492264BB9D0
pacman -U --noconfirm https://pkg.devkitpro.org/devkitpro-keyring.pkg.tar.zst
pacman-key --populate devkitpro

echo -e "\n[dkp-libs]\nServer = https://pkg.devkitpro.org/packages" | tee -a /etc/pacman.conf > /dev/null
echo -e "\n[dkp-wcindows]\nServer = https://pkg.devkitpro.org/packages/windows/\$arch/" | tee -a /etc/pacman.conf > /dev/null

pacman -Sy --noconfirm
pacman -S wiiu-dev git make wiiu-curl --noconfirm

echo 'Installed DevkitPro with WiiU repositories'