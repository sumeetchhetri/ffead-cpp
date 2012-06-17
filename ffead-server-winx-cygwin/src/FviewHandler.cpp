/*
 * FviewHandler.cpp
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#include "FviewHandler.h"

FviewHandler::FviewHandler() {
	// TODO Auto-generated constructor stub

}

FviewHandler::~FviewHandler() {
	// TODO Auto-generated destructor stub
}

string FviewHandler::handle(HttpRequest* req, HttpResponse& res, map<string, string> fviewmap)
{
	string content;
	cout << "inside fview " << req->getFile() << endl;
	string file = req->getFile();
	boost::replace_first(file,"fview","html");
	string ffile = req->getCntxt_root()+"/fviews/"+file;
	cout << ffile << endl;
	ifstream infile(ffile.c_str());
	string temp;
	if(infile.is_open())
	{
		content = "";
		while(getline(infile, temp))
		{
			if(temp.find("<?")==string::npos && temp.find("?>")==string::npos)
				content.append(temp);
		}
		int h = content.find("</head>");
		int ht = content.find("<html>");
		if(h!=string::npos)
		{
			string st = content.substr(0,h-1);
			string en = content.substr(h);
			content = st + "<script type=\"text/javascript\" src=\"public/json2.js\"></script>";
			content += "<script type=\"text/javascript\" src=\"public/prototype.js\"></script>";
			content += "<script type=\"text/javascript\" src=\"public/afc.js\"></script>";
			content += "<script type=\"text/javascript\" src=\"public/_afc_Objects.js\"></script>";
			content += "<script type=\"text/javascript\" src=\"public/_afc_Interfaces.js\"></script>";
			content += "<script type=\"text/javascript\" src=\"public/"+fviewmap[file]+".js\"></script>" + en;
		}
		else
		{
			if(ht!=string::npos)
			{
				string st = content.substr(0,ht+6);
				string en = content.substr(ht+6);
				content = st + "<script type=\"text/javascript\" src=\"public/json2.js\"></script>";
				content += "<script type=\"text/javascript\" src=\"public/prototype.js\"></script>";
				content += "<script type=\"text/javascript\" src=\"public/afc.js\"></script>";
				content += "<script type=\"text/javascript\" src=\"public/_afc_Objects.js\"></script>";
				content += "<script type=\"text/javascript\" src=\"public/_afc_Interfaces.js\"></script>";
				content += "<script type=\"text/javascript\" src=\"public/"+fviewmap[file]+".js\"></script>" + en;
			}
		}
	}
	infile.close();
	res.setContent_type("text/html");
	//cout << content << flush;
	return content;
}
