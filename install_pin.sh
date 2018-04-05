#!/bin/bash

INSTALL_DIR="$HOME/opt"
WEB_DIR="http://software.intel.com/sites/landingpage/pintool/downloads"
TARBALL="pin-3.6-97554-g31f0a167d-gcc-linux.tar.gz"

mkdir -p "$INSTALL_DIR"
cd "$INSTALL_DIR"
wget "$WEB_DIR/$TARBALL"
tar -xf "$TARBALL"

