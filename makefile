VPATH=src:bin
flags=-std=gnu99
cc=clang

all:
	@echo "To build a specific file, do make filename."
	
-include $(patsubst %.o,bin/%.d,$(objs))

%: %.c
	$(cc) $(flags) $($*_flags) -o bin/$* src/$*.c
	$(cc) -MM $(flags) src/$*.c > bin/$*.d


clean:
	rm bin/*
