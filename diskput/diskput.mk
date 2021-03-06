PROG = ../bin/diskput

SRCS = $(wildcard src/*.c)
HDRS = $(wildcard inc/*.h)
OBJS = $(patsubst src/%.c, %.o, $(SRCS))

$(PROG): $(SRCS)
	@mkdir -p ../bin
	@$(CC) $^ $(CFLAGS) $(LIBS) -o $@
	@echo "    Generate Program $(notdir $(PROG)) from $^"

.PHONY: clean
clean:
	@$(RM) -f $(OBJS) $(PROG)
	@echo "    Removing Program:   $(PROG)"
	@echo "    Removing Objects:   $(OBJS)"
