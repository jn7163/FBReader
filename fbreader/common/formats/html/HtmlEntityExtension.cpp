/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <cctype>

#include <abstract/ZLInputStream.h>
#include <abstract/ZLFSManager.h>
#include <abstract/ZLXMLReader.h>

#include "HtmlEntityExtension.h"

class CollectionReader : public ZLXMLReader {

protected:
	const Tag *tags() const;

public:
	CollectionReader(std::map<std::string,int> &collection);
	void startElementHandler(int tag, const char **attributes);
	void endElementHandler(int) {}
	void characterDataHandler(const char*, int) {}

private:
	std::map<std::string,int> &myCollection;
};

std::map<std::string,int> HtmlEntityExtension::ourCollection;

int HtmlEntityExtension::symbolNumber(const std::string &name) {
	if (ourCollection.empty()) {
		CollectionReader(ourCollection).readDocument(ZLFile(CollectionFile).inputStream());
	}
	std::map<std::string,int>::const_iterator it = ourCollection.find(name);
	return (it == ourCollection.end()) ? 0 : it->second;
}

static const ZLXMLReader::Tag TAGS[] = {
	{ "entity", 1 },
	{ 0, 0 }
};

CollectionReader::CollectionReader(std::map<std::string,int> &collection) : myCollection(collection) {
}

const ZLXMLReader::Tag *CollectionReader::tags() const {
	return TAGS;
}

void CollectionReader::startElementHandler(int tag, const char **attributes) {
	if (tag == 1) {
		for (int i = 0; i < 4; i++) {
			if (attributes[i] == 0) {
				return;
			}
		}
		static const std::string _name = "name";
		static const std::string _number = "number";
		if ((_name == attributes[0]) && (_number == attributes[2])) {
			myCollection[attributes[1]] = atoi(attributes[3]);
		}
	}
}
