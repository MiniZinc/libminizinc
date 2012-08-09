/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *  Pierre WILKE (wilke.pierre@gmail.com)
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include <vector>
#include <string>
#include <iostream>
class Document {
public:
	Document() {
		level = 0;
	}
	virtual ~Document() {
	}
	int getLevel() {
		return level;
	}
	void setParent(Document* d) {
		level = d->level + 1;
//		parent = d;
	}
//	Document* getParent(){
//		return parent;
//	}
private:
	int level;
//	Document* parent;
};

class BreakPoint: public Document {
public:
	BreakPoint() {
		dontSimplify = false;
	}
	BreakPoint(bool ds) {
		dontSimplify = ds;
	}
	virtual ~BreakPoint() {
	}
	void setDontSimplify(bool b) {
		dontSimplify = b;
	}
	bool getDontSimplify() {
		return dontSimplify;
	}
private:
	bool dontSimplify;

};

class StringDocument: public Document {
public:
	StringDocument() {
	}
	virtual ~StringDocument() {
	}
	StringDocument(std::string s) :
			stringDocument(s) {
	}

	std::string getString() {
		return stringDocument;
	}
	void setString(std::string s) {
		stringDocument = s;
	}
private:
	std::string stringDocument;
};

class DocumentList: public Document {
public:
	DocumentList() {
	}
	virtual ~DocumentList() {
		std::vector<Document*>::iterator it;
		for (it = docs.begin(); it != docs.end(); it++) {
			delete *it;
		}
	}
	DocumentList(std::string _beginToken = "", std::string _separator = "",
			std::string _endToken = "", bool _alignment = true);
	void addDocumentToList(Document* d) {
		docs.push_back(d);
		d->setParent(this);
		if (DocumentList* dl = dynamic_cast<DocumentList*>(d)) {
			dl->setParent(this);
		}
	}
	void setParent(Document* d) {
		std::vector<Document*>::iterator it;
		for (it = docs.begin(); it != docs.end(); it++) {
			(*it)->setParent(this);
			if (DocumentList* dl = dynamic_cast<DocumentList*>(*it)) {
				dl->setParent(this);
			}
		}
	}
	void addStringToList(std::string s) {
		addDocumentToList(new StringDocument(s));
	}
	void addBreakPoint(bool b = false) {
		addDocumentToList(new BreakPoint(b));
	}
	std::vector<Document*> getDocs() {
		return docs;
	}
	void setList(std::vector<Document*> ld) {
		docs = ld;
	}
	std::string getBeginToken() {
		return beginToken;
	}
	std::string getEndToken() {
		return endToken;
	}
	std::string getSeparator() {
		return separator;
	}
	bool getUnbreakable() {
		return unbreakable;
	}
	void setUnbreakable(bool b) {
		unbreakable = b;
	}
	bool getAlignment() {
		return alignment;
	}
//	void setDontSimplify(bool b) {
//		dontSimplify = b;
//	}
//	bool getDontSimplify() {
//		return dontSimplify;
//	}

private:
	std::vector<Document*> docs;
	std::string beginToken;
	std::string separator;
	std::string endToken;
	bool unbreakable;
	bool alignment;
//	bool dontSimplify;
};

#endif /* DOCUMENTLIST_H_ */
