#
#  Main authors:
#    Guido Tack <guido.tack@monash.edu>
#
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/. */

PLATFORM=macosx

TARGETS=mzn2fzn

GENERATE=yes

SOURCES=allocator ast context lexer.yy parser.tab  \
	print printer/Document printer/Line printer/PrettyPrinter \
	typecheck

HEADERS=allocator ast context exception model parser parser.tab print type typecheck astiterator

GENERATED=include/minizinc/parser.tab.hh lib/parser.tab.cpp lib/lexer.yy.cpp

ifeq ($(PLATFORM),windows)
CXXFLAGS=/nologo /Ox /EHsc /DNDEBUG /MD /Iinclude
INPUTSRC=/Tp
OUTPUTOBJ=-c /Fo
OUTPUTEXE=/Fe
CXX=cl
OBJ=obj
EXE=.exe
else
CXX=clang++
CC=clang
CXXFLAGS=-std=c++11 -stdlib=libc++ -Wall -g -Iinclude
#CXXFLAGS=-Wall -g -O2 -DNDEBUG -Iinclude
INPUTSRC=
OUTPUTOBJ=-c -o 
OUTPUTEXE=-o 
OBJ=o
EXE=
endif


all: $(TARGETS:%=%$(EXE))

$(SOURCES:%=lib/%.$(OBJ)): %.$(OBJ): %.cpp
	$(CXX) $(CXXFLAGS) $(OUTPUTOBJ)$@ $(INPUTSRC)$<

%.$(OBJ): %.cpp
	$(CXX) $(CXXFLAGS) $(OUTPUTOBJ)$@ $<

ifeq ($(PLATFORM),windows)
mzn2fzn$(EXE): mzn2fzn.$(OBJ) $(SOURCES:%=lib/%.$(OBJ))
	$(CXX) $(CXXFLAGS) $(OUTPUTEXE)$@ $^
	mt.exe -nologo -manifest $@.manifest -outputresource:$@\;1
else
mzn2fzn$(EXE): mzn2fzn.$(OBJ) $(SOURCES:%=lib/%.$(OBJ))
	$(CXX) $(CXXFLAGS) $(OUTPUTEXE)$@ $^
endif

ifeq ($(GENERATE), yes)
lib/lexer.yy.cpp: lib/lexer.lxx include/minizinc/parser.tab.hh
	flex -o$@ $<

include/minizinc/parser.tab.hh lib/parser.tab.cpp: lib/parser.yxx include/minizinc/parser.hh
	bison --verbose -t -o lib/parser.tab.cpp --defines=include/minizinc/parser.tab.hh $<
endif

.PHONY: clean
clean:
	rm -f mzn2fzn.$(OBJ) $(SOURCES:%=lib/%.$(OBJ))

.PHONY: veryclean
veryclean: clean
	rm -f $(TARGETS:%=%$(EXE)) 
	rm -f $(GENERATED)

# Dependencies
mzn2fzn.$(OBJ): $(HEADERS:%=include/minizinc/%.hh)
