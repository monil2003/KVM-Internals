.PHONY: all clean

all:
	@for dir in kvm-hypercalls vm-scheduling; do \
		$(MAKE) -C $$dir; \
	done

clean:
	@for dir in kvm-hypercalls vm-scheduling; do \
		$(MAKE) -C $$dir clean; \
	done