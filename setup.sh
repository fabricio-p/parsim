#!/usr/bin/env bash

# Downloads and extracts raylib release version 5.0

curl -LO https://github.com/raysan5/raylib/releases/download/5.0/raylib-5.0_linux_amd64.tar.gz
tar -xf raylib-5.0_linux_amd64.tar.gz
# Remove the dynamic libs because we want to link statically but I couldn't do it with the flags
rm raylib-5.0_linux_amd64/lib/*.so*