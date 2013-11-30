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
 * Bigdecimal.cpp
 *
 *  Created on: 06-Mar-2013
 *      Author: sumeetc
 */

#include "Bigdecimal.h"

Bigdecimal::Bigdecimal() {
	isPositive = true;
	this->parts.push_back(Bigint::ZERO_INT);
	this->decimalDigits = 0;
	this->decimalStartsAt = 0;
}

Bigdecimal::~Bigdecimal() {
}

Bigdecimal::Bigdecimal(string value)
{
	create(value);
}

void Bigdecimal::create(string value)
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
		this->decimalStartsAt = 0;
		this->decimalDigits = 0;
		isPositive = true;
		this->parts.push_back(Bigint::ZERO_INT);
		return;
	}
	temp = temp.substr(temp.find_first_not_of("0"));
	if(temp.find(".")!=string::npos)
	{
		if(temp.find(".")!=temp.find_last_of("."))
		{
			throw "Invalid decimal Number";
		}
		else
		{
			decimalDigits = temp.length() - temp.find(".") - 1;
			decimalStartsAt = (int)temp.find(".");
			StringUtil::replaceFirst(temp, ".", "");
			temp = temp.substr(0, temp.find_last_not_of("0")+1);
		}
	}
	else
	{
		this->decimalStartsAt = 0;
		this->decimalDigits = 0;
	}
	while((int)temp.length()>Bigint::NUM_LENGTH)
	{
		try {
			int x = CastUtil::lexical_cast<int>(temp.substr(temp.length()-Bigint::NUM_LENGTH));
			temp = temp.substr(0, temp.length()-Bigint::NUM_LENGTH);
			parts.push_back(x);
		} catch(...) {
			throw "Invalid Bigdecimal value";
		}
	}
	if(temp.length()>0)
	{
		try {
			int x = CastUtil::lexical_cast<int>(temp);
			parts.push_back(x);
		} catch(...) {
			throw "Invalid Bigdecimal value";
		}
	}
	if(parts.at(parts.size()-1)==0)
		parts.erase(parts.begin()+(parts.size()-1));
	checkAndSetIfZero();
}

void Bigdecimal::checkAndSetIfZero()
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
		this->parts.push_back(Bigint::ZERO_INT);
		isPositive = true;
	}
}

void Bigdecimal::add(Bigdecimal number)
{
	string fval = toString();
	string sval = number.toString();
	int maxdecdigits = decimalDigits>number.decimalDigits?decimalDigits:number.decimalDigits;
	for (int j = 0; j < maxdecdigits-decimalDigits; j++) {
		fval.append(Bigint::ZERO);
	}
	for (int j = 0; j < maxdecdigits-number.decimalDigits; j++) {
		sval.append(Bigint::ZERO);
	}
	StringUtil::replaceFirst(fval, ".", "");
	StringUtil::replaceFirst(sval, ".", "");
	Bigint fnum(fval);
	Bigint snum(sval);
	fnum.add(sval);
	this->parts = fnum.parts;
	this->isPositive = fnum.isPositive;
	decimalDigits = (decimalDigits>number.decimalDigits?decimalDigits:number.decimalDigits);
}

Bigdecimal Bigdecimal::operator+(Bigdecimal number)
{
	Bigdecimal temp = *this;
	temp.add(number);
	return temp;
}

Bigdecimal Bigdecimal::operator-(Bigdecimal number)
{
	Bigdecimal temp = *this;
	temp.subtract(number);
	return temp;
}

Bigdecimal Bigdecimal::operator*(Bigdecimal number)
{
	Bigdecimal temp = *this;
	temp.multiply(number);
	return temp;
}

Bigdecimal Bigdecimal::operator/(Bigdecimal number)
{
	Bigdecimal temp = *this;
	temp.divide(number);
	return temp;
}

Bigdecimal& Bigdecimal::operator++()
{
	Bigdecimal temp("1");
	this->add(temp);
	return *this;
}

Bigdecimal& Bigdecimal::operator+=(Bigdecimal number)
{
	this->add(number);
	return *this;
}

Bigdecimal& Bigdecimal::operator--()
{
	Bigdecimal temp("1");
	this->subtract(temp);
	return *this;
}

Bigdecimal& Bigdecimal::operator-=(Bigdecimal number)
{
	this->subtract(number);
	return *this;
}

bool operator==(Bigdecimal &lhs, Bigdecimal &rhs)
{
	if(lhs.compare(rhs)==0)
		return true;
	return false;
}

bool operator!=(Bigdecimal &lhs, Bigdecimal &rhs)
{
	if(lhs.compare(rhs)!=0)
		return true;
	return false;
}

bool operator<(Bigdecimal &lhs, Bigdecimal &rhs)
{
	if(lhs.compare(rhs)==-1)
		return true;
	return false;
}

bool operator<=(Bigdecimal &lhs, Bigdecimal &rhs)
{
	if(lhs.compare(rhs)<=0)
		return true;
	return false;
}

bool operator>(Bigdecimal &lhs, Bigdecimal &rhs)
{
	if(lhs.compare(rhs)==1)
		return true;
	return false;
}

bool operator>=(Bigdecimal &lhs, Bigdecimal &rhs)
{
	if(lhs.compare(rhs)>=0)
		return true;
	return false;
}

void Bigdecimal::subtract(Bigdecimal number)
{
	string fval = toString();
	string sval = number.toString();
	int maxdecdigits = decimalDigits>number.decimalDigits?decimalDigits:number.decimalDigits;
	for (int j = 0; j < maxdecdigits-decimalDigits; j++) {
		fval.append(Bigint::ZERO);
	}
	for (int j = 0; j < maxdecdigits-number.decimalDigits; j++) {
		sval.append(Bigint::ZERO);
	}
	StringUtil::replaceFirst(fval, ".", "");
	StringUtil::replaceFirst(sval, ".", "");
	Bigint fnum(fval);
	Bigint snum(sval);
	fnum.subtract(sval);
	this->parts = fnum.parts;
	this->isPositive = fnum.isPositive;
	decimalDigits = (decimalDigits>number.decimalDigits?decimalDigits:number.decimalDigits);
}

void Bigdecimal::multiply(Bigdecimal number)
{
	string fval = toString();
	string sval = number.toString();
	int maxdecdigits = decimalDigits>number.decimalDigits?decimalDigits:number.decimalDigits;
	for (int j = 0; j < maxdecdigits-decimalDigits; j++) {
		fval.append(Bigint::ZERO);
	}
	for (int j = 0; j < maxdecdigits-number.decimalDigits; j++) {
		sval.append(Bigint::ZERO);
	}
	StringUtil::replaceFirst(fval, ".", "");
	StringUtil::replaceFirst(sval, ".", "");
	Bigint fnum(fval);
	Bigint snum(sval);
	fnum.multiply(sval);
	this->parts = fnum.parts;
	this->isPositive = fnum.isPositive;
	decimalDigits += number.decimalDigits;
}

void Bigdecimal::divide(Bigdecimal number, int precision)
{
	string fval = toString();
	string sval = number.toString();
	int maxdecdigits = decimalDigits>number.decimalDigits?decimalDigits:number.decimalDigits;
	for (int j = 0; j < maxdecdigits-decimalDigits; j++) {
		fval.append(Bigint::ZERO);
	}
	for (int j = 0; j < maxdecdigits-number.decimalDigits; j++) {
		sval.append(Bigint::ZERO);
	}
	StringUtil::replaceFirst(fval, ".", "");
	StringUtil::replaceFirst(sval, ".", "");
	Bigint fnum(fval);
	Bigint snum(sval);
	fnum.internalDivide(sval, true, precision-1);
	this->parts = fnum.parts;
	this->isPositive = fnum.isPositive;
	this->decimalStartsAt = fnum.decimalStartsAt;
	decimalDigits -= number.decimalDigits;
}


int Bigdecimal::compare(Bigdecimal number1, Bigdecimal number2)
{
	return number1.compare(number2);
}

int Bigdecimal::compare(Bigdecimal number)
{
	if(isPositive==number.isPositive)
	{
		string fnvalue = toString();
		string snvalue = number.toString();
		Bigint fnum, snum;
		if(decimalStartsAt>0)
		{
			Bigint temp(fnvalue.substr(0, decimalStartsAt));
			fnum = temp;
		}
		else
		{
			Bigint temp(fnvalue);
			fnum = temp;
		}
		if(number.decimalStartsAt>0)
		{
			Bigint temp(snvalue.substr(0, number.decimalStartsAt));
			snum = temp;
		}
		else
		{
			Bigint temp(snvalue);
			snum = temp;
		}
		int compVal = fnum.compare(snum);
		if(compVal==0)
		{
			string fmantstr = fnvalue.substr(decimalStartsAt+1, decimalDigits);
			string smantstr = snvalue.substr(number.decimalStartsAt+1, number.decimalDigits);
			int maxdecdigits = decimalDigits>number.decimalDigits?decimalDigits:number.decimalDigits;
			for (int j = 0; j < maxdecdigits-decimalDigits; j++) {
				fmantstr.append(Bigint::ZERO);
			}
			for (int j = 0; j < maxdecdigits-number.decimalDigits; j++) {
				smantstr.append(Bigint::ZERO);
			}
			Bigint fmantissa(fmantstr);
			Bigint smantissa(smantstr);
			return fmantissa.compare(smantissa);
		}
		return compVal;
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

string Bigdecimal::toString()
{
	if(parts.size()==0)
		return Bigint::ZERO;
	string build;
	vector<int> nparts = parts;
	std::reverse(nparts.begin(),nparts.end());
	if(!isPositive)
	{
		build.append(Bigint::MINUS);
	}
	for (int i=0;i<(int)nparts.size();i++) {
		if(i!=0)
		{
			string numstr = CastUtil::lexical_cast<string>(nparts.at(i));
			for (int j = 0; j < Bigint::NUM_LENGTH-(int)numstr.length(); j++) {
				build.append(Bigint::ZERO);
			}
			build.append(numstr);
		}
		else
		{
			build.append(CastUtil::lexical_cast<string>(nparts.at(i)));
		}
	}
	if(decimalDigits>0)
	{
		build = build.substr(0, build.find_last_not_of("0")+1);
		build = build.substr(0, build.length()-decimalDigits) + "." + build.substr(build.length()-decimalDigits);;
	}
	else if(decimalStartsAt>0)
	{
		build = build.substr(0, decimalStartsAt) + "." + build.substr(decimalStartsAt);
	}
	return build;
}
