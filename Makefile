.PHONY: all
all:
	cmake -H. -BBuild
	$(MAKE) -C Build
clean:
	rm -rf Build
