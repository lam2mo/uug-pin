#!/bin/bash

INSTALL_DIR="$HOME/opt"
WEB_DIR="http://software.intel.com/sites/landingpage/pintool/downloads"
TARBALL="pin-3.0-76991-gcc-linux.tar.gz"

mkdir -p "$INSTALL_DIR"
cd "$INSTALL_DIR"
wget "$WEB_DIR/$TARBALL"
tar -xf "$TARBALL"

