PLATFORM=macosx

TARGETS=mzn2fzn

GENERATE=yes

SOURCES=allocator ast context lexer.yy parser.tab  \
	print printer/Document printer/Line printer/PrettyPrinter printer \
	typecheck solver_interface/solver_interface solver_interface/cplex_interface

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
CXX=g++
CC=clang
#cplex options
CPLEXDIR = /usr/ilog/cplex121
CONCERTDIR = /usr/ilog/concert29
CCOPT = -m64 -O0 -fPIC -fexceptions -DNDEBUG -DIL_STD
CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
SYSTEM     = x86-64_debian4.0_4.1
LIBFORMAT  = static_pic
CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -pthread

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) $(CCLNFLAGS)
CXXFLAGS=-std=c++0x -Wall -g -Iinclude $(CCFLAGS)
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
	$(CXX) $^ $(CXXFLAGS) $(OUTPUTEXE)$@ 
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
