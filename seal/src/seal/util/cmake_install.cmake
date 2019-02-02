# Install script for directory: /home/edouard/Git/SEAL/src/seal/util

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/seal/util" TYPE FILE FILES
    "/home/edouard/Git/SEAL/src/seal/util/aes.h"
    "/home/edouard/Git/SEAL/src/seal/util/baseconverter.h"
    "/home/edouard/Git/SEAL/src/seal/util/clang.h"
    "/home/edouard/Git/SEAL/src/seal/util/clipnormal.h"
    "/home/edouard/Git/SEAL/src/seal/util/common.h"
    "/home/edouard/Git/SEAL/src/seal/util/config.h"
    "/home/edouard/Git/SEAL/src/seal/util/defines.h"
    "/home/edouard/Git/SEAL/src/seal/util/gcc.h"
    "/home/edouard/Git/SEAL/src/seal/util/globals.h"
    "/home/edouard/Git/SEAL/src/seal/util/hash.h"
    "/home/edouard/Git/SEAL/src/seal/util/hestdparms.h"
    "/home/edouard/Git/SEAL/src/seal/util/locks.h"
    "/home/edouard/Git/SEAL/src/seal/util/mempool.h"
    "/home/edouard/Git/SEAL/src/seal/util/msvc.h"
    "/home/edouard/Git/SEAL/src/seal/util/numth.h"
    "/home/edouard/Git/SEAL/src/seal/util/pointer.h"
    "/home/edouard/Git/SEAL/src/seal/util/polyarith.h"
    "/home/edouard/Git/SEAL/src/seal/util/polyarithmod.h"
    "/home/edouard/Git/SEAL/src/seal/util/polyarithsmallmod.h"
    "/home/edouard/Git/SEAL/src/seal/util/polycore.h"
    "/home/edouard/Git/SEAL/src/seal/util/randomtostd.h"
    "/home/edouard/Git/SEAL/src/seal/util/smallntt.h"
    "/home/edouard/Git/SEAL/src/seal/util/uintarith.h"
    "/home/edouard/Git/SEAL/src/seal/util/uintarithmod.h"
    "/home/edouard/Git/SEAL/src/seal/util/uintarithsmallmod.h"
    "/home/edouard/Git/SEAL/src/seal/util/uintcore.h"
    )
endif()

