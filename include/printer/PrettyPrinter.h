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

#include "Document.h"
#include "Line.h"

class PrettyPrinter {
public:
	/*
	 * \brief Constructor for class Pretty Printer
	 * \param maxwidth (default 80) : number of rows
	 * \param indentationBase : spaces that represent the atomic number of spaces
	 * \param sim : whether we want to simplify the result
	 * \param deepSimp : whether we want to simplify at each breakpoint or not
	 */
	PrettyPrinter(int _maxwidth = 80, int _indentationBase = 4,
			bool sim = false, bool deepSimp = false);

	void print(Document* d);

	virtual ~PrettyPrinter() {
	}

private:
	int maxwidth;
	int indentationBase;
	int currentLine;
	int currentItem;
	std::vector<std::vector<Line> > items;
	std::vector<std::vector<int> > linesToSimplify;
	bool simp;
	bool deeplySimp;

	void addItem();
	void addLine(int indentation, bool bp = false);
	static std::string printSpaces(int n);
	const std::vector<Line>& getCurrentItemLines() const;

	void printDocument(Document* d, bool alignment, int startColAlignment,
			std::string before = "", std::string after = "");
	void printDocList(DocumentList* d, bool alignment, int startColAlignment,
			std::string before = "", std::string after = "");
	void printStringDoc(StringDocument* d, bool alignment,
			int startColAlignment, std::string before = "", std::string after =
					"");
	void printString(std::string s, bool alignment, int startColAlignment);
	bool simplify(int item, int line);
	void simplifyItem(int item);

	friend std::ostream& operator<<(std::ostream& os, const PrettyPrinter& pp);
};

#endif
