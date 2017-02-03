PROG := genplatform
SRCS := $(wildcard *.cpp)
PNVMSRCS := $(wildcard ../src/*.cpp)
OBJS := $(SRCS:.cpp=.o)
PNVMOBJS := $(patsubst ../src/%,%, $(PNVMSRCS:.cpp=.o))
DEBUG ?= 1
CPPFLAGS := -Wall -Wextra -I../includes -I.
CPPFLAGS += $(shell pkg-config --cflags liblog4cxx) -pthread
LDFLAGS := -pthread
PNVMLIB := pnvm.a
LIBS := -lrt -lboost_program_options -lboost_system \
    -lm $(shell pkg-config --libs liblog4cxx)

ifeq ($(DEBUG), 1)
    CPPFLAGS += -g -ggdb3 -DDEBUG -Werror
else 
    CPPFLAGS += -O2 -DDEBUG
endif

.PHONY: all ctags clean clean-all

all: $(PROG)

$(PROG): $(OBJS) $(PNVMLIB)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

$(PNVMLIB): $(PNVMOBJS)
	$(AR) rcs $@ $?

%_plugin.o: ../src/%_plugin.cpp
	$(COMPILE.cpp) -Wno-unused $? -o $@

%.o: ../src/%.cpp
	$(COMPILE.cpp) $? -o $@

ctags:
	ctags -R . ../includes ../src

clean:
	-rm *.o $(PROG) $(PNVMLIB)

clean-all: clean
	-rm in_pins out_pins input_messages
