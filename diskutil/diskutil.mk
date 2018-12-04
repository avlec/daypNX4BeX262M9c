PROG = ../libs/diskutil

SRCS = $(wildcard src/*.c)
HDRS = $(wildcard inc/*.h)
OBJS = $(patsubst src/%.c, %.o, $(SRCS))

$(PROG): $(SRCS) $(HDRS)
	@mkdir -p ../libs
	@$(foreach var,$(SRCS), $(CC) -c -fpic $(var) $(CFLAGS) -o $(var).o;)
	$(CC) -shared -o ../libs/libdiskutil.so $(foreach var,$(SRCS), $(var).o)
	rm $(foreach var, $(SRCS), $(var).o)
	@echo "    Generate Program $(notdir $(PROG)) from $^"

.PHONY: clean
clean:
	@$(RM) -f $(OBJS) $(PROG)
	@echo "    Removing Program:   $(PROG)"
	@echo "    Removing Objects:   $(OBJS)"
