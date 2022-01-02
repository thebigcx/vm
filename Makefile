all:
	@$(MAKE) -C vm
	@$(MAKE) -C asm

clean:
	@$(MAKE) -C vm clean
	@$(MAKE) -C asm clean
	@rmdir dist
