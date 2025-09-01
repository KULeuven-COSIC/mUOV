CC  ?= clang
LD  = $(CC)


CFLAGS   := -O3 $(CFLAGS) -std=c99 -Wall -Wextra -Wpedantic -fno-omit-frame-pointer
CXXFLAGS := -O3 $(CPPFLAGS) -Wall -Wextra -fno-exceptions -fno-rtti -nostdinc++
INCPATH  := -I/usr/local/include -I/opt/local/include -I/usr/include -Iunit_tests -Ibenchmark
LIBS     = -lcrypto

SRC_DIR  = ./

SRCS    :=  $(foreach dir, $(SRC_DIR), $(wildcard  $(dir)/*.c) )

SRCS_O         :=  $(SRCS:.c=.o)
SRCS_O_NOTDIR  :=  $(notdir $(SRCS_O))


OBJ = $(SRCS_O_NOTDIR)

EXE= muov-benchmark

.INTERMEDIATE:  $(OBJ)

.PHONY: all clean 

all: $(EXE)

%-benchmark: $(OBJ) main.o
	$(LD) $(LDFLAGS) $(LIBPATH) -o $@ $^ $(LIBS)

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCPATH) -c $<

clean:
	-rm -f *.o *-test *-benchmark massif.out.* *.req *.rsp PQCgenKAT_sign $(EXE);
