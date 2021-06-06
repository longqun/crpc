SUBDIRS = SUB_DIR_NAME_1 SUB_DIR_NAME_2
TESTDIRS = test

.PHONY: all
all:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
		echo "Clean in $$subdir";\
		cd $$subdir && $(MAKE);\
	done

.PHONY: test
test:
	@list='$(TESTDIRS)'; for subdir in $$list; do \
		echo "Clean in $$subdir";\
		cd $$subdir && $(MAKE) test;\
	done


.PHONY: clean
clean:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
		echo "Clean in $$subdir";\
		cd $$subdir && $(MAKE) clean;\
	done
