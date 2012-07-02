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
#include <iostream>
#include <sstream>

#include "../document/Document.h"
#include "Line.h"


class PrettyPrinter {
public:
	PrettyPrinter(int _maxwidth = 80, std::string _indentationBase = "    ");

	void print(Document* d);

	virtual ~PrettyPrinter() {
	}


private:
	int maxwidth; //max width of lines, in rows
	std::string indentationBase; //could be "    " e.g. - Represents the style of indentation desired
	int currentLine;
	int currentItem;
	std::vector<std::vector<Line> > items;

	void addItem();
	void addLine(int indentation);
	static std::string printSpaces(int n);
	const std::vector<Line>& getCurrentItemLines() const;

	void printDocument(Document* d, bool alignment, int startColAlignment,
			std::string before = "", std::string after = "");
	void printDocList(DocumentList* d, bool alignment, int startColAlignment,
			std::string before = "", std::string after = "");
	void printStringDoc(StringDocument* d, bool alignment,
			int startColAlignment, std::string before = "", std::string after =
					"");
	void simplify();
	void simplifyItem(int item);

	friend std::ostream& operator<<(std::ostream& os, const PrettyPrinter& pp);
};

#endif
