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
#include <map>
#include <algorithm>

class Line {

public:
	Line() :
			indentation(0), lineLength(0), text(0) {
		//	std::cout << "default constructor for line" << std::endl;
	}
	Line(const Line& l) :
			indentation(l.indentation), lineLength(l.lineLength), text(l.text) {
		//std::cout << "copy constructor for line" << text.size() << std::endl;
	}
	Line(const int indent) :
			indentation(indent), lineLength(0), text(0) {
		//std::cout << "constructor(" << indent << ") for line" << std::endl;
	}
	bool operator==(const Line& l) {
		return &l == this;
	}

	void setIndentation(int i) {
		indentation = i;
	}

	const int getLength() const {
		return lineLength;
	}
	int getIndentation() const {
		return indentation;
	}
	int getSpaceLeft(int maxwidth);
	void addString(const std::string& s);
	void concatenateLines(Line& l);

private:
	friend std::ostream& operator<<(std::ostream& os, const Line& l);
	int indentation;
	int lineLength;
	std::vector<std::string> text;
	const std::vector<std::string>& getText() const {
		return text;
	}
};

class LinesToSimplify {
	std::map<int, std::vector<int> > lines;
public:
	void showMap() {
		std::map<int, std::vector<int> >::iterator it;
		for (it = lines.begin(); it != lines.end(); it++) {
			std::cout << it->first << " : ";
			showVector(&(it->second));
			//std::cout << std::endl;
		}
		std::cout << std::endl;
	}
	std::vector<int>* getLinesForPriority(int p) {
		std::map<int, std::vector<int> >::iterator it;
		for (it = lines.begin(); it != lines.end(); it++) {
			if (it->first == p)
				return &(it->second);
		}
		return NULL;
	}
	void addLine(int p, int l) {
		std::map<int, std::vector<int> >::iterator it;
		for (it = lines.begin(); it != lines.end(); it++) {
			if (it->first == p) {
				it->second.push_back(l);
				return;
			}
		}
		std::vector<int> v;
		v.push_back(l);
		lines.insert(std::pair<int, std::vector<int> >(p, v));
	}
	void decrementLine(std::vector<int>* vec, int l) {
		std::vector<int>::iterator vit;

		if (vec != NULL) {
			for (vit = vec->begin(); vit != vec->end(); vit++) {
				if (*vit >= l)
					*vit = *vit - 1;
			}
		}

		//Now the map
		std::map<int, std::vector<int> >::iterator it;
		for (it = lines.begin(); it != lines.end(); it++) {
			for (vit = it->second.begin(); vit != it->second.end(); vit++) {
				if (*vit >= l)
					*vit = *vit - 1;
			}
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
	void remove(std::vector<int>* v, int l) {
		if (v != NULL) {
			v->erase(std::remove(v->begin(), v->end(), l), v->end());
		}
		std::map<int, std::vector<int> >::iterator it;
		for (it = lines.begin(); it != lines.end(); it++) {
			std::vector<int>* v = &(it->second);
			v->erase(std::remove(v->begin(), v->end(), l), v->end());
		}
	}
	std::vector<int>* getLinesToSimplify() {
//		showMap();
		std::vector<int>* vec = new std::vector<int>();

		std::map<int, std::vector<int> >::iterator it;
		for (it = lines.begin(); it != lines.end(); it++) {
			std::vector<int>& svec = it->second;
			vec->insert(vec->begin(), svec.begin(), svec.end());
		}
//		showVector(vec);
		return vec;
	}
};

#endif /* LINE_H_ */
