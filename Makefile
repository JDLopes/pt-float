PTFLOAT_DIR:=.
PTFLOAT_HW_DIR=$(PTFLOAT_DIR)/hardware
PTFLOAT_SIM_DIR=$(PTFLOAT_HW_DIR)/simulation
PTFLOAT_SW_DIR=$(PTFLOAT_DIR)/software

sw-test:
	make -C $(PTFLOAT_SW_DIR) test

sw-clean:
	make -C $(PTFLOAT_SW_DIR) clean-all

hw-sim:
	make -C $(PTFLOAT_SIM_DIR) all

hw-clean:
	make -C $(PTFLOAT_SIM_DIR) clean

clean: sw-clean hw-clean

.PHONY: sw-test sw-clean\
	hw-sim hw-clean\
	clean
