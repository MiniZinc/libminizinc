/*
 * DocumentList.h
 *
 *  Created on: 21 juin 2012
 *      Author: pwilke
 */

#ifndef DOCUMENTLIST_H_
#define DOCUMENTLIST_H_

#include "Document.h"
#include <vector>
#include <string>



class DocumentList: public Document {
public:
	DocumentList(std::string _beginToken="", std::string _separator="", std::string _endToken="", bool alignment = true);
	void addDocumentToList(Document* d);
	void addStringToList(std::string s);
	void addBreakPoint();
	std::vector<Document*> getDocs() { return docs; }
	void setList(std::vector<Document*> ld) { docs = ld; }
	std::string getBeginToken() {return beginToken; }
	std::string getEndToken() {return endToken; }
	std::string getSeparator() {return separator; }
	bool getAlignment() {return alignment; }
	virtual ~DocumentList();

private:
	std::vector<Document*> docs;
	std::string beginToken;
	std::string separator;
	std::string endToken;
	bool alignment;
};

#endif /* DOCUMENTLIST_H_ */
