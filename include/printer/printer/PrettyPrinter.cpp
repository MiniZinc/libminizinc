/*
 * PrettyPrinter.cpp
 *
 *  Created on: 21 juin 2012
 *      Author: pwilke
 */

#include "PrettyPrinter.h"
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>

using namespace std;

void PrettyPrinter::print(Document* d) {
	addItem();
	addLine(0);
	printDocument(d, true, 0);
	simplifyItem(currentItem);
}

PrettyPrinter::PrettyPrinter(int _maxwidth, string _indentationBase) {
	maxwidth = _maxwidth;
	indentationBase = _indentationBase;
	currentLine = -1;
	currentItem = -1;
}
const std::vector<Line>& PrettyPrinter::getCurrentItemLines() const {
	return items[currentItem];
}

void PrettyPrinter::addLine(int indentation, bool bp) {
	items[currentItem].push_back(Line(indentation));
	currentLine++;
	if (bp)
		linesToSimplify[currentItem].push_back(currentLine);
}
void PrettyPrinter::addItem() {
	items.push_back(std::vector<Line>());
	linesToSimplify.push_back(std::vector<int>());
	currentItem++;
	currentLine = -1;
}

std::ostream& operator<<(std::ostream& os, const PrettyPrinter& pp) {
	std::vector<Line>::const_iterator it;
	int nItems = pp.items.size();
	for (int item = 0; item < nItems; item++) {
		for (it = pp.items[item].begin(); it != pp.items[item].end(); it++) {
			os << (*it);
		}
		os << endl;
	}
	return os;
}
string PrettyPrinter::printSpaces(int n) {
	string result;
	for (int i = 0; i < n; i++) {
		result += " ";
	}
	return result;
}

void PrettyPrinter::printDocument(Document* d, bool alignment, int alignmentCol,
		string before, string after) {
	string s;
	if (DocumentList* dl = dynamic_cast<DocumentList*>(d)) {
		printDocList(dl, alignment, alignmentCol, before, after);
	} else if (StringDocument* sd = dynamic_cast<StringDocument*>(d)) {
		printStringDoc(sd, alignment, alignmentCol, before, after);
	} else if (dynamic_cast<BreakPoint*>(d)) {
		printStringDoc(NULL, alignment, alignmentCol, before, "");
		addLine(alignmentCol, true);
		printStringDoc(NULL, alignment, alignmentCol, "", after);
	} else {
		cerr << "PrettyPrinter::print : Wrong type of document" << endl;
		exit(0);
	}
}

void PrettyPrinter::printStringDoc(StringDocument* d, bool alignment,
		int alignmentCol, std::string before, std::string after) {
	string s;
	if (d != NULL)
		s = d->getString();
	s = before + s + after;
	Line& l = items[currentItem][currentLine];
	int size = s.size();
	if (size <= l.getSpaceLeft(maxwidth)) {
		l.addString(s);
	} else {
		int col =
				alignment && maxwidth - alignmentCol >= size ?
						alignmentCol : indentationBase.size();
		addLine(col);
		items[currentItem][currentLine].addString(s);
	}

}

void PrettyPrinter::printDocList(DocumentList* d, bool alignment,
		int alignmentCol, std::string super_before, std::string super_after) {
	vector<Document*> ld = d->getDocs();
	string beginToken = d->getBeginToken();
	string separator = d->getSeparator();
	string endToken = d->getEndToken();
	bool _alignment = d->getAlignment();

	int currentCol = items[currentItem][currentLine].getIndentation()
			+ items[currentItem][currentLine].getLength();
	int newAlignmentCol =
			_alignment ? currentCol + beginToken.size() : alignmentCol;
	int vectorSize = ld.size();
	int lastVisibleElementIndex;
	for (int i = 0; i < vectorSize; i++) {
		if (!dynamic_cast<BreakPoint*>(ld[i]))
			lastVisibleElementIndex = i;
	}
	if (vectorSize == 0) {
		printStringDoc(NULL, true, newAlignmentCol, super_before + beginToken,
				endToken + super_after);
	}
	for (int i = 0; i < vectorSize; i++) {
		Document* subdoc = ld[i];
		bool bp = false;
		if (dynamic_cast<BreakPoint*>(subdoc)) {
			if (!_alignment)
				newAlignmentCol += indentationBase.size();
			bp = true;
		}
		string af, be;
		if (i != vectorSize - 1) {
			if (bp || lastVisibleElementIndex <= i)
				af = "";
			else
				af = separator;
		} else {
			af = endToken + super_after;
		}
		if (i == 0) {
			be = super_before + beginToken;
		} else {
			be = "";
		}
		printDocument(subdoc, _alignment, newAlignmentCol, be, af);
	}

}

void PrettyPrinter::simplifyItem(int item) {
	int nLines = items[item].size();
	linesToSimplify[item].push_back(nLines - 1);
	int nLinesToSimplify = linesToSimplify[item].size();
	for (int l = nLinesToSimplify - 1; l > 0; l--) {
		for (int line = linesToSimplify[item][l]; line > 0; line--) {
			if (items[item][line].getLength()
					> items[item][line - 1].getSpaceLeft(maxwidth))
				break;
			else {
				items[item][line - 1].concatenateLines(items[item][line]);
				items[item].erase(items[item].begin() + line);
				//replace line by line - 1 in linesToSimplify[item]
				replace(linesToSimplify[item].begin(),
						linesToSimplify[item].end(), line, line - 1);
			}
		}
		linesToSimplify[item].pop_back();
	}
}

