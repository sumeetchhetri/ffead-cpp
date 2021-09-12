/*
	Copyright 2009-2020, Sumeet Chhetri 

    Licensed under the Apache License, Version 2.0 (const the& "License"); 
    you may not use this file except in compliance with the License. 
    You may obtain a copy of the License at 

        http://www.apache.org/licenses/LICENSE-2.0 

    Unless required by applicable law or agreed to in writing, software 
    distributed under the License is distributed on an "AS IS" BASIS, 
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
    See the License for the specific language governing permissions and 
    limitations under the License.  
 */
/*
 * SimpleXmlParser.h
 *
 *  Created on: Sep 11, 2009
 *      Author: sumeet
 */

//Refer - https://gist.github.com/sebclaeys/1227644/3761c33416d71c20efc300e78ea1dc36221185c5
/*
	std::stringstream a;
    std::string s;
    //StringRefStream b(s);
    //SimpleStreamXmlWriter writer(b.stream()); //Option 1
    //SimpleStreamXmlWriter writer(std::cout);  //Option 2
    //std::stringstream ss;
    //SimpleStreamXmlWriter writer(ss); //Option 3
    SimpleStringXmlWriter writer(s); //Option 4
    writer.startElement("Movies");
    writer.startElement("Goldeneye").attribute("date", "1998").content("This is a James Bond movie").closeElement();
    writer.startElement("Leon").attribute("director", "Luc Besson");
    writer.startElement("Actor").attribute("role", "Leon").attr("name", "Jean Reno").cdata("<asdas></asdas>").closeAll();
    std::cout << s << std::endl;
*/


#ifndef XMLPARSER_H_
#define XMLPARSER_H_

#include "AppDefines.h"
#include <iostream>
#include "stdlib.h"
#include "Document.h"
#include <fstream>
#include "XmlParseException.h"
#include "CastUtil.h"
#include "LoggerFactory.h"
#include "StringUtil.h"

#define HEADER "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
#define INDENT_STR "\t"
#define NEWLINE_STR "\n"

#include <string>
#include <stack>
#include <iostream>
#include <sstream>

class StringRefStream: public std::streambuf {
	using Base = std::basic_streambuf<char>;
	using int_type = typename Base::int_type;
public:
	StringRefStream(std::string& s) : _s(s), _st(this) {}
	~StringRefStream() {}
	std::streamsize xsputn(const char_type* s, std::streamsize n) {
		//std::cout << n << std::endl;
		//std::cout << s << std::endl;
		_s.append(s, n);
		return n;
	}
	int_type overflow(int_type ch) {
		//std::cout << "overflow " << ch << std::endl;
		_s.push_back((char)ch);
		return 1;
	}
	std::ostream& stream() {
		return _st;
	}
private:
	std::string& _s;
	std::ostream _st;
};

class XmlWriter {
public:
	static const std::string BLANK;
	virtual ~XmlWriter() {};
	virtual void startElement(const char* tag)=0;
	virtual void startElement(const std::string& tag)=0;
	virtual void closeElement()=0;
	virtual void closeAll()=0;
	virtual void attribute(const char* key, const char* val)=0;
	virtual void attribute(const std::string& key, const std::string& val)=0;
	virtual void content(const char* content)=0;
	virtual void content(const std::string& content)=0;
	virtual void cdata(const char* content)=0;
	virtual void cdata(const std::string& content)=0;
	virtual std::string writerType()=0;
	virtual std::string toString()=0;
};

class SimpleStreamXmlWriter: public XmlWriter
{
public:
	SimpleStreamXmlWriter(std::ostream& os, bool is_format = false);
	virtual ~SimpleStreamXmlWriter();

	void startElement(const char* tag);
	void startElement(const std::string& tag);
	void closeElement();
	void closeAll();
	void attribute(const char* key, const char* val);
	void attribute(const std::string& key, const std::string& val);
	void content(const char* content);
	void content(const std::string& content);
	void cdata(const char* content);
	void cdata(const std::string& content);
	std::string writerType();
	std::string toString();
private:
	std::ostream& os;
	bool tag_open;
	bool new_line;
	bool is_format;
	std::stack<std::string> elt_stack;

	inline void closeTag() {
		if (tag_open)
		{
			this->os << ">";
			tag_open = false;
		}
	}

	inline void indent() {
		if(!is_format) return;
		for (int i = 0; i < (int)elt_stack.size(); i++)
			os << (INDENT_STR);
	}

	inline void write_escape(const char* str) {
		for (; *str; str++) {
			switch (*str) {
			case '&': os << "&amp;"; break;
			case '<': os << "&lt;"; break;
			case '>': os << "&gt;"; break;
			case '\'': os << "&apos;"; break;
			case '"': os << "&quot;"; break;
			default: os.put(*str); break;
			}
		}
	}

	inline void write_escape(const std::string& val) {
		for (auto str: val) {
			switch (str) {
			case '&': os << "&amp;"; break;
			case '<': os << "&lt;"; break;
			case '>': os << "&gt;"; break;
			case '\'': os << "&apos;"; break;
			case '"': os << "&quot;"; break;
			default: os.put(str); break;
			}
		}
	}
};

class SimpleStringXmlWriter: public XmlWriter
{
public:
	SimpleStringXmlWriter(std::string& os, bool is_format = false);
	SimpleStringXmlWriter(std::string* os, bool is_format = false);
	SimpleStringXmlWriter(bool is_format = false);
	virtual ~SimpleStringXmlWriter();

	void startElement(const char* tag);
	void startElement(const std::string& tag);
	void closeElement();
	void closeAll();
	void attribute(const char* key, const char* val);
	void attribute(const std::string& key, const std::string& val);
	void content(const char* content);
	void content(const std::string& content);
	void cdata(const char* content);
	void cdata(const std::string& content);
	std::string writerType();
	std::string toString();
private:
	std::string ib;
	std::string* os;
	bool tag_open;
	bool new_line;
	bool is_format;
	std::stack<std::string> elt_stack;

	inline void closeTag() {
		if (tag_open)
		{
			os->append(">");
			tag_open = false;
		}
	}

	inline void indent() {
		if(!is_format) return;
		for (int i = 0; i < (int)elt_stack.size(); i++)
			os->append(INDENT_STR);
	}

	inline void write_escape(const char* str) {
		for (; *str; str++) {
			switch (*str) {
			case '&': os->append("&amp;"); break;
			case '<': os->append("&lt;"); break;
			case '>': os->append("&gt;"); break;
			case '\'': os->append("&apos;"); break;
			case '"': os->append("&quot;"); break;
			default: os->push_back(*str); break;
			}
		}
	}

	inline void write_escape(const std::string& val) {
		for (auto str: val) {
			switch (str) {
			case '&': os->append("&amp;"); break;
			case '<': os->append("&lt;"); break;
			case '>': os->append("&gt;"); break;
			case '\'': os->append("&apos;"); break;
			case '"': os->append("&quot;"); break;
			default: os->push_back(str); break;
			}
		}
	}
};

class SimpleXmlParser {
public:
	SimpleXmlParser(const std::string&);
	virtual ~SimpleXmlParser();
	void parse(std::string, Document&);
	void readDocument(const std::string& filename, Document&);
private:
	Logger logger;
	std::string mode;
	void sanitizeXml(std::string& data);
	void readXML(std::string, const std::string&, Element *);
};


#ifdef HAVE_PUGI_XML
#include "pugixml.hpp"
class PugiXmlParser {
	virtual ~PugiXmlParser(){}
public:
	static void parse(std::string xml, pugi::xml_document& doc) {
		pugi::xml_parse_result result = doc.load_buffer(xml.c_str(), xml.length());
		if (!result) {
			throw std::runtime_error(result.description());
		}
	}
	static void readDocument(const std::string& filename, pugi::xml_document& doc) {
		pugi::xml_parse_result result = doc.load_file(filename.c_str());
		if (!result) {
			throw std::runtime_error(result.description());
		}
	}
};
#endif

#endif /* XMLPARSER_H_ */
