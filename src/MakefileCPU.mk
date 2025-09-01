CC  ?= clang
LD  = $(CC)


CFLAGS   := -O3 $(CFLAGS) -std=c99 -Wall -Wextra -Wpedantic -fno-omit-frame-pointer
CXXFLAGS := -O3 $(CPPFLAGS) -Wall -Wextra -fno-exceptions -fno-rtti -nostdinc++
INCPATH  := -I/usr/local/include -I/opt/local/include -I/usr/include -Iunit_tests -Ibenchmark
LIBS     = -lcrypto

SRC_DIR  = ./ 

SRCS    :=  $(foreach dir, $(SRC_DIR), $(wildcard  $(dir)/*.c) )

OBJ = $(SRCS:.c=.o)

EXE= muov-benchmark

.INTERMEDIATE:  $(OBJ)

.PHONY: all clean 

all: $(EXE)

%-benchmark: $(OBJ) main.o
	$(LD) $(LDFLAGS) $(LIBPATH) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCPATH) -c $< -o $@

clean:
	-rm -f *.o *-test *-benchmark massif.out.* *.req *.rsp PQCgenKAT_sign output_performance.txt $(EXE);
