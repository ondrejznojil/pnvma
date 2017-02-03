SRCS := $(wildcard *.txt)
DSTS := $(SRCS:.txt=.h)

all: $(DSTS)

%.h: %.txt
	../../tools/genplatform -i $? -o $@

clean:
	-rm *.h
