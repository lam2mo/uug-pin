#!/bin/bash

tool="obj-intel64/$1.so"
shift

"$PIN_ROOT/pin" -t "$tool" -- $@

