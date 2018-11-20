MAKE_DIR = $(PWD)

DISKUTIL_DIR	:= $(MAKE_DIR)/diskutil
DISKINFO_DIR	:= $(MAKE_DIR)/diskinfo
DISKLIST_DIR	:= $(MAKE_DIR)/disklist
DISKGET_DIR		:= $(MAKE_DIR)/diskget
DISKPUT_DIR		:= $(MAKE_DIR)/diskput

INC_SRCH_PATH := 
INC_SRCH_PATH += -I$(DISKUTIL_DIR)/inc
INC_SRCH_PATH += -I$(DISKINFO_DIR)/inc
INC_SRCH_PATH += -I$(DISKLIST_DIR)/inc
INC_SRCH_PATH += -I$(DISKGET_DIR)/inc
INC_SRCH_PATH += -I$(DISKPUT_DIR)/inc

LIB_SRCH_PATH :=
LIB_SRCH_PATH += -L$(MAKE_DIR)/libs
LIB_SRCH_PATH += -L$(MAKE_DIR)/libs


CC = gcc
LD = ld

LIBS :=

CFLAGS :=
CFLAGS += $(INC_SRCH_PATH) $(LIB_SRCH_PATH) 
CFLAGS += -Wall -g -Wall -Werror -ldiskutil
CFLAGS += -DDEBUG -D_REENTRANT

LDFLAGS :=

export MAKE_DIR CC LD CFLAGS LDFLAGS LIBS INC_SRCH_PATH

all:
	@$(MAKE) -C diskutil -f diskutil.mk
	@$(MAKE) -C diskinfo -f diskinfo.mk
	@$(MAKE) -C disklist -f disklist.mk
	@$(MAKE) -C diskget -f diskget.mk
	@$(MAKE) -C diskput -f diskput.mk

.PHONY: clean
clean:
	@$(MAKE) -C diskinfo -f diskinfo.mk clean
	@$(MAKE) -C disklist -f disklist.mk clean
	@$(MAKE) -C diskget -f diskget.mk clean
	@$(MAKE) -C diskput -f diskput.mk clean
