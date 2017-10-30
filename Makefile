.PHONY: clean All

All:
	@echo "----------Building project:[ EksamenC - Debug ]----------"
	@"$(MAKE)" -f  "EksamenC.mk"
clean:
	@echo "----------Cleaning project:[ EksamenC - Debug ]----------"
	@"$(MAKE)" -f  "EksamenC.mk" clean
