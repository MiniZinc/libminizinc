PLATFORM=macosx

TARGETS=mzn2fzn

GENERATE=yes

SOURCES=allocator lexer.yy parser.tab ast print printer/DocumentList printer/Line printer/PrettyPrinter

HEADERS=allocator ast context model parser parser.tab print

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
CXXFLAGS=-Wall -g -Iinclude
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
	bison -t -o lib/parser.tab.cpp --defines=include/minizinc/parser.tab.hh $<
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
