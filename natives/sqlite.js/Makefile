#
# SQLite JS bridge
# (C) 2014 Ledger
#

NACL_SDK_ROOT ?= $(abspath $(CURDIR)/../../nacl_sdk/pepper_37/)

CC 		= $(NACL_SDK_ROOT)/toolchain/mac_pnacl/bin/pnacl-clang

CPP 	= $(NACL_SDK_ROOT)/toolchain/mac_pnacl/bin/pnacl-clang++

AR		= $(NACL_SDK_ROOT)/toolchain/mac_pnacl/bin/pnacl-ar

FINALIZE = $(NACL_SDK_ROOT)/toolchain/mac_pnacl/bin/pnacl-finalize

include $(NACL_SDK_ROOT)/tools/common.mk

CXFLAGS = -I$(NACL_SDK_ROOT)/include -I$(NACL_SDK_ROOT)/include/pnacl -O2 -I../secure_sqlite/

LDFLAGS =  -L$(NACL_SDK_ROOT)/lib/pnacl/Release -lnacl_io -lppapi_simple -pthread -lppapi_cpp -lppapi -O2 $(DEPS) -L../openssl -L../secure_sqlite -lsecuresqlite -lcrypto

PEXE_NAME	= sqlite_bridge.pexe

FINAL_PEXE_NAME = sqlite_bridge.final.pexe

SRC_CXX = 	sqlite_bridge.cpp sqlite_commands.cpp

OBJ_CXX = $(SRC_CXX:.cpp=.o)

.cpp.o:
	$(CPP) -o $@ -c $< $(CXFLAGS)

all: build_cpp link finalize

build_cpp: $(OBJ_CXX)

link:
	$(CPP) $(OBJ_CXX) $(LDFLAGS) -o $(PEXE_NAME)

re: clean all

finalize:
	$(FINALIZE) $(PEXE_NAME) -o $(FINAL_PEXE_NAME)
	cp $(FINAL_PEXE_NAME) ../../app/public/sqlite_bridge/

