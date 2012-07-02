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
PrettyPrinter::PrettyPrinter(int _maxwidth, string _indentationBase) {
	maxwidth = _maxwidth;
	indentationBase = _indentationBase;
	addLine(0);
	currentLine = 0;
}
const std::vector<Line>& PrettyPrinter::getLines() const {
	return lines;
}

void PrettyPrinter::addLine(int indentation) {
	lines.push_back(Line(indentation));
	currentLine++;
}

std::ostream& operator<<(std::ostream& os, const PrettyPrinter& pp) {
	std::vector<Line>::const_iterator it;
	for (it = pp.getLines().begin(); it != pp.getLines().end(); it++) {
		os << (*it);
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

void PrettyPrinter::print(Document* d, bool alignment, int alignmentCol,
		string before, string after) {
	string s;
	if (DocumentList* dl = dynamic_cast<DocumentList*>(d)) {
		printDocList(dl, alignment, alignmentCol, before, after);
	} else if (StringDocument* sd = dynamic_cast<StringDocument*>(d)) {
		printStringDoc(sd, alignment, alignmentCol, before, after);
	} else if (dynamic_cast<BreakPoint*>(d)) {
		printStringDoc(NULL, alignment, alignmentCol, before, "");
		addLine(alignmentCol);
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
	Line& l = lines[currentLine];
	int size = s.size();
	if (size <= l.getSpaceLeft(maxwidth)) {
		l.addString(s);
	} else {
		int col =
				alignment && maxwidth - alignmentCol > size ?
						alignmentCol : indentationBase.size();
		addLine(col);
		lines[currentLine].addString(s);
	}

}

void PrettyPrinter::printDocList(DocumentList* d, bool alignment,
		int alignmentCol, std::string super_before, std::string super_after) {
	vector<Document*> ld = d->getDocs();
	string beginToken = d->getBeginToken();
	string separator = d->getSeparator();
	string endToken = d->getEndToken();
	bool _alignment = d->getAlignment();

	int currentCol = lines[currentLine].getIndentation()
			+ lines[currentLine].getLength();
	int newAlignmentCol =
			_alignment ? currentCol + beginToken.size() : alignmentCol;
	int vectorSize = ld.size();
	int lastVisibleElementIndex;
	for (int i = 0; i < vectorSize; i++) {
		if (!dynamic_cast<BreakPoint*>(ld[i]))
			lastVisibleElementIndex = i;
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
		print(subdoc, _alignment, newAlignmentCol, be, af);
	}

}

void PrettyPrinter::simplify() {
	int nLines = lines.size();
	for (int i = nLines - 1; i > 0; i--) {
		if (lines[i].getLength() > lines[i - 1].getSpaceLeft(maxwidth))
			break;
		else {
			lines[i - 1].concatenateLines(lines[i]);
			lines.pop_back();
		}
	}
}
