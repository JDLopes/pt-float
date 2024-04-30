PTFLOAT_DIR:=.
PTFLOAT_SW_DIR=$(PTFLOAT_DIR)/software
PTFLOAT_PYTHON_DIR=$(PTFLOAT_SW_DIR)/python
PTFLOAT_TRG:=ptfloat

CPP=g++
AR=ar

ifneq ($(findstring test, $(MAKECMDGOALS)),)
TEST_PTFLOAT=1
endif

include $(PTFLOAT_SW_DIR)/pc/ptfloat.mk
COMMON+=$(INCLUDE)

DATA_W=$(shell grep "define DATA_W" $(PTFLOAT_SW_DIR)/iob_ptfloat.h | awk '{print $$3}')
EW_W=$(shell grep "define EW_W" $(PTFLOAT_SW_DIR)/iob_ptfloat.h | awk '{print $$3}')

test: sw-clean $(PTFLOAT_TRG)
	./$(PTFLOAT_TRG) -i data.in -w weights.in -d ./$@-double.out -f ./$@-float.out -ptf ./$@-ptfloat.out
#	./$(PTFLOAT_TRG) -d ./$@-double.out -f ./$@-float.out -ptf ./$@-ptfloat.out
	$(PTFLOAT_PYTHON_DIR)/floatVSptfloat -d ./$@-double.out -f ./$@-float.out -o ./float-floatVSptfloat.out -p 16 -dw $(DATA_W) -eww $(EW_W)
	$(PTFLOAT_PYTHON_DIR)/floatVSptfloat -d ./$@-double.out -ptf ./$@-ptfloat.out -o ./ptfloat-floatVSptfloat.out -p 16 -dw $(DATA_W) -eww $(EW_W)

$(PTFLOAT_TRG): $(PTFLOAT_SW_DIR)/pc/$(PTFLOAT_TRG).a
	$(CPP) $^ -o $@

sw-clean: clean-ptfloat
	rm -f $(PTFLOAT_TRG)

.PHONY: sw-clean test
