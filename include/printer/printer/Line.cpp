#include "Line.h"
#include <iostream>

std::ostream& operator<<(std::ostream& os, const Line& l) {
	for (int i = 0; i < l.getIndentation(); i++) {
		os << std::string(" ");
	}
	std::vector<std::string>::const_iterator it;
	for (it = l.getText().begin(); it != l.getText().end(); it++) {
		os << (*it);
	}
	os << std::string("\n");
	return os;
}

int Line::getSpaceLeft(int maxwidth) {
	return maxwidth - length - indentation;
}
void Line::addString(std::string s) {
	length += s.size();
	text.push_back(s);
}
void Line::concatenateLines(Line& l) {
	std::vector<std::string>& svec = l.text;
	text.insert(text.end(), svec.begin(), svec.end());
	length += l.length;
}
