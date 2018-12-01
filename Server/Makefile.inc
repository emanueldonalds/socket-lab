# Makefile.inc - common definitions used by all makefiles

LINUX_LIBRT = -lrt
LINUX_LIBDL = -ldl
LINUX_LIBACL = -lacl
LINUX_LIBCRYPT = -lcrypt
LINUX_LIBCAP = -lcap

# "-Wextra" is a more descriptive synonym for "-W", but only
# available in more recent gcc versions

IMPL_CFLAGS = -std=c++11 \
		-D_XOPEN_SOURCE=700 \
	    -D_DEFAULT_SOURCE \
		-D_unix_ \
		-g  \
		-pedantic \
		-Wall \
		-W \
		-Wno-sign-compare \
		-Wno-unused-parameter \
		-fpermissive
		

CFLAGS = ${IMPL_CFLAGS}

IMPL_CXXFLAGS = -std=c++11 \
		-D_DEFAULT_SOURCE \
		-D_unix_ \
		-g  \
		-Wall \
		-fpermissive

CXXFLAGS = ${IMPL_CXXFLAGS}

# We seems to need to tell that we like c++
CC = g++

IMPL_THREAD_FLAGS = -pthread

IMPL_LDLIBS = -lm

LDLIBS = ${IMPL_LDLIBS}

RM = rm -f
