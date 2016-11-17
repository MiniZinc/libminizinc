INSRC 		= intro modelling modelling2 predicates search \
		  search_zinc efficient datastructures sat keywords \
                  optiontypes
INTEXSRC 	= ${INSRC:%=%.tex}

MINIZINCFILES = apple banana

MAIN = main

TEXSRC = \
	${MAIN}.tex macros.tex mymacros.tex ${INTEXSRC}

PS2PDF	= ps2pdf
DVIPS	= dvips -K -Ppdf
DVIPDF	= dvipdf
BIBTEX	= bibtex
LATEX	= latex
TEXINDY = texindy
#TEXINDY = makeindex
GL	= perl bin/gl.perl

all: quick

# XXX the rules in Make.doc.common don't (yet) understand indexes, so we
# need to override those rules for the MiniZinc tutorial.
#
main.idx: $(wildcard *.tex)
	$(LATEX) main

main.ind: main.idx
	$(TEXINDY) main.idx

main.tex.in: main.tex.in.in ${INTEXSRC}
	perl bin/include.perl < main.tex.in.in | \
	perl bin/shorten.perl > main.tex.in

quick: main.tex
	$(LATEX) main
	$(TEXINDY) -M mystyle.xdy main.idx 
#	$(TEXINDY) main.idx 
	$(LATEX) main
	$(DVIPS) main.dvi
	$(PS2PDF) main.ps

%.tex: %.tex.in bin/gl.perl
	$(GL) < $< > $@

tex: $(INTEXSRC)

clean::
	rm -f main.warns main.out main.out.in
	rm -f *.aux *.lo[gftp] *.bbl *.blg *~ *.toc *.bak *.ilg *.ind *.idx *.idxx *.brf *.ps.bz2 *.pdf.bz2 *.top 
	rm -f *.obj *.exe *.manifest
	rm -f ${INTEXSRC} titles.tex.in
	rm -f main.tex main.tex.in

realclean:: clean
	rm -f main.ps main.dvi main.pdf main.zip main.bib
	rm -f *.mzn *.dzn *.zinc

veryclean:: realclean

