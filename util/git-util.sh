#!/bin/bash

cd $(dirname "$0")
find . -type d -empty -exec touch {}/.gitkeep \;
