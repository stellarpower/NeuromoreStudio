# Platforms directory
PLATDIR = $(dir $(lastword $(MAKEFILE_LIST)))

# Include shared for host os
include $(PLATDIR)/osx-all.mk

# Android Specific
ANDROID_API         = 21
ANDROID_SYSROOT     = $(ANDROID_NDK_HOME)/platforms/android-$(ANDROID_API)/arch-x86_64
ANDROID_TOOLCHAIN   = $(ANDROID_NDK_HOME)/toolchains/llvm/prebuilt/darwin-x86_64

# Generic
EXTBIN     =
EXTLIB     = .a
EXTDLL     = .so
EXTPDB     = .pdb
OBJDIR     = obj/android-x64-$(MODE)
LIBDIR     = lib/android-x64
BINDIR     = bin/android-x64
TARGET     = x86_64-linux-android
CPUFLAGS   = -msse -msse2 -msse3 -msse4.1 -msse4.2 -mpclmul
DEFINES    = -DANDROID -D__ANDROID_API__=$(ANDROID_API)
INCLUDES   = -I$(ANDROID_NDK_HOME)/sources/android/cpufeatures
CXX        = $(ANDROID_TOOLCHAIN)/bin/$(TARGET)$(ANDROID_API)-clang++
CXXFLAGS   = -static \
             -target $(TARGET) \
             -isystem $(ANDROID_NDK_HOME)/sysroot/usr/include/$(TARGET)
CC         = $(ANDROID_TOOLCHAIN)/bin/$(TARGET)$(ANDROID_API)-clang
CFLAGS     = -static \
             -target $(TARGET) \
             -isystem $(ANDROID_NDK_HOME)/sysroot/usr/include/$(TARGET)
AR         = $(ANDROID_TOOLCHAIN)/bin/$(TARGET)-ar
ARFLAGS    = rcs
STRIP      = $(ANDROID_TOOLCHAIN)/bin/$(TARGET)-strip
STRIPFLAGS = --strip-all
LINK       = $(CXX)
LINKFLAGS  = -target $(TARGET) -fuse-ld=lld -static-libstdc++ -static-libgcc --sysroot=$(ANDROID_SYSROOT)
LINKPATH   = -L$(LIBDIR) \
             -L$(ANDROID_SYSROOT)/usr/lib \
             -L$(ANDROID_TOOLCHAIN)/sysroot/usr/lib/$(TARGET)/$(ANDROID_API) \
             -L$(ANDROID_TOOLCHAIN)/sysroot/usr/lib/$(TARGET)
LINKLIBS   = 

# Debug vs. Release
ifeq ($(MODE),release)
DEFINES   := $(DEFINES) -DNDEBUG
CXXFLAGS  := $(CXXFLAGS) -flto -O3 -g -ffunction-sections -fdata-sections
CFLAGS    := $(CFLAGS) -flto -O3 -g -ffunction-sections -fdata-sections
LINKFLAGS := $(LINKFLAGS) -flto -g -Wl,--gc-sections
else
DEFINES   := $(DEFINES) -D_DEBUG
CXXFLAGS  := $(CXXFLAGS) -Og -g3
CFLAGS    := $(CFLAGS) -Og -g3
LINKFLAGS := $(LINKFLAGS) -g3
endif