/*
 * StringDocument.h
 *
 *  Created on: 21 juin 2012
 *      Author: pwilke
 */

#ifndef STRINGDOCUMENT_H_
#define STRINGDOCUMENT_H_

#include "Document.h"
#include <string>

class StringDocument: public Document {
public:
	StringDocument();
	StringDocument(std::string s) : stringDocument(s) {}
	virtual ~StringDocument();
	std::string getString() {return stringDocument;}
	void setString(std::string s) { stringDocument = s;}
private:
	std::string stringDocument;
};

#endif /* STRINGDOCUMENT_H_ */
