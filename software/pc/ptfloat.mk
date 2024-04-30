PTFLOAT_SW_DIR=$(PTFLOAT_DIR)/software

TEST?=0

# Search paths
INCLUDE+=-I$(PTFLOAT_SW_DIR)
INCLUDE+=-I$(PTFLOAT_SW_DIR)/pc
INCLUDE+=-I$(PTFLOAT_SW_DIR)/ieee

# Headers
HDR+=$(PTFLOAT_SW_DIR)/iob_ptfloat.h
HDR+=$(PTFLOAT_SW_DIR)/pc/ptfloat.h
HDR+=$(PTFLOAT_SW_DIR)/ieee/ieee754.h

# Sources
SRC+=$(PTFLOAT_SW_DIR)/pc/iob_ptfloat.c
SRC+=$(PTFLOAT_SW_DIR)/ieee/ieee754.c
SRC+=$(PTFLOAT_SW_DIR)/pc/ptfloat.cpp
ifeq ($(TEST_PTFLOAT),1)
SRC+=$(PTFLOAT_SW_DIR)/pc/ptfloat_tests.cpp
endif

PTFLOAT_OBJ:=$(SRC:.c=.o)
PTFLOAT_OBJ:=$(PTFLOAT_OBJ:.cpp=.o)

$(PTFLOAT_SW_DIR)/pc/ptfloat.a: $(PTFLOAT_OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(PTFLOAT_SW_DIR)/pc/%.o: $(PTFLOAT_SW_DIR)/pc/%.c
	$(CPP) -Wextra -O3 $(INCLUDE) -c $< -o $@

$(PTFLOAT_SW_DIR)/pc/%.o: $(PTFLOAT_SW_DIR)/pc/%.cpp
	$(CPP) -Wextra -O3 $(INCLUDE) -c $< -o $@

clean-ptfloat:
	rm -f $(PTFLOAT_SW_DIR)/pc/ptfloat.a $(PTFLOAT_SW_DIR)/pc/*.o

.PHONY: clean-ptfloat
