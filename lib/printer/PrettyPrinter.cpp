/*
 * PrettyPrinter.cpp
 *
 *  Created on: 21 juin 2012
 *      Author: pwilke
 */

#include <printer/PrettyPrinter.h>
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
	if (simp)
		simplifyItem(currentItem);
}

PrettyPrinter::PrettyPrinter(int _maxwidth, int _indentationBase, bool sim,
		bool deepsim) {
	maxwidth = _maxwidth;
	indentationBase = _indentationBase;
	currentLine = -1;
	currentItem = -1;

	simp = sim;
	deeplySimp = deepsim;
}
const std::vector<Line>& PrettyPrinter::getCurrentItemLines() const {
	return items[currentItem];
}

void PrettyPrinter::addLine(int indentation, bool bp, int level) {
	items[currentItem].push_back(Line(indentation));
	currentLine++;
	if (bp && deeplySimp) {
		linesToSimplify[currentItem].addLine(level, currentLine);
	}
}
void PrettyPrinter::addItem() {
	items.push_back(std::vector<Line>());
	linesToSimplify.push_back(LinesToSimplify());
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
		const std::string& before, const std::string& after) {
	string s;
	if (DocumentList* dl = dynamic_cast<DocumentList*>(d)) {
		printDocList(dl, alignment, alignmentCol, before, after);
	} else if (StringDocument* sd = dynamic_cast<StringDocument*>(d)) {
		printStringDoc(sd, alignment, alignmentCol, before, after);
	} else if (dynamic_cast<BreakPoint*>(d)) {
		printString(before, alignment, alignmentCol);
		addLine(alignmentCol, deeplySimp
		/*&& !((DocumentList*) (d->getParent()))->getDontSimplify()*/,
				d->getLevel());
		printString(after, alignment, alignmentCol);
	} else {
		cerr << "PrettyPrinter::print : Wrong type of document" << endl;
		exit(0);
	}
}

void PrettyPrinter::printStringDoc(StringDocument* d, bool alignment,
		int alignmentCol, const std::string& before, const std::string& after) {
	string s;
	if (d != NULL)
		s = d->getString();
	s = before + s + after;
	printString(s, alignment, alignmentCol);

}

void PrettyPrinter::printString(const std::string& s, bool alignment,
		int alignmentCol) {
	Line& l = items[currentItem][currentLine];
	int size = s.size();
	if (size <= l.getSpaceLeft(maxwidth)) {
		l.addString(s);
	} else {
		int col =
				alignment && maxwidth - alignmentCol >= size ?
						alignmentCol : indentationBase;
		addLine(col);
		items[currentItem][currentLine].addString(s);
	}
}

void PrettyPrinter::printDocList(DocumentList* d, bool alignment,
		int alignmentCol, const std::string& super_before,
		const std::string& super_after) {
	vector<Document*> ld = d->getDocs();
	string beginToken = d->getBeginToken();
	string separator = d->getSeparator();
	string endToken = d->getEndToken();
	bool _alignment = d->getAlignment();
	if (d->getUnbreakable()) {
		addLine(alignmentCol);
	}
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
				newAlignmentCol += indentationBase;
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
	if (d->getUnbreakable()) {
		simplify(currentItem, currentLine, NULL);
	}

}
void showVector(std::vector<int>* vec) {
	if (vec != NULL) {
		std::vector<int>::iterator it;
		for (it = vec->begin(); it != vec->end(); it++) {
			std::cout << *it << " ";
		}
		std::cout << std::endl;
	}
}
void PrettyPrinter::simplifyItem(int item) {
	std::vector<int>* vec = (linesToSimplify[item].getLinesToSimplify());
	while (!vec->empty()) {
		if (!simplify(item, (*vec)[0], vec))
			break;
	}
}

bool PrettyPrinter::simplify(int item, int line, std::vector<int>* vec) {

	if (line == 0) {
		linesToSimplify[item].remove(vec, line, false);
		return false;
	}
	if (items[item][line].getLength()
			> items[item][line - 1].getSpaceLeft(maxwidth)) {
		linesToSimplify[item].remove(vec, line, false);
		return false;
	} else {
		linesToSimplify[item].remove(vec, line, true);
		items[item][line - 1].concatenateLines(items[item][line]);
		items[item].erase(items[item].begin() + line);

		linesToSimplify[item].decrementLine(vec, line);
		currentLine--;
	}

	return true;
}
