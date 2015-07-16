CXX				= clang++
CXXFLAGS	:= -g -c -std=c++11

DEPSDIR			= deps
SRCDIR			= src
TARGETDIR		= bin
TARGETNAME 	= tileserver
BUILDDIR		= build
INTERMEDIATESDIR = $(BUILDDIR)/intermediates

LOCALINCDIR 		= $(BUILDDIR)/include
TARGETHEADERDIR = $(LOCALINCDIR)/$(TARGETNAME)
INCDIRS		=	 /usr/local/include $(LOCALINCDIR)
INCS 			:= $(foreach d, $(INCDIRS), -I$d) $(shell mapnik-config --includes --dep-includes) \
							-I/usr/local/include/r3
LDLIBS		:= $(shell mapnik-config --libs --ldflags --dep-libs) \
							-lproxygenhttpserver -lfolly -lglog -lgflags -pthread \
							-lr3
LDFLAGS		:= -g -std=c++11 $(LDLIBS)

SOURCES := $(shell find $(SRCDIR) -type f -name *.cpp)
HEADERS := $(shell find $(SRCDIR) -type f -name *.h)
OBJECTS := $(patsubst $(SRCDIR)/%,$(INTERMEDIATESDIR)/%,$(SOURCES:.cpp=.o))
HDEPS 	:= $(OBJECTS:.o=.d)
TARGETHEADERS := $(patsubst $(SRCDIR)/%,$(TARGETHEADERDIR)/%,$(HEADERS))

all: checkouts $(TARGETDIR)/$(TARGETNAME)

checkouts:
	@echo "Checking out dependencies."
	@git submodule update --init

$(TARGETDIR)/$(TARGETNAME): $(OBJECTS)
	@echo "Linking $(TARGETNAME)"
	@echo "$(LDFLAGS)"
	@$(CXX) $(LDFLAGS) $(OBJECTS) -o $@ 

-include $(HDEPS)

$(INTERMEDIATESDIR)/%.o: $(SRCDIR)/%.cpp $(TARGETHEADERS)
	@echo "Compiling $<"
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) $(INCS) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	@$(CXX) $(CXXFLAGS) $(INCS) -c -o $@ $<

$(TARGETHEADERDIR)/%.h: $(SRCDIR)/%.h
	@mkdir -p $(@D)
	@cp $^ $(@D)

.SECONDARY: $(TARGETHEADERS)
.PHONY: clean

clean:
	@rm -rf $(BUILDDIR) $(TARGETDIR)/$(TARGETNAME)
