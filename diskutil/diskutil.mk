PROG = ../libs/diskutil

SRCS = $(wildcard src/*.c)
HDRS = $(wildcard inc/*.h)
OBJS = $(patsubst src/%.c, %.o, $(SRCS))

$(PROG): $(SRCS)
	@mkdir -p ../libs
	@$(CC) -c -fpic $^ $(CFLAGS) -o $@.o
	@$(CC) -shared -o ../libs/libdiskutil.so $@.o
	@rm $@.o
	@echo "    Generate Program $(notdir $(PROG)) from $^"

.PHONY: clean
clean:
	@$(RM) -f $(OBJS) $(PROG)
	@echo "    Removing Program:   $(PROG)"
	@echo "    Removing Objects:   $(OBJS)"
