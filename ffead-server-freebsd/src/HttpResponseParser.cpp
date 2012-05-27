/*
 * HttpResponseParser.cpp
 *
 *  Created on: Nov 27, 2010
 *      Author: sumeet
 */

#include "HttpResponseParser.h"

string* HttpResponseParser::headernames = NULL;

HttpResponseParser::HttpResponseParser() {
	// TODO Auto-generated constructor stub

}

HttpResponseParser::~HttpResponseParser() {
	// TODO Auto-generated destructor stub
}



HttpResponseParser::HttpResponseParser(string vecstr,string path)
{
	vector<string> vec;
	boost::iter_split(vec, vecstr, boost::first_finder("\n"));
	if(HttpResponseParser::headernames==NULL)
	{
		HttpResponseParser::headernames = new string();
		ifstream ifs((path+"http-res-headers").c_str(),ifstream::in);
		string tempios;
		while(getline(ifs,tempios,'\n'))
		{
			HttpResponseParser::headernames->append(tempios);
		}
		cout << endl << "done reading header types" << endl;
	}
	if(vec.size()!=0)
	{
		this->content = "";
		string conten;
		bool contStarts = false;
		for(unsigned int i=0;i<vec.size();i++)
		{
			//cout << endl << "Reading line " << vec.at(i) << endl << flush;
			vector<string> temp,vemp,memp;

			if(vec.at(i)=="\r" || vec.at(i)==""|| vec.at(i)=="\r\n")
			{
				contStarts = true;
				continue;
			}
			boost::iter_split(temp, vec.at(i), boost::first_finder(": "));
			if(temp.size()>1)
			{
				cout << "line => " << vec.at(i) << endl;
				boost::replace_first(temp.at(1),"\r","");
				if(HttpResponseParser::headernames->find(temp.at(0)+":")!=string::npos)
				{
					//cout << temp.at(0) << " = " << temp.at(1) << endl;
					this->headers[temp.at(0)] = temp.at(1);
				}
				else
					cout << "\nnot a valid header" << temp.at(0) << endl;
			}
			else
			{

				string tem = vec.at(i);
				if(!contStarts)
				{
					cout << "line => " << vec.at(i) << endl;
					vector<string> httpst;
					boost::iter_split(httpst, tem, boost::first_finder(" "));
					if(httpst.at(0).find("HTTP")==string::npos)
					{

					}
					else
					{
						this->headers["Version"] = httpst.at(0);
						this->headers["StatusCode"] = httpst.at(1);
						this->headers["StatusMsg"] = httpst.at(2);
					}
				}
				else
				{
					string temp;
					if(vec.at(i).find("<?")!=string::npos && vec.at(i).find("?>")!=string::npos)
					{
						temp = vec.at(i).substr(vec.at(i).find("?>")+2);
						conten.append(temp);
					}
					else
						conten.append(vec.at(i));
					if(i!=vec.size()-1)
						conten.append("\n");
				}
			}
		}
		this->content = conten;
		if(this->content!="")
		{
			//cout << this->content << flush;
		}
	}
}

HttpResponseParser::HttpResponseParser(string vecstr)
{
	vector<string> vec;
	boost::iter_split(vec, vecstr, boost::first_finder("\n"));
	if(vec.size()!=0)
	{
		this->content = "";
		string conten;
		bool contStarts = false;
		for(unsigned int i=0;i<vec.size();i++)
		{
			//cout << endl << "Reading line " << vec.at(i) << endl << flush;
			vector<string> temp,vemp,memp;
			if((vec.at(i)=="\r" || vec.at(i)==""|| vec.at(i)=="\r\n") && !contStarts)
			{
				contStarts = true;
				continue;
			}
			boost::iter_split(temp, vec.at(i), boost::first_finder(": "));
			if(temp.size()>1 && !contStarts)
			{
				cout << temp.at(0) << " => " << temp.at(1) << endl;
				boost::replace_first(temp.at(1),"\r","");
				this->headers[temp.at(0)] = temp.at(1);
			}
			else
			{
				string tem = vec.at(i);
				if(!contStarts)
				{
					cout << "line => " << vec.at(i) << endl;
					boost::replace_first(tem,"\r","");
					vector<string> httpst;
					boost::iter_split(httpst, tem, boost::first_finder(" "));
					if(httpst.at(0).find("HTTP")==string::npos)
					{

					}
					else
					{
						this->headers["Version"] = httpst.at(0);
						this->headers["StatusCode"] = httpst.at(1);
						this->headers["StatusMsg"] = httpst.at(2);
					}
				}
				else
				{
					conten.append(vec.at(i));
					if(i!=vec.size()-1)
						conten.append("\n");
				}
			}
		}
		this->content = conten;
		if(this->content!="")
		{
			//cout << this->content << flush;
		}
	}
}
