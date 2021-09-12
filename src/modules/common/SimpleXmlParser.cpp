/*
	Copyright 2009-2020, Sumeet Chhetri 
  
    Licensed under the Apache License, Version 2.0 (the "License"); 
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
 * SimpleXmlParser.cpp
 *
 *  Created on: Sep 11, 2009
 *      Author: sumeet
 */

#include "SimpleXmlParser.h"

const std::string XmlWriter::BLANK = "";

SimpleStreamXmlWriter::SimpleStreamXmlWriter(std::ostream& os, bool is_format) : os(os), tag_open(false), new_line(true), is_format(is_format) {
	if(is_format) {
		os << HEADER;
	}
}

SimpleStreamXmlWriter::~SimpleStreamXmlWriter() {}

std::string SimpleStreamXmlWriter::writerType() {
	return "stream";
}

std::string SimpleStreamXmlWriter::toString() {
	return XmlWriter::BLANK;
}

void SimpleStreamXmlWriter::startElement(const char* tag) {
	this->closeTag();
	if (is_format && elt_stack.size() > 0)
		os << NEWLINE_STR;
	this->indent();
	this->os << "<" << tag;
	elt_stack.push(tag);
	tag_open = true;
	new_line = false;
}

void SimpleStreamXmlWriter::startElement(const std::string& tag) {
	this->closeTag();
	if (is_format && elt_stack.size() > 0)
		os << NEWLINE_STR;
	this->indent();
	this->os << "<" << tag;
	elt_stack.push(tag);
	tag_open = true;
	new_line = false;
}

void SimpleStreamXmlWriter::closeElement() {
	this->closeTag();
	std::string elt = elt_stack.top();
	this->elt_stack.pop();
	if (is_format && new_line)
	{
		os << NEWLINE_STR;
		this->indent();
	}
	new_line = true;
	this->os << "</" << elt << ">";
}

void SimpleStreamXmlWriter::closeAll() {
	while (elt_stack.size())
		this->closeElement();
}

void SimpleStreamXmlWriter::attribute(const std::string& key, const std::string& val) {
	this->os << " " << key << "=\"";
	this->write_escape(val);
	this->os << "\"";
}

void SimpleStreamXmlWriter::attribute(const char* key, const char* val) {
	this->os << " " << key << "=\"";
	this->write_escape(val);
	this->os << "\"";
}

void SimpleStreamXmlWriter::content(const char* val) {
	this->closeTag();
	this->write_escape(val);
}

void SimpleStreamXmlWriter::content(const std::string& val) {
	this->closeTag();
	this->write_escape(val);
}

void SimpleStreamXmlWriter::cdata(const char* val) {
	this->closeTag();
	this->os << "<![CDATA[";
	this->os << val;
	this->os << "]]>";
}

void SimpleStreamXmlWriter::cdata(const std::string& val) {
	this->closeTag();
	this->os << "<![CDATA[";
	this->os << val;
	this->os << "]]>";
}

SimpleStringXmlWriter::SimpleStringXmlWriter(std::string& os, bool is_format) {
	this->os = &os;
	tag_open = false;
	new_line = true;
	this->is_format = is_format;
	if(is_format) {
		this->os->append(HEADER);
	}
}

SimpleStringXmlWriter::SimpleStringXmlWriter(std::string* os, bool is_format) {
	this->os = os;
	tag_open = false;
	new_line = true;
	this->is_format = is_format;
	if(is_format) {
		this->os->append(HEADER);
	}
}

SimpleStringXmlWriter::SimpleStringXmlWriter(bool is_format) {
	this->os = &ib;
	tag_open = false;
	new_line = true;
	this->is_format = is_format;
	if(is_format) {
		this->os->append(HEADER);
	}
}

SimpleStringXmlWriter::~SimpleStringXmlWriter() {}

std::string SimpleStringXmlWriter::writerType() {
	return "string";
}

std::string SimpleStringXmlWriter::toString() {
	return ib;
}

void SimpleStringXmlWriter::startElement(const char* tag) {
	this->closeTag();
	if (is_format && elt_stack.size() > 0)
		os->append(NEWLINE_STR);
	this->indent();
	os->append("<");os->append(tag);
	elt_stack.push(tag);
	tag_open = true;
	new_line = false;
}

void SimpleStringXmlWriter::startElement(const std::string& tag) {
	this->closeTag();
	if (is_format && elt_stack.size() > 0)
		os->append(NEWLINE_STR);
	this->indent();
	os->append("<");os->append(tag);
	elt_stack.push(tag);
	tag_open = true;
	new_line = false;
}

void SimpleStringXmlWriter::closeElement() {
	this->closeTag();
	std::string elt = elt_stack.top();
	this->elt_stack.pop();
	if (is_format && new_line)
	{
		os->append(NEWLINE_STR);
		this->indent();
	}
	new_line = true;
	os->append("</");os->append(elt);os->append(">");
}

void SimpleStringXmlWriter::closeAll() {
	while (elt_stack.size())
		this->closeElement();
}

void SimpleStringXmlWriter::attribute(const std::string& key, const std::string& val) {
	os->append(" ");os->append(key);os->append("=\"");
	this->write_escape(val);
	os->append("\"");
}

void SimpleStringXmlWriter::attribute(const char* key, const char* val) {
	os->append(" ");os->append(key);os->append("=\"");
	this->write_escape(val);
	os->append("\"");
}

void SimpleStringXmlWriter::content(const char* val) {
	this->closeTag();
	this->write_escape(val);
}

void SimpleStringXmlWriter::content(const std::string& val) {
	this->closeTag();
	this->write_escape(val);
}

void SimpleStringXmlWriter::cdata(const char* val) {
	this->closeTag();
	os->append("<![CDATA[");
	os->append(val);
	os->append("]]>");
}

void SimpleStringXmlWriter::cdata(const std::string& val) {
	this->closeTag();
	os->append("<![CDATA[");
	os->append(val);
	os->append("]]>");
}

SimpleXmlParser::SimpleXmlParser(const std::string& mode)
{
	this->mode = mode;
}

SimpleXmlParser::~SimpleXmlParser() {
}

void SimpleXmlParser::readDocument(const std::string& filename, Document& doc)
{
	std::ifstream infile(filename.c_str());
	std::string temp;
	std::string xml;
	if(infile.is_open())
	{
		xml = "";
		while(getline(infile, temp))
		{
			if(temp.find("<?")==std::string::npos && temp.find("?>")==std::string::npos)
				xml.append(temp+"\n");
		}
		infile.close();
	}
	parse(xml, doc);
}

void SimpleXmlParser::parse(std::string xml, Document& doc)
{
	StringUtil::trim(xml);
	if(xml.find("<")==0 && xml.find(">")!=std::string::npos)
	{
		if(xml.find("<?")!=std::string::npos && xml.find("?>")!=std::string::npos)
		{
			xml = xml.substr(xml.find("?>")+2);
		}
		readXML(xml, "", &(doc.root));
	}
	if(StringUtil::toLowerCopy(mode)=="validator")
	{
		try {
			doc.root.validateNs();
		} catch(const std::exception& ex) {
			std::string errmsg(ex.what());
			throw XmlParseException(errmsg);
		}
	}
}

void SimpleXmlParser::sanitizeXml(std::string& strret) {
	StringUtil::replaceAll(strret,"&amp;","&");
	StringUtil::replaceAll(strret,"&quot;","\"");
	StringUtil::replaceAll(strret,"&apos;","'");
	StringUtil::replaceAll(strret,"&lt;","<");
	StringUtil::replaceAll(strret,"&gt;",">");
}

void SimpleXmlParser::readXML(std::string xml, const std::string& parent, Element *par)
{
	if(xml=="")
		return;
	StringUtil::trim(xml);
	int cdt = xml.find("<![CDATA[");
	if(cdt==0)
	{
		int ecdt = xml.find("]]>");
		if(ecdt==(int)std::string::npos)
		{
			std::string errmsg = ("Incomplete CDATA tag\n");
			throw XmlParseException(errmsg);

		}
		else
		{
			par->setCdata(true);
			par->setText(xml.substr(cdt+9,ecdt-cdt-9));
			return;
		}
	}
	int cmt =  xml.find("<!--");
	if(cmt!=(int)std::string::npos)
	{
		int ecmt = xml.find("-->");
		if(ecmt==(int)std::string::npos)
		{
			std::string errmsg = ("Incomplete Comment tag\n");
			throw XmlParseException(errmsg);

		}
		else
		{
			std::string stx = xml.substr(0,cmt);
			std::string enx = xml.substr(ecmt+3);
			xml = stx + enx;
		}
	}
	int st = xml.find("<")+1;
    int ed = 0;
    int ed1 = xml.find("/>");
    int ed2 = xml.find(">");
    if((ed2<ed1 || ed1==-1) && ed2!=-1)
    	ed = ed2;
    else if(ed1!=-1)
    {
    	/*if(xml[ed1+1]=='>')*/
    		ed = ed1;
    	/*else
    		ed = ed2;*/
    }
    std::string tag = xml.substr(st,ed-st);
    int ss = tag.find_first_not_of(" ");
    int se = tag.find_last_not_of(" ")+1;
    tag = tag.substr(ss,se-ss);
    Element element;

    //split the tag with a space to get all the attribute sets of the element
    std::string ta;
    if(tag.find_first_of(" ")!=std::string::npos)
    {
        ta = tag.substr(0,tag.find_first_of(" "));
        tag = tag.substr(tag.find_first_of(" ")+1);
        while(tag.find_first_of("=")!=std::string::npos && tag.find_first_of("\"")!=std::string::npos)
        {
            std::string atname = tag.substr(0,tag.find_first_of("="));
            int as = atname.find_first_not_of(" ");
            int ae = atname.find_last_not_of(" ")+1;
            atname = atname.substr(as,ae-as);
            int ds = tag.find_first_of("\"")+1;
            tag = tag.substr(ds);
            std::string atvalue = tag.substr(0,tag.find_first_of("\""));
            tag = tag.substr(tag.find_first_of("\"")+1);
            try {
            	//sanitizeXml(atvalue);
				if(parent!="")
				{
					element.addAttribute(StringUtil::trimCopy(atname),atvalue,true);
				}
				else
				{
					par->addAttribute(StringUtil::trimCopy(atname),atvalue,true);
				}
            } catch(const std::exception& ex) {
            	if(StringUtil::toLowerCopy(mode)=="validator")
            	{
            		std::string errmsg(ex.what());
            		throw XmlParseException(errmsg);
            	}
            }
            //logger << "attname = " << atname << "   attvalue = " << atvalue << "\n" << std::flush;
        }
    }
    else
    	ta = tag;
    int initcheck = xml.find_first_of("<");
    size_t someTag = (xml.substr(initcheck+1)).find("<");
    int pndTag=0,endTag=0;
    if(xml.find("</"+ta)!=std::string::npos)
    	pndTag = xml.find("</"+ta);
    else if(xml.find("/>")!=std::string::npos && xml.find("/>")<someTag)
    	endTag = xml.find("/>");
    if(xml.find("< ")!=std::string::npos)
    {
        std::string errmsg = ("Invalid Start Tag - at position: " + CastUtil::fromNumber((int)xml.find("< ")+1) + "\n");
        throw XmlParseException(errmsg);
    }
    else if(xml.find("<\t")!=std::string::npos)
    {
        std::string errmsg = ("Invalid Start Tag - at position: " + CastUtil::fromNumber((int)xml.find("<\t")+1) + "\n");
        throw XmlParseException(errmsg);

    }
    else if(xml.find("</ ")!=std::string::npos)
    {
        std::string errmsg = ("Invalid End Tag - at position: " + CastUtil::fromNumber((int)xml.find("</ ")+1) + "\n");
        throw XmlParseException(errmsg);

    }
    else if(xml.find("</\t")!=std::string::npos)
    {
        std::string errmsg = ("Invalid End Tag - at position: " + CastUtil::fromNumber((int)xml.find("</\t")+1) + "\n");
        throw XmlParseException(errmsg);

    }
    else if(xml.find("< /")!=std::string::npos)
    {
        std::string errmsg = ("Invalid End Tag - at position: " + CastUtil::fromNumber((int)xml.find("< /")+1) + "\n");
        throw XmlParseException(errmsg);

    }
    else if(xml.find("/ >")!=std::string::npos)
	{
		std::string errmsg = ("Invalid End Tag - at position: " + CastUtil::fromNumber((int)xml.find("/ >")+1) + "\n");
		throw XmlParseException(errmsg);

	}
    else if(xml.find("<\t/")!=std::string::npos)
    {
        std::string errmsg = ("Invalid End Tag - at position: " + CastUtil::fromNumber((int)xml.find("<\t/")+1) + "\n");
        throw XmlParseException(errmsg);
    }
    else if(xml.find("<"+ta)==std::string::npos && xml.find("</"+ta)!=std::string::npos)
    {
        std::string errmsg = ("No Start Tag - for : " + ta + "\n");
        throw XmlParseException(errmsg);
    }
    else if(xml.find("<"+ta)!=std::string::npos && pndTag==0 && endTag==0)
    {
        std::string errmsg = ("No End Tag - for : " + ta + "\n");
        throw XmlParseException(errmsg);
    }
    if(xml.find("<"+ta)!=std::string::npos && (xml.find("</"+ta)!=std::string::npos || xml.find("/>")!=std::string::npos))
    {
        //logger << "tag = " << ta << std::flush;
        //logger << "   parent = " << parent << std::flush;
        //logger << "\n" << std::flush;

        if(ed==ed1)
        {
        	if(parent!="")
			{
				element.setTagName(StringUtil::trimCopy(ta));
				par->addElement(element);
			}
			else
			{
				par->setTagName(StringUtil::trimCopy(ta));
			}
        	xml = xml.substr(xml.find("/>")+2);
        }
        else if(xml.find("</"+ta)!=std::string::npos)
		{
			//split each set by = to get the pairs
			std::string tagx = "</"+ta+">";
			int end = xml.find("</"+ta+">");
			std::string txml = xml.substr(ed+1,end-ed-1);
			int stoc = StringUtil::countOccurrences(txml, "<"+ta+">");
			int sstoc = StringUtil::countOccurrences(txml, "<"+ta+" ");
			int etoc = StringUtil::countOccurrences(txml, "</"+ta+">");
			while(stoc+sstoc!=etoc) {
				end = xml.find("</"+ta+">", end+2);
				if(end==std::string::npos) {
			        std::string errmsg = ("No Start Tag - for : " + ta + "\n");
			        throw XmlParseException(errmsg);
			    }
				txml = xml.substr(ed+1,end-ed-1);
				stoc = StringUtil::countOccurrences(txml, "<"+ta+">");
				sstoc = StringUtil::countOccurrences(txml, "<"+ta+" ");
				etoc = StringUtil::countOccurrences(txml, "</"+ta+">");
			}
			//logger << "temp = " << txml << std::flush;
			//logger << "\n" << std::flush;
			if(parent!="")
			{
				element.setTagName(StringUtil::trimCopy(ta));
				if(txml.find("<")!=std::string::npos)
				{
					readXML(txml,ta,&element);
				}
				else
				{
					//sanitizeXml(txml);
					element.setText(txml);
				}
				par->addElement(element);
			}
			else
			{
				par->setTagName(StringUtil::trimCopy(ta));
				if(txml.find("<")!=std::string::npos)
				{
					readXML(txml,ta,par);
				}
				else
				{
					//sanitizeXml(txml);
					par->setText(txml);
				}
			}
			xml = xml.substr(end+tagx.length());
		}
    }
    if(xml.find("<")!=std::string::npos && (xml.find("</")!=std::string::npos || xml.find("/>")!=std::string::npos))
    {
        //logger << "xml = " << xml << std::flush;
        //logger << "\n" << std::flush;
        readXML(xml,parent,par);
    }
    else if(xml.find("<")!=std::string::npos && (xml.find("</")==std::string::npos || xml.find("/>")==std::string::npos))
    {
    	std::string errmsg = ("Invalid Start Tag\n");
    	throw XmlParseException(errmsg);
    }
    else if(xml.find("<")==std::string::npos && (xml.find("</")!=std::string::npos || xml.find("/>")!=std::string::npos))
	{
		std::string errmsg = ("Invalid End Tag\n");
		throw XmlParseException(errmsg);
	}
}
