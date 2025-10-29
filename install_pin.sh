#!/bin/bash

INSTALL_DIR="$HOME/opt"
WEB_DIR="https://software.intel.com/sites/landingpage/pintool/downloads/"
TARBALL="pin-external-4.0-99633-g5ca9893f2-gcc-linux.tar.gz"

mkdir -p "$INSTALL_DIR"
cd "$INSTALL_DIR"
wget "$WEB_DIR/$TARBALL"
tar -xf "$TARBALL"

