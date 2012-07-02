/*
 * Line.h
 *
 *  Created on: 29 juin 2012
 *      Author: pwilke
 */

#ifndef LINE_H_
#define LINE_H_

#include <vector>
#include <string>
#include <iostream>

class Line {

public:
	Line() :
			indentation(0), length(0), text(0) {
		//	std::cout << "default constructor for line" << std::endl;
	}
	Line(const Line& l) :
			indentation(l.indentation), length(l.length), text(l.text) {
		//std::cout << "copy constructor for line" << text.size() << std::endl;
	}
	Line(const int indent) :
			indentation(indent), length(0), text(0) {
		//std::cout << "constructor(" << indent << ") for line" << std::endl;
	}

	void setIndentation(int i) {
		indentation = i;
	}

	const int getLength() const {
		return length;
	}
	int getIndentation() const {
		return indentation;
	}
	int getSpaceLeft(int maxwidth);
	void addString(std::string s);
	void concatenateLines(Line& l);
	friend std::ostream& operator<<(std::ostream& os, const Line& l);

private:
	int indentation;
	int length;
	std::vector<std::string> text;
	const std::vector<std::string>& getText() const {
		return text;
	}
};

#endif /* LINE_H_ */
