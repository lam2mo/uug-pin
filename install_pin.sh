#!/bin/bash

INSTALL_DIR="$HOME/opt"
WEB_DIR="http://software.intel.com/sites/landingpage/pintool/downloads"
TARBALL="pin-3.30-98830-g1d7b601b3-gcc-linux.tar.gz"

mkdir -p "$INSTALL_DIR"
cd "$INSTALL_DIR"
wget "$WEB_DIR/$TARBALL"
tar -xf "$TARBALL"

