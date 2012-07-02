/*
 * PrettyPrinter.h
 *
 *  Created on: 21 juin 2012
 *      Author: pwilke
 */

#ifndef PRETTYPRINTER_H_
#define PRETTYPRINTER_H_

#include <string>
#include <vector>
#include "../document/Document.h"
#include "../document/DocumentList.h"
#include "../document/StringDocument.h"
#include "../document/BreakPoint.h"
#include <iostream>
#include <sstream>
#include "Line.h"

class PrettyPrinter {
public:
	PrettyPrinter(int _maxwidth = 80, std::string _indentationBase = "    ");
	virtual ~PrettyPrinter() {}
	void print(Document* d, bool alignment, int startColAlignment,
			std::string before = "", std::string after = "");
	void printDocList(DocumentList* d, bool alignment, int startColAlignment,
			std::string before = "", std::string after = "");
	void printStringDoc(StringDocument* d, bool alignment,
			int startColAlignment, std::string before = "", std::string after =
					"");
	void simplify();

private:
	int maxwidth; //max width of lines, in rows
	std::string indentationBase; //could be "    " e.g. - Represents the style of indentation desired
	int currentLine;
	std::vector<Line> lines;
	void addLine(int indentation);
	static std::string printSpaces(int n);
	const std::vector<Line>& getLines() const;
	friend std::ostream& operator<<(std::ostream& os, const PrettyPrinter& pp);
};

#endif
