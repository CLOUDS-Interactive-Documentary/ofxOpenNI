#!/bin/sh
#set -x

BASE=${PWD}/
BASE_INSTALL=${BASE}../OpenNI_openFrameworks/
LIB_INSTALL_PATH=${BASE_INSTALL}openni/lib/
CONFIG_INSTALL_PATH=${BASE_INSTALL}/openni/config/

# Copy the libraries to their correct locations
# -----------------------------------------------
cp ${BASE}/Bin/*.dylib ${LIB_INSTALL_PATH}
cp ${BASE}/XnVFeatures/Bin/*.dylib ${LIB_INSTALL_PATH}
cp ${BASE}/XnVHandGenerator/Bin/*.dylib ${LIB_INSTALL_PATH}

# Copy the configuration files
# ------------------------------
cp ${BASE}/XnVFeatures/Data/* ${CONFIG_INSTALL_PATH}
cp ${BASE}/XnVHandGenerator/Data/* ${CONFIG_INSTALL_PATH}

# Change dynamic linker settings
cd ${LIB_INSTALL_PATH}

