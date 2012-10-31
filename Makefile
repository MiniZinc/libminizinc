PLATFORM=macosx

TARGETS=mzn2fzn

GENERATE=yes

SOURCES=allocator ast context lexer.yy parser.tab  \
	prettyprinter typecheck eval_par builtins

HEADERS=allocator ast context exception model parser parser.tab type typecheck prettyprinter eval_par builtins hash

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
CXXFLAGS=-std=c++0x -stdlib=libc++ -Wall -g -Iinclude
#CXXFLAGS=-Wall -g -O2 -DNDEBUG -Iinclude
OUTPUTOBJ=-c -o 
OUTPUTEXE=-o 
OBJ=o
EXE=
endif


all: $(TARGETS:%=%$(EXE))

$(SOURCES:%=lib/%.$(OBJ)): %.$(OBJ): %.cpp 
	$(CXX) $(CXXFLAGS) $(OUTPUTOBJ)$@ $<

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
	bison -o lib/parser.tab.cpp --defines=include/minizinc/parser.tab.hh $<
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
