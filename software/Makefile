ROOT_DIR:=.
SW_DIR=$(ROOT_DIR)
PYTHON_DIR=$(SW_DIR)/python
TRG:=ptfloat

CPP=g++
AR=ar

ARFLAGS=rcs

ifneq ($(findstring test, $(MAKECMDGOALS)),)
TEST_PTFLOAT=1
endif

include $(SW_DIR)/pc/ptfloat.mk
COMMON+=$(INCLUDE)

DATA_W=$(shell grep "define DATA_W" $(SW_DIR)/iob_ptfloat.h | awk '{print $$3}')
EW_W=$(shell grep "define EW_W" $(SW_DIR)/iob_ptfloat.h | awk '{print $$3}')

test: clean $(TRG)
	./$(TRG) -i data.in -w weights.in -d ./$@-double.out -f ./$@-float.out -p ./$@-ptfloat.out
#	./$(TRG) -d ./$@-double.out -f ./$@-float.out -p ./$@-ptfloat.out
	$(PYTHON_DIR)/floatVSptfloat -d ./$@-double.out -f ./$@-float.out -o ./float-floatVSptfloat.out -p 16 -dw $(DATA_W) -eww $(EW_W)
	$(PYTHON_DIR)/floatVSptfloat -d ./$@-double.out -ptf ./$@-ptfloat.out -o ./ptfloat-floatVSptfloat.out -p 16 -dw $(DATA_W) -eww $(EW_W)

$(TRG): $(SW_DIR)/pc/$(TRG).a
	$(CPP) $^ -o $@

clean: clean-ptfloat
	rm -f $(TRG)
	rm -rf $(PYTHON_DIR)/__pycache__

clean-all: clean
	rm -f *.o *.out *.summary

.PHONY: test clean clean-all
