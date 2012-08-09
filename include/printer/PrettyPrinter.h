/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Pierre WILKE (wilke.pierre@gmail.com)     
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
	int currentSubItem;
	std::vector<std::vector<Line> > items;
	std::vector<LinesToSimplify> linesToSimplify;
	std::vector<LinesToSimplify> linesNotToSimplify;
	//linesToSimplify[item][part_of_item]
	bool simp;
	bool deeplySimp;

	void addItem();

	void addLine(int indentation, bool bp = false, bool ds = false, int level = 0);
	static std::string printSpaces(int n);
	const std::vector<Line>& getCurrentItemLines() const;

	void printDocument(Document* d, bool alignment, int startColAlignment,
			const std::string& before = "", const std::string& after = "");
	void printDocList(DocumentList* d, bool alignment, int startColAlignment,
			const std::string& before = "", const std::string& after = "");
	void printStringDoc(StringDocument* d, bool alignment,
			int startColAlignment, const std::string& before = "",
			const std::string& after = "");
	void printString(const std::string& s, bool alignment,
			int startColAlignment);
	bool simplify(int item, int line, std::vector<int>* vec);
	void simplifyItem(int item);

	friend std::ostream& operator<<(std::ostream& os, const PrettyPrinter& pp);
};

#endif
