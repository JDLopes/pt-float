TEST_PTFLOAT?=0

# Search paths
INCLUDE+=-I$(SW_DIR)
INCLUDE+=-I$(SW_DIR)/pc
INCLUDE+=-I$(SW_DIR)/ieee

# Headers
HDR+=$(SW_DIR)/iob_ptfloat.h
HDR+=$(SW_DIR)/pc/ptfloat.h
HDR+=$(SW_DIR)/ieee/ieee754.h

# Sources
SRC+=$(SW_DIR)/pc/iob_ptfloat.c
SRC+=$(SW_DIR)/ieee/ieee754.c
SRC+=$(SW_DIR)/pc/ptfloat.cpp
ifeq ($(TEST_PTFLOAT),1)
SRC+=$(SW_DIR)/pc/ptfloat_tests.cpp
endif

PTFLOAT_OBJ:=$(SRC:.c=.o)
PTFLOAT_OBJ:=$(PTFLOAT_OBJ:.cpp=.o)

$(SW_DIR)/pc/ptfloat.a: $(PTFLOAT_OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(SW_DIR)/%.o: $(SW_DIR)/%.c
	$(CPP) -Wextra -O3 $(INCLUDE) -c $< -o $@

$(SW_DIR)/%.o: $(SW_DIR)/%.cpp
	$(CPP) -Wextra -O3 $(INCLUDE) -c $< -o $@

clean-ptfloat:
	rm -f $(SW_DIR)/pc/ptfloat.a $(SW_DIR)/pc/*.o $(SW_DIR)/ieee/*.o

.PHONY: clean-ptfloat
