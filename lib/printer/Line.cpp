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
