/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Pierre WILKE (wilke.pierre@gmail.com)
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <printer/Document.h>

DocumentList::DocumentList(std::string _beginToken, std::string _separator, std::string _endToken, bool _alignment){
	beginToken = _beginToken;
	separator = _separator;
	endToken = _endToken;
	alignment = _alignment;
	unbreakable = false;

}

