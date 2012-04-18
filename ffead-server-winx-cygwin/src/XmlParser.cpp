/*
	Copyright 2010, Sumeet Chhetri 
  
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
 * XmlParser.cpp
 *
 *  Created on: Sep 11, 2009
 *      Author: sumeet
 */

#include "XmlParser.h"


XmlParser::XmlParser(string mode)
{
	this->mode = mode;
}

XmlParser::~XmlParser() {
	// TODO Auto-generated destructor stub
}

Document XmlParser::getDocument(string xml)
{
	ifstream infile(xml.c_str());
	string temp;
	if(infile.is_open())
	{
		xml = "";
		while(getline(infile, temp))
		{
			if(temp.find("<?")==string::npos && temp.find("?>")==string::npos)
				xml.append(temp+"\n");
		}
	}
	Document doc;
	Element root;
	if(xml.find("<")!=string::npos && xml.find(">")!=string::npos)
	{
		if(xml.find("<?")!=string::npos && xml.find("?>")!=string::npos)
		{
			xml = xml.substr(xml.find("?>")+2);
		}
		readXML(xml,"",&root);
	}
	doc.setRootElement(root);
	return doc;
}

void XmlParser::readXML(string xml,string parent,Element *par)
{
	if(xml=="")
		return;
	boost::trim(xml);
	int cdt = xml.find("<![CDATA[");
	if(cdt==0)
	{
		int ecdt = xml.find("]]>");
		if(ecdt==string::npos)
		{
			string errmsg = ("Incomplete CDATA tag\n");
			throw new XmlParseException(errmsg);
		}
		else
		{
			par->setCdata(true);
			par->setText(xml.substr(cdt+9,ecdt-cdt-9));
			return;
		}
	}
	int cmt =  xml.find("<!--");
	if(cmt!=string::npos)
	{
		int ecmt = xml.find("-->");
		if(ecmt==string::npos)
		{
			string errmsg = ("Incomplete Comment tag\n");
			throw new XmlParseException(errmsg);
		}
		else
		{
			string stx = xml.substr(0,cmt);
			string enx = xml.substr(ecmt+3);
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
    string tag = xml.substr(st,ed-st);
    int ss = tag.find_first_not_of(" ");
    int se = tag.find_last_not_of(" ")+1;
    tag = tag.substr(ss,se-ss);
    Element element;

    //split the tag with a space to get all the attribute sets of the element
    string ta;
    if(tag.find_first_of(" ")!=string::npos)
    {
        ta = tag.substr(0,tag.find_first_of(" "));
        tag = tag.substr(tag.find_first_of(" ")+1);
        while(tag.find_first_of("=")!=string::npos && tag.find_first_of("\"")!=string::npos)
        {
            string atname = tag.substr(0,tag.find_first_of("="));
            int as = atname.find_first_not_of(" ");
            int ae = atname.find_last_not_of(" ")+1;
            atname = atname.substr(as,ae-as);
            int ds = tag.find_first_of("\"")+1;
            tag = tag.substr(ds);
            string atvalue = tag.substr(0,tag.find_first_of("\""));
            tag = tag.substr(tag.find_first_of("\"")+1);
            if(parent!="")
			{
            	element.addAttribute(atname,atvalue);
			}
			else
			{
				par->addAttribute(atname,atvalue);
			}
            cout << "attname = " << atname << "   attvalue = " << atvalue << "\n" << flush;
        }
    }
    else
    	ta = tag;
    int initcheck = xml.find_first_of("<");
    unsigned int someTag = (xml.substr(initcheck+1)).find("<");
    int pndTag=0,endTag=0;
    if(xml.find("</"+ta)!=string::npos)
    	pndTag = xml.find("</"+ta);
    else if(xml.find("/>")!=string::npos && xml.find("/>")<someTag)
    	endTag = xml.find("/>");
    if(xml.find("< ")!=string::npos)
    {
        string errmsg = ("Invalid Start Tag - at position: " + boost::lexical_cast<string>(xml.find("< ")+1) + "\n");
        throw new XmlParseException(errmsg);
    }
    else if(xml.find("<\t")!=string::npos)
    {
        string errmsg = ("Invalid Start Tag - at position: " + boost::lexical_cast<string>(xml.find("<\t")+1) + "\n");
        throw new XmlParseException(errmsg);
    }
    else if(xml.find("</ ")!=string::npos)
    {
        string errmsg = ("Invalid End Tag - at position: " + boost::lexical_cast<string>(xml.find("</ ")+1) + "\n");
        throw new XmlParseException(errmsg);
    }
    else if(xml.find("</\t")!=string::npos)
    {
        string errmsg = ("Invalid End Tag - at position: " + boost::lexical_cast<string>(xml.find("</\t")+1) + "\n");
        throw new XmlParseException(errmsg);
    }
    else if(xml.find("< /")!=string::npos)
    {
        string errmsg = ("Invalid End Tag - at position: " + boost::lexical_cast<string>(xml.find("< /")+1) + "\n");
        throw new XmlParseException(errmsg);
    }
    else if(xml.find("/ >")!=string::npos)
	{
		string errmsg = ("Invalid End Tag - at position: " + boost::lexical_cast<string>(xml.find("/ >")+1) + "\n");
		throw new XmlParseException(errmsg);
	}
    else if(xml.find("<\t/")!=string::npos)
    {
        string errmsg = ("Invalid End Tag - at position: " + boost::lexical_cast<string>(xml.find("<\t/")+1) + "\n");
        throw new XmlParseException(errmsg);
    }
    else if(xml.find("<"+ta)==string::npos && xml.find("</"+ta)!=string::npos)
    {
        string errmsg = ("No Start Tag - for : " + ta + "\n");
        throw new XmlParseException(errmsg);
    }
    else if(xml.find("<"+ta)!=string::npos && pndTag==0 && endTag==0)
    {
        string errmsg = ("No End Tag - for : " + ta + "\n");
        throw new XmlParseException(errmsg);
    }
    if(xml.find("<"+ta)!=string::npos && (xml.find("</"+ta)!=string::npos || xml.find("/>")!=string::npos))
    {
        cout << "tag = " << ta << flush;
        cout << "   parent = " << parent << flush;
        cout << "\n" << flush;

        if(ed==ed1)
        {
        	if(parent!="")
			{
				element.setTagName(ta);
				par->addElement(element);
			}
			else
			{
				par->setTagName(ta);
			}
        	xml = xml.substr(xml.find("/>")+2);
        }
        else if(xml.find("</"+ta)!=string::npos)
		{
			//split each set by = to get the pairs
			string tagx = "</"+ta+">";
			int end = xml.find("</"+ta+">");
			string txml = xml.substr(ed+1,end-ed-1);
			cout << "temp = " << txml << flush;
			cout << "\n" << flush;
			if(parent!="")
			{
				element.setTagName(ta);
				if(txml.find("<")!=string::npos)
				{
					readXML(txml,ta,&element);
				}
				else
					element.setText(txml);
				par->addElement(element);
			}
			else
			{
				par->setTagName(ta);
				if(txml.find("<")!=string::npos)
				{
					readXML(txml,ta,par);
				}
				else
					par->setText(txml);
			}
			xml = xml.substr(end+tagx.length());
		}
    }
    if(xml.find("<")!=string::npos && (xml.find("</")!=string::npos || xml.find("/>")!=string::npos))
    {
        cout << "xml = " << xml << flush;
        cout << "\n" << flush;
        readXML(xml,parent,par);
    }
    else if(xml.find("<")!=string::npos && (xml.find("</")==string::npos || xml.find("/>")==string::npos))
    {
    	string errmsg = ("Invalid Start Tag\n");
    	throw new XmlParseException(errmsg);
    }
    else if(xml.find("<")==string::npos && (xml.find("</")!=string::npos || xml.find("/>")!=string::npos))
	{
		string errmsg = ("Invalid End Tag\n");
		throw new XmlParseException(errmsg);
	}
}

