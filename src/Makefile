PLUGIN := pnvm_plugin
C_HEADERS := ../includes/$(PLUGIN).h
C_SOURCES := $(PLUGIN).cpp
INCLUDES := ../includes
CLEANER := clean_plugin.py

all: $(C_HEADERS) $(C_SOURCES)

$(PLUGIN).cpp: $(PLUGIN)_cleaned.c
	if which indent 2>/dev/null && \
		indent --version | grep -q '^GNU\s\+indent'; \
	then \
	    indent -l80 -kr -bfda -nut -ip8 -di8 $? -o $@; \
	else \
	    cp $? $@; \
	fi

$(PLUGIN)_cleaned.c $(C_HEADERS): $(PLUGIN)_generated.c $(CLEANER)
	python $(CLEANER) --header-file ../includes/$(PLUGIN).h \
	    $(PLUGIN)_generated.c \
	    $(PLUGIN)_cleaned.c

lint:
	pylint $(CLEANER)

clean:
	-rm $(PLUGIN)_cleaned.c

