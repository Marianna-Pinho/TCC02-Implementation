
#Name of the project
PROJ_NAME = colector

# .c files. Wildcard takes all files which match ".c" pattern
C_SOURCE = $(wildcard ./src/*.c)

# .h files.
H_SOURCE = $(wildcard ./src/*.h)

#Object files. As objects don't exist yet, we replace .c by .o. Object names will be the same of .c files.
#OBJ = $(C_SOURCE: .c = .o)
OBJ=$(subst .c,.o,$(subst src,objects,$(C_SOURCE)))

#Compiler
CC = gcc
CC_CROSS = arm-linux-gnueabihf-gcc

#Arquivo main
MAIN = main_code

#Flags for compiler
CC_FLAGS = -c        \
           -W        \
           -Wall     \
           -pedantic 

#Library flags
LDFLAGS = -lm -pthread -l json-c -lcurl
#
# Compilation  and linking
#

all:
	$(CC) -o ./src/$(PROJ_NAME) $(C_SOURCE) $(LDFLAGS)

clean:
	rm -rf ./src/$(PROJ_NAME) *~
