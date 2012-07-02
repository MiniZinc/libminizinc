/*
 * DocumentList.cpp
 *
 *  Created on: 21 juin 2012
 *      Author: pwilke
 */

#include "DocumentList.h"
#include "StringDocument.h"
#include "BreakPoint.h"

DocumentList::DocumentList(std::string _beginToken, std::string _separator, std::string _endToken, bool _alignment){
	beginToken = _beginToken;
	separator = _separator;
	endToken = _endToken;
	alignment = _alignment;
}

DocumentList::~DocumentList() {
}


void DocumentList::addDocumentToList(Document* d){
	docs.push_back(d);

}

void DocumentList::addStringToList(std::string s){
	addDocumentToList(new StringDocument(s));
}
void DocumentList::addBreakPoint(){
	addDocumentToList(new BreakPoint());
}
