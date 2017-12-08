.PHONY: clean All

All:
	@echo "----------Building project:[ DreamBoy - Debug ]----------"
	@"$(MAKE)" -f  "DreamBoy.mk" && "$(MAKE)" -f  "DreamBoy.mk" PostBuild
clean:
	@echo "----------Cleaning project:[ DreamBoy - Debug ]----------"
	@"$(MAKE)" -f  "DreamBoy.mk" clean
