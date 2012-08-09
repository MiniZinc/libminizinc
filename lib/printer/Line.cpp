/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Pierre WILKE (wilke.pierre@gmail.com)
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <printer/Line.h>
#include <iostream>

std::ostream& operator<<(std::ostream& os, const Line& l) {
	for (int i = 0; i < l.getIndentation(); i++) {
		os << " ";
	}
	std::vector<std::string>::const_iterator it;
	for (it = l.getText().begin(); it != l.getText().end(); it++) {
		os << (*it);
	}
	os << "\n";
	return os;
}

int Line::getSpaceLeft(int maxwidth) {
	return maxwidth - lineLength - indentation;
}
void Line::addString(const std::string& s) {
	lineLength += s.size();
	text.push_back(s);
}
void Line::concatenateLines(Line& l) {
	text.insert(text.end(), l.text.begin(), l.text.end());
	lineLength += l.lineLength;
}
