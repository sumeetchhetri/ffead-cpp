/*
	Copyright 2009-2012, Sumeet Chhetri

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
 * Bigint.cpp
 *
 *  Created on: 04-Mar-2013
 *      Author: sumeetc
 */

#include "Bigint.h"

string Bigint::BLANK = "";
string Bigint::MINUS = "-";
string Bigint::ZERO = "0";
int Bigint::ZERO_INT = 0L;
int Bigint::NUM_LENGTH = 8;
int Bigint::NUM_MAX = 99999999;
int Bigint::NUM_MAX_THRESHOLD = 100000000;

Bigint::Bigint() {
	isPositive = true;
	decimalStartsAt = 0;
	this->parts.push_back(ZERO_INT);
}

Bigint::~Bigint() {
}

Bigint::Bigint(string value)
{
	create(value);
}

void Bigint::create(string value)
{
	isPositive = true;
	parts.clear();
	string temp = StringUtil::trimCopy(value);
	int minusSign = temp.find_last_of("-");
	if(minusSign>0)
		throw "Invalid -";
	else if(minusSign==0)
	{
		isPositive = false;
		temp = temp.substr(1);
	}
	if(temp.find_first_not_of("0")==string::npos)
	{
		decimalStartsAt = 0;
		isPositive = true;
		this->parts.push_back(ZERO_INT);
		return;
	}
	temp = temp.substr(temp.find_first_not_of("0"));
	while((int)temp.length()>NUM_LENGTH)
	{
		try {
			int x = CastUtil::lexical_cast<int>(temp.substr(temp.length()-NUM_LENGTH));
			temp = temp.substr(0, temp.length()-NUM_LENGTH);
			parts.push_back(x);
		} catch(...) {
			throw "Invalid Bigint value";
		}
	}
	if(temp.length()>0)
	{
		try {
			int x = CastUtil::lexical_cast<int>(temp);
			parts.push_back(x);
		} catch(...) {
			throw "Invalid Bigint value";
		}
	}
	if(parts.at(parts.size()-1)==0)
		parts.erase(parts.begin()+(parts.size()-1));
	checkAndSetIfZero();
}

void Bigint::add(Bigint number)
{
	if(isPositive!=number.isPositive)
	{
		subtract(number);
		return;
	}
	internalAdd(number);
}

Bigint Bigint::operator+(Bigint number)
{
	Bigint temp = *this;
	temp.add(number);
	return temp;
}

Bigint Bigint::operator-(Bigint number)
{
	Bigint temp = *this;
	temp.subtract(number);
	return temp;
}

Bigint Bigint::operator*(Bigint number)
{
	Bigint temp = *this;
	temp.multiply(number);
	return temp;
}

Bigint Bigint::operator/(Bigint number)
{
	Bigint temp = *this;
	temp.divide(number);
	return temp;
}

Bigint& Bigint::operator++()
{
	Bigint temp("1");
	this->add(temp);
	return *this;
}

Bigint& Bigint::operator+=(Bigint number)
{
	this->add(number);
	return *this;
}

Bigint& Bigint::operator--()
{
	Bigint temp("1");
	this->subtract(temp);
	return *this;
}

Bigint& Bigint::operator-=(Bigint number)
{
	this->subtract(number);
	return *this;
}

bool operator==(Bigint &lhs, Bigint &rhs)
{
	if(lhs.compare(rhs)==0)
		return true;
	return false;
}

bool operator!=(Bigint &lhs, Bigint &rhs)
{
	if(lhs.compare(rhs)!=0)
		return true;
	return false;
}

bool operator<(Bigint &lhs, Bigint &rhs)
{
	if(lhs.compare(rhs)==-1)
		return true;
	return false;
}

bool operator<=(Bigint &lhs, Bigint &rhs)
{
	if(lhs.compare(rhs)<=0)
		return true;
	return false;
}

bool operator>(Bigint &lhs, Bigint &rhs)
{
	if(lhs.compare(rhs)==1)
		return true;
	return false;
}

bool operator>=(Bigint &lhs, Bigint &rhs)
{
	if(lhs.compare(rhs)>=0)
		return true;
	return false;
}

void Bigint::internalAdd(Bigint number)
{
	vector<int> nparts;
	if(parts.size()>0 && number.parts.size()>0)
	{
		int eqprtssiz = parts.size();
		if(eqprtssiz<(int)number.parts.size())
			eqprtssiz = number.parts.size();
		bool carryOver = false;
		for (int i = 0; i < eqprtssiz; i++)
		{
			int res =  ((int)parts.size()>i?parts.at(i):0) + ( (int)number.parts.size()>i? number.parts.at(i):0) + (carryOver?1:0);
			if(res>NUM_MAX)
			{
				res -= NUM_MAX_THRESHOLD;
				carryOver = true;
			}
			else
			{
				carryOver = false;
			}
			nparts.push_back(res);
		}
		parts = nparts;
	}
}

void Bigint::subtract(Bigint number)
{
	if(isPositive!=number.isPositive)
	{
		add(number);
		return;
	}
	vector<int> nparts;
	if(parts.size()>0 && number.parts.size()>0)
	{
		int eqprtssiz = parts.size();
		if(eqprtssiz<(int)number.parts.size())
			eqprtssiz = number.parts.size();
		bool carryOver = false;
		int compValue = compare(number);
		vector<int> fparts, sparts;
		if(compValue>=0)
		{
			fparts = parts;
			sparts = number.parts;
		}
		else
		{
			sparts = parts;
			fparts = number.parts;
		}
		for (int i = 0; i < eqprtssiz; i++)
		{
			int res =  ((int)fparts.size()>i?fparts.at(i) - (carryOver?1:0):0) - ((int)sparts.size()>i?sparts.at(i):0);
			if(res<0)
			{
				res = NUM_MAX_THRESHOLD - ((int)sparts.size()>i?sparts.at(i):0) + (((int)fparts.size()>i?fparts.at(i) - (carryOver?1:0):0));
				carryOver = true;
			}
			else
			{
				carryOver = false;
			}
			nparts.push_back(res);
		}
		parts = nparts;
		checkAndSetIfZero();
		if(compValue<0)
		{
			isPositive = false;
		}
	}
}

void Bigint::multiply(Bigint number)
{
	Bigint mulResult;
	string mstring;
	vector<int> mparts;
	string fnvalue = toString();
	string snvalue = number.toString();
	if(fnvalue.length()>snvalue.length())
	{
		if(!number.isPositive)
			mstring = snvalue.substr(1);
		else
			mstring = snvalue;
		mparts = parts;
	}
	else
	{
		if(!number.isPositive)
			mstring = fnvalue.substr(1);
		else
			mstring = fnvalue;
		mparts = number.parts;
	}
	if(mstring!="" && mparts.size()>0)
	{
		int position = 0;
		for (int i = mstring.length(); i > 0 ; i--, position++) {
			string numstr = BLANK;
			int mult = mstring.at(i-1) - '0';
			int carryOver = 0;
			for (int j=0;j<(int)mparts.size();j++) {
				string res = CastUtil::lexical_cast<string>(mparts.at(j)*mult);
				if(j!=(int)mparts.size()-1)
				{

					string mrtn = CastUtil::lexical_cast<string>(mparts.at(j));
					if(res.length()>mrtn.length())
					{
						int numm = CastUtil::lexical_cast<int>(res.substr(1)) + carryOver;
						numstr = CastUtil::lexical_cast<string>(numm) + numstr;
						carryOver = res.at(0) - '0';
					}
					else
					{
						int numm = CastUtil::lexical_cast<int>(res) + carryOver;
						numstr = CastUtil::lexical_cast<string>(numm) + numstr;
						carryOver = 0;
					}
					if(j==0)
					{
						int nl = numstr.length();
						for (int jj = 0; jj < NUM_LENGTH-nl; jj++) {
							numstr = ZERO + numstr;
						}
					}
				}
				else
				{
					int numm = CastUtil::lexical_cast<int>(res) + carryOver;
					numstr = CastUtil::lexical_cast<string>(numm) + numstr;
					carryOver = 0;
				}
			}
			for (int j = 0; j < position; j++) {
				numstr += ZERO;
			}
			try {
				Bigint num(numstr);
				mulResult.internalAdd(num);
			} catch (...) {
			}
		}
	}
	this->parts = mulResult.parts;
	if(isPositive!=number.isPositive)
	{
		isPositive = false;
	}
	else if(isPositive==number.isPositive)
	{
		isPositive = true;
	}
}

void Bigint::divide(Bigint number)
{
	internalDivide(number, false, 1);
}

void Bigint::internalDivide(Bigint number, bool isDecimal, int precision)
{
	string mstring;
	vector<int> mparts;
	string fnvalue = toString();
	string snvalue = number.toString();
	if(fnvalue.length()>snvalue.length())
	{
		if(!number.isPositive)
			mstring = snvalue.substr(1);
		else
			mstring = snvalue;
		mparts = parts;
	}
	else
	{
		if(!number.isPositive)
			mstring = fnvalue.substr(1);
		else
			mstring = fnvalue;
		mparts = number.parts;
	}
	if(mstring!="" && mparts.size()>0)
	{
		int recurse = 0;
		string build;
		decompose(fnvalue, snvalue, number, recurse, build, isDecimal, precision);
		try {
			create(build);
		} catch (...) {
		}
	}
}

int Bigint::decompose(string fnvalue, string snvalue, Bigint number, int recurse, string& build, bool isDecimal, int precision)
{
	if(recurse>=precision || fnvalue==BLANK)return recurse;
	string fntemp;
	int diff = fnvalue.length() - snvalue.length();
	if(diff>0)
	{
		fntemp = fnvalue.substr(0, snvalue.length());
		int len = snvalue.length();
		try {
			Bigint fntmp(fntemp);
			while(fntmp.compare(number)==-1)
			{
				len++;
				fntemp = fnvalue.substr(0, len);
				fntmp.create(fntemp);
			}
		} catch (...) {
		}
	}
	else if(diff<0)
	{
		diff = -diff;
		for (int i = 0; i < diff+1; i++) {
			fnvalue += ZERO;
			if(recurse==0 || (recurse>0 && i>0))
			{
				build.append(ZERO);
				if(recurse==0)
				{
					if(!isDecimal)
					{
						recurse = 15;
						return recurse;
					}
					else
					{
						recurse++;
						decimalStartsAt = build.length();
					}
				}
				else
					recurse++;
			}
			Bigint fntmp(fnvalue);
			if(fntmp.compare(number)==1)
				break;
		}
		fntemp = fnvalue;
	}
	else
	{
		Bigint fntmp(fnvalue);
		if(fntmp.compare(number)==-1)
		{
			fnvalue += ZERO;
			Bigint fntmp(fnvalue);
			if(recurse==0)
			{
				if(!isDecimal)
				{
					recurse = 15;
					return recurse;
				}
				else
				{
					decimalStartsAt = build.length();
					recurse++;
				}
			}
			else
				recurse++;
		}
		fntemp = fnvalue;
	}
	fnvalue = fnvalue.substr(snvalue.length());
	try {
		Bigint fbtemp(fntemp);
		int quotient = 0;
		while(fbtemp.compare(number)>=0)
		{
			quotient++;
			fbtemp.subtract(number);
		}
		if(quotient>0)
		{
			build.append(CastUtil::lexical_cast<string>(quotient));
		}
		if(fnvalue=="")
		{
			if(isDecimal)
			{
				if(recurse==0)
				{
					decimalStartsAt = build.length();
					recurse++;
				}
				else
					recurse++;
			}
			else
			{
				recurse = 15;
			}
		}
		recurse = decompose((fbtemp.toString()==ZERO?"":fbtemp.toString())  + fnvalue, snvalue, number, recurse, build, isDecimal, precision);
	} catch (...) {
		// TODO Auto-generated catch block
	}
	return recurse;
}

void Bigint::checkAndSetIfZero()
{
	bool flag = false;
	for (int i=0;i<(int)parts.size();i++) {
		if(parts.at(i)>0)
		{
			flag = true;
			break;
		}
	}
	if(!flag || parts.size()==0)
	{
		parts.clear();
		this->parts.push_back(ZERO_INT);
		isPositive = true;
	}
}

int Bigint::compare(Bigint number1, Bigint number2)
{
	return number1.compare(number2);
}

int Bigint::compare(Bigint number)
{
	if(isPositive==number.isPositive)
	{
		if(parts.size()==number.parts.size())
		{
			if(parts.at(parts.size()-1)==number.parts.at(parts.size()-1))
			{
				return 0;
			}
			else if(parts.at(parts.size()-1)>number.parts.at(parts.size()-1))
			{
				return 1;
			}
			else
			{
				return -1;
			}
		}
		else if(parts.size()>number.parts.size())
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	else if(isPositive && !number.isPositive)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

string Bigint::toString()
{
	if(parts.size()==0)
		return ZERO;
	string build;
	vector<int> nparts = parts;
	std::reverse(nparts.begin(),nparts.end());
	if(!isPositive)
	{
		build.append(MINUS);
	}
	for (int i=0;i<(int)nparts.size();i++) {
		if(i!=0)
		{
			string numstr = CastUtil::lexical_cast<string>(nparts.at(i));
			for (int j = 0; j < NUM_LENGTH-(int)numstr.length(); j++) {
				build.append(ZERO);
			}
			build.append(numstr);
		}
		else
		{
			build.append(CastUtil::lexical_cast<string>(nparts.at(i)));
		}
	}
	return build;
}
