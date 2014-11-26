#
# AES SQLite Codec implementation
# (C) 2014 Ledger
# Based on botansqlite3 git@github.com:OlivierJG/botansqlite3.git
#

CC 		= cc

CPP 	= g++

AR		= ar

CFLAGS 	=

CXFLAGS =

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

clean:
		rm -rf *.o

test: 	re
		$(CPP) main.cpp libsecuresqlite.a -lcrypto
		rm test.sqlite
		./a.out
