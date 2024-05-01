ROOT_DIR:=.
INC_DIR=$(ROOT_DIR)/../include
SRC_DIR=$(ROOT_DIR)/../src

TEST?=fpu

ROUNDING?=1
ifeq ($(ROUNDING),1)
DEFINE+=-DROUNDING
endif

SUBNORMALS?=1
ifeq ($(SUBNORMALS),1)
DEFINE+=-DSUBNORMALS
endif

CC:=iverilog
CFLAGS:=-W all -g2005-sv

INCLUDE+=-I$(INC_DIR)

VHDR+=$(wildcard $(INC_DIR)/*.vh)

VSRC+=$(filter-out %_tb.v, $(wildcard $(SRC_DIR)/*.v))

all: clean sim

sim: $(TEST)
	./a.out
	if [ ! `pgrep -u $(USER) gtkwave` ]; then gtkwave *.vcd; fi &

fpu: $(VHDR) $(VSRC) $(SRC_DIR)/iob_ptfloat_fpu_tb.v
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFINE) $(VSRC) $(SRC_DIR)/iob_ptfloat_fpu_tb.v

sticky: $(INC_DIR)/iob_ptfloat_defs.vh $(SRC_DIR)/iob_sticky.v $(SRC_DIR)/iob_mask.v $(SRC_DIR)/iob_sticky_tb.v
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFINE) $^

unpack: $(INC_DIR)/iob_ptfloat_defs.vh $(SRC_DIR)/iob_ptfloat_unpack.v $(SRC_DIR)/iob_ptfloat_unpack_tb.v
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFINE) $^

clean:
	rm -f a.out *.vcd *~

.PHONY: all\
	sim\
	fpu sticky unpack\
	clean
