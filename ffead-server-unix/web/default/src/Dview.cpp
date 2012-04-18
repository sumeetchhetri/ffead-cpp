/*
 * Dview.cpp
 *
 *  Created on: Sep 12, 2009
 *      Author: sumeet
 */

#include "Dview.h"
#include "XmlParser.h"

Dview::Dview() {
	// TODO Auto-generated constructor stub

}

Dview::~Dview() {
	// TODO Auto-generated destructor stub
}

Document Dview::getDocument()
{
	string xml = "<html><head><script src=\"1.js\"></script></head><body><input type=\"text\"/><input type=\"submit\"/></body></html>";
	XmlParser parser("Parser");
	return parser.getDocument(xml);
}
