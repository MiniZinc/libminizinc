/*
 * DocumentList.cpp
 *
 *  Created on: 21 juin 2012
 *      Author: pwilke
 */

#include <printer/Document.h>

DocumentList::DocumentList(std::string _beginToken, std::string _separator, std::string _endToken, bool _alignment){
	beginToken = _beginToken;
	separator = _separator;
	endToken = _endToken;
	alignment = _alignment;
}

