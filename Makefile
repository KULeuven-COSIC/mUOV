.PHONY: all clean

DIRS=libopencm3
COMMONDIR=common

all clean:
	for dir in $(DIRS); do \
		$(MAKE) -C $$dir $@ ; \
	done; \
	if [ "$@" = "clean" ]; then \
		rm $(COMMONDIR)/*.o $(COMMONDIR)/*.d ; \
	fi
