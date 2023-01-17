#!/bin/bash

BUILD_DIR=$(realpath $(dirname $0))/../build

SOURCE_DIR=$(realpath $(dirname $0))

cmake -B${BUILD_DIR} -G "Eclipse CDT4 - Unix Makefiles" -H${SOURCE_DIR}