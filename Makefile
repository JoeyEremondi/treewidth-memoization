# ------------------------------------------------
# Generic Makefile
# Adapted from http://stackoverflow.com/questions/5820303/how-do-i-force-make-gcc-to-show-me-the-commands
# Author: yanick.rochon@gmail.com
# Date  : 2011-08-10
#
# Used by: Joey Eremondi, Utrecht University, November 2015
#
# Changelog :
#   2010-11-05 - first version
#   2011-08-10 - added structure : sources, objects, binaries
#                thanks to http://stackoverflow.com/users/128940/beta
# ------------------------------------------------

# project name (generate executable with this name)
TARGET   = test

CXX=clang++ -std=c++14 -I$(INCDIR)
# compiling flags here
CFLAGS   = -g -O2

LINKER   = clang++ -std=c++14 -o
# linking flags here
LDFLAGS   = -g -O2
LDLIBS=-lm -lboost_system -lboost_timer -lboost_filesystem


# change these to set the proper directories where each files shoould be
SRCDIR   = src
INCDIR   = include
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.cc)
#INCLUDES := $(wildcard $(INCDIR)/*.hh)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)
rm       = rm -f

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $@ $(LDFLAGS) $(LDLIBS) $(OBJECTS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cc
	@$(CXX) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONEY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"
