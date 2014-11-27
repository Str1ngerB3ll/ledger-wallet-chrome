#
# AES SQLite Codec implementation
# (C) 2014 Ledger
# Based on botansqlite3 git@github.com:OlivierJG/botansqlite3.git
#

NACL_SDK_ROOT ?= $(abspath $(CURDIR)/../../nacl_sdk/pepper_37/)

CC 		= ../../nacl_sdk/pepper_37/toolchain/mac_pnacl/bin/pnacl-clang

CPP 	= ../../nacl_sdk/pepper_37/toolchain/mac_pnacl/bin/pnacl-clang++

AR		= ../../nacl_sdk/pepper_37/toolchain/mac_pnacl/bin/pnacl-ar


include $(NACL_SDK_ROOT)/tools/common.mk

CFLAGS 	= -I$(NACL_SDK_ROOT)include -I$(NACL_SDK_ROOT)include/sys -DPPAPI

CXFLAGS = -I$(NACL_SDK_ROOT)include -lnacl_io -lppapi_simple -lnacl_io -pthread -lppapi_cpp -lppapi

LIBNAME	= libsecuresqlite.a

SRC 	= 	codecext.c

SRC_CXX = 	codec.cpp			\
			aes_codec.cpp		\

OBJ 	= $(SRC:.c=.o)

OBJ_CXX = $(SRC_CXX:.cpp=.o)

.c.o:
	$(CC) -o $@ -c $< $(CFLAGS)

.cpp.o:
	$(CPP) -o $@ -c $< $(CXFLAGS)

all: 	build_c build_cpp build_lib

build_c: $(OBJ)

build_cpp: $(OBJ_CXX)

build_lib:
	$(AR) rcs $(LIBNAME) $(OBJ) $(OBJ_CXX)

re: clean all

