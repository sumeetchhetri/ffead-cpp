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

#include "AMEFObject.h"
#include "iostream"


AMEFObject::~AMEFObject() {
	for (int var = 0; var < (int)packets.size(); var++) {
		AMEFObject *ob = packets.at(var);
		delete ob;
	}
	clear();
}

string AMEFObject::intTocharArray(int l, int ind)
{
	string result;
	for (int i = 0; i<ind; i++)
	{
		int offset = (ind - 1 - i) * 8;
		result.push_back((char) ((l >> offset) & 0xFF));
	}
	return result;
}

string AMEFObject::ulonglongTocharArray(unsigned long long lon)
{
	int ind;
	if(lon<256)
	{
		ind = 1;
	}
	else if(lon<65536)
	{
		ind = 2;
	}
	else if(lon<16777216)
	{
		ind = 3;
	}
	else if(lon<4294967296ULL)
	{
		ind = 4;
	}
	else if(lon<1099511627776ULL)
	{
		ind = 5;
	}
	else if(lon<281474976710656ULL)
	{
		ind = 6;
	}
	else if(lon<72057594037927936ULL)
	{
		ind = 7;
	}
	else
	{
		ind = 8;
	}
	string result;
	for (int i = 0; i<ind; i++)
	{
		int offset = (ind - 1 - i) * 8;
		result.push_back((char) ((lon >> offset) & 0xFF));
	}
	return result;
}

string AMEFObject::ulonglongTocharArrayWithLI(unsigned long long lon)
{
	int ind;
	if(lon<256)
	{
		ind = 1;
	}
	else if(lon<65536)
	{
		ind = 2;
	}
	else if(lon<16777216)
	{
		ind = 3;
	}
	else if(lon<4294967296ULL)
	{
		ind = 4;
	}
	else if(lon<1099511627776ULL)
	{
		ind = 5;
	}
	else if(lon<281474976710656ULL)
	{
		ind = 6;
	}
	else if(lon<72057594037927936ULL)
	{
		ind = 7;
	}
	else
	{
		ind = 8;
	}
	string result;
	result.push_back((char)ind);
	for (int i = 0; i<ind; i++)
	{
		int offset = (ind - 1 - i) * 8;
		result.push_back((char) ((lon >> offset) & 0xFF));
	}
	return result;
}

int AMEFObject::charArrayToInt(string l,int off,int ind)
{
	int t = 0;
	for (int i = 0; i < ind; i++)
	{
		t = (t << 8) |(unsigned char) l[off+i];
	}
	return t;
}

unsigned long long AMEFObject::charArrayToULongLong(string l)
{
	unsigned long long t = 0;
	int ind = l.length();
	for (int i = 0; i < ind; i++)
	{
		t = (t << 8) |(unsigned char) l[i];
	}
	return t;
}

unsigned long long AMEFObject::charArrayToULongLong(string l, int ind)
{
	unsigned long long t = 0;
	for (int i = 0; i < ind; i++)
	{
		t = (t << 8) |(unsigned char) l[i];
	}
	return t;
}

bool AMEFObject::isNull()
{
	if(type=='a' || type=='g' || type=='j' || type=='k' || type=='v' || type=='z')
		return true;
	else
		return false;
}

/**
 * @return Array of JDBObjectNew
 *
 */

char AMEFObject::getEqvNullType(char type)
{
	if(type=='s' || type=='t' || type=='h' || type=='y')
		return NULL_STRING;
	else if(type=='n' || type=='w' || type=='r' || type=='q'
			|| type=='f' || type=='x' || type=='e' || type=='l')
		return NULL_NUMBER;
	else if(type=='d')
		return NULL_DATE;
	else if(type=='b')
		return NULL_BOOL;
	else if(type=='c')
		return NULL_CHAR;
	else
		return 0;
}


void AMEFObject::clear()
{
	this->packets.clear();
}

/*Create a new AMEF object which will initilaize the values*/
AMEFObject::AMEFObject()
{
	type = OBJECT_TYPE;
	name = "";
}

void AMEFObject::addNullPacket(char type)
{
	AMEFObject* JDBObjectNew = new AMEFObject();
	JDBObjectNew->type = type;
	packets.push_back(JDBObjectNew);
}

void AMEFObject::addNullPacket(char type, string name)
{
	AMEFObject* JDBObjectNew = new AMEFObject();
	JDBObjectNew->name = name;
	JDBObjectNew->type = type;
	packets.push_back(JDBObjectNew);
}

/**
 * @param string
 * @param name
 * Add a string property to an Object
 */
void AMEFObject::addPacket(string stringa, const string name)
{
	AMEFObject* JDBObjectNew = addStringPacket(stringa);
	JDBObjectNew->name = name;
}
/**
 * @param string
 * Add a string property to an Object
 */
AMEFObject* AMEFObject::addStringPacket(string stringa)
{
	AMEFObject* JDBObjectNew = new AMEFObject();
	JDBObjectNew->name = "";

	if(stringa.length()<=256)
	{
		JDBObjectNew->type = STRING_256_TYPE;
	}
	else if(stringa.length()<=65536)
	{
		JDBObjectNew->type = STRING_65536_TYPE;
	}
	else if(stringa.length()<=16777216)
	{
		JDBObjectNew->type = STRING_16777216_TYPE;
	}
	else
	{
		JDBObjectNew->type = STRING_TYPE;
	}
	JDBObjectNew->value = stringa;
	packets.push_back(JDBObjectNew);
	return JDBObjectNew;
}

/**
 * @param string
 * @param name
 * Add a string property to an Object
 */
void AMEFObject::addPacket(char* stringa, const string name)
{
	AMEFObject* JDBObjectNew = addCharStringPacket(stringa);
	JDBObjectNew->name = name;
}
/**
 * @param string
 * Add a string property to an Object
 */
AMEFObject* AMEFObject::addCharStringPacket(char* stringa)
{
	AMEFObject* JDBObjectNew = new AMEFObject();
	JDBObjectNew->name = "";

	if(strlen(stringa)<=256)
	{
		JDBObjectNew->type = STRING_256_TYPE;
	}
	else if(strlen(stringa)<=65536)
	{
		JDBObjectNew->type = STRING_65536_TYPE;
	}
	else if(strlen(stringa)<=16777216)
	{
		JDBObjectNew->type = STRING_16777216_TYPE;
	}
	else
	{
		JDBObjectNew->type = STRING_TYPE;
	}
	JDBObjectNew->value = stringa;
	packets.push_back(JDBObjectNew);
	return JDBObjectNew;
}

/**
 * @param bool
 * @param name
 * Add a bool  property to an Object
 */
void AMEFObject::addPacket(bool boole, const string name)
{
	AMEFObject* JDBObjectNew = addBoolPacket(boole);
	JDBObjectNew->name = name;
}
/**
 * @param bool
 * Add a bool  property to an Object
 */
AMEFObject* AMEFObject::addBoolPacket(bool boole)
{
	AMEFObject* JDBObjectNew = new AMEFObject();
	JDBObjectNew->type = BOOLEAN_TYPE;
	JDBObjectNew->name = "";
	if(boole==true)
	{
		JDBObjectNew->value = "1";
	}
	else
	{
		JDBObjectNew->value = "0";
	}
	packets.push_back(JDBObjectNew);
	return JDBObjectNew;
}

void AMEFObject::addPacket(char chr, const string name)
{
	AMEFObject* JDBObjectNew = addCharPacket(chr);
	JDBObjectNew->name = name;
}

void AMEFObject::addPacket(unsigned char chr, const string name)
{
	AMEFObject* JDBObjectNew = addCharPacket(chr);
	JDBObjectNew->name = name;
}

AMEFObject* AMEFObject::addCharPacket(char chr)
{
	AMEFObject* JDBObjectNew = new AMEFObject();
	JDBObjectNew->type = 'c';
	JDBObjectNew->name = "";
	JDBObjectNew->value.push_back(chr);
	packets.push_back(JDBObjectNew);
	return JDBObjectNew;
}

void AMEFObject::addPacket(short lon, const string name)
{
	AMEFObject* JDBObjectNew = addULLPacket((unsigned long long)lon);
	JDBObjectNew->name = name;
}

void AMEFObject::addPacket(unsigned short lon, const string name)
{
	AMEFObject* JDBObjectNew = addULLPacket((unsigned long long)lon);
	JDBObjectNew->name = name;
}

void AMEFObject::addPacket(int lon, const string name)
{
	AMEFObject* JDBObjectNew = addULLPacket((unsigned long long)lon);
	JDBObjectNew->name = name;
}

void AMEFObject::addPacket(unsigned int lon, const string name)
{
	AMEFObject* JDBObjectNew = addULLPacket((unsigned long long)lon);
	JDBObjectNew->name = name;
}

void AMEFObject::addPacket(long lon, const string name)
{
	AMEFObject* JDBObjectNew = addULLPacket((unsigned long long)lon);
	JDBObjectNew->name = name;
}

void AMEFObject::addPacket(unsigned long lon, const string name)
{
	AMEFObject* JDBObjectNew = addULLPacket((unsigned long long)lon);
	JDBObjectNew->name = name;
}

void AMEFObject::addPacket(long long lon, const string name)
{
	AMEFObject* JDBObjectNew = addULLPacket((unsigned long long)lon);
	JDBObjectNew->name = name;
}

void AMEFObject::addPacket(unsigned long long lon, const string name)
{
	AMEFObject* JDBObjectNew = addULLPacket(lon);
	JDBObjectNew->name = name;
}

AMEFObject* AMEFObject::addULLPacket(unsigned long long lon)
{
	AMEFObject* JDBObjectNew = new AMEFObject();
	JDBObjectNew->value = ulonglongTocharArray(lon);
	if(lon<256)
	{
		JDBObjectNew->type = VERY_SMALL_INT_TYPE;
	}
	else if(lon<65536)
	{
		JDBObjectNew->type = SMALL_INT_TYPE;
	}
	else if(lon<16777216)
	{
		JDBObjectNew->type = BIG_INT_TYPE;
	}
	else if(lon<4294967296ULL)
	{
		JDBObjectNew->type = INT_TYPE;
	}
	else if(lon<1099511627776ULL)
	{
		JDBObjectNew->type = VS_LONG_INT_TYPE;
	}
	else if(lon<281474976710656ULL)
	{
		JDBObjectNew->type = S_LONG_INT_TYPE;
	}
	else if(lon<72057594037927936ULL)
	{
		JDBObjectNew->type = B_LONG_INT_TYPE;
	}
	else
	{
		JDBObjectNew->type = LONG_INT_TYPE;
	}
	JDBObjectNew->name = "";
	packets.push_back(JDBObjectNew);
	return JDBObjectNew;
}

/**
 * @param doub
 * @param name
 * Add a double property to an Object
 */
void AMEFObject::addPacket(float doub, const string name)
{
	AMEFObject* JDBObjectNew = addFloatPacket(doub);
	JDBObjectNew->name = name;
}
/**
 * @param doub
 * Add a double property to an Object
 */
AMEFObject* AMEFObject::addFloatPacket(float doub)
{
	AMEFObject* JDBObjectNew = new AMEFObject();
	JDBObjectNew->type = DOUBLE_FLOAT_TYPE;
	JDBObjectNew->name = "";
	JDBObjectNew->value = CastUtil::lexical_cast<string>(doub);
	packets.push_back(JDBObjectNew);
	return JDBObjectNew;
}



void AMEFObject::addPacket(double doub, const string name)
{
	AMEFObject* JDBObjectNew = addDoublePacket(doub);
	JDBObjectNew->name = name;
}

void AMEFObject::addPacket(long double doub, const string name)
{
	AMEFObject* JDBObjectNew = addLDoublePacket(doub);
	JDBObjectNew->name = name;
}

AMEFObject* AMEFObject::addLDoublePacket(long double doub)
{
	AMEFObject* JDBObjectNew = new AMEFObject();
	JDBObjectNew->type = DOUBLE_FLOAT_TYPE;
	JDBObjectNew->name = "";
	JDBObjectNew->value = CastUtil::lexical_cast<string>(doub);
	packets.push_back(JDBObjectNew);
	return JDBObjectNew;
}


AMEFObject* AMEFObject::addDoublePacket(double doub)
{
	AMEFObject* JDBObjectNew = new AMEFObject();
	JDBObjectNew->type = DOUBLE_FLOAT_TYPE;
	JDBObjectNew->name = "";
	JDBObjectNew->value = CastUtil::lexical_cast<string>(doub);
	packets.push_back(JDBObjectNew);
	return JDBObjectNew;
}

void AMEFObject::addPacket(AMEFObject *packet)
{
	packets.push_back(packet);
}


bool AMEFObject::isFloatingPoint(char type)
{
	if(type=='u' || type=='k')
		return true;
	return false;
}

bool AMEFObject::isNumber(char type)
{
	if(type=='n' || type=='w' || type=='r' || type=='i'
			|| type=='f' || type=='x' || type=='e' || type=='l' || type=='g')
		return true;
	return false;
}

bool AMEFObject::isInteger(char type)
{
	if(type=='n' || type=='w' || type=='r' || type=='i')
		return true;
	return false;
}

bool AMEFObject::isLong(char type)
{
	if(type=='n' || type=='w' || type=='r' || type=='i'
			|| type=='f' || type=='x' || type=='e' || type=='l')
		return true;
	return false;
}

bool AMEFObject::isChar(char type)
{
	if(type=='c' ||  type=='z')
		return true;
	return false;
}

bool AMEFObject::isBoolean(char type)
{
	if(type=='b' || type=='v')
		return true;
	return false;
}

bool AMEFObject::isDate(char type)
{
	if(type=='d' || type=='j')
		return true;
	return false;
}

bool  AMEFObject::isStringOrNullString()
{
	if(type=='s' || type=='t' || type=='h' || type=='y' || type=='a')
		return true;
	return false;
}

bool  AMEFObject::isString()
{
	if(type=='s' || type=='t' || type=='h' || type=='y')
		return true;
	return false;
}

bool  AMEFObject::isFloatingPoint()
{
	if(type=='u' || type=='k')
		return true;
	return false;
}

bool  AMEFObject::isNumber()
{
	if(type=='n' || type=='w' || type=='r' || type=='i'
			|| type=='f' || type=='x' || type=='e' || type=='l' || type=='g')
		return true;
	return false;
}

bool  AMEFObject::isNumberOrNullNumber()
{
	if(type=='n' || type=='w' || type=='r' || type=='i'
			|| type=='f' || type=='x' || type=='e' || type=='l')
		return true;
	return false;
}

bool  AMEFObject::isChar()
{
	if(type=='b' || type=='c' || type=='v' || type=='z')
		return true;
	return false;
}

bool  AMEFObject::isDate()
{
	if(type=='d' || type=='j')
		return true;
	return false;
}

char* AMEFObject::getName()
{
	return (char*)name.c_str();
}
string AMEFObject::getNameStr()
{
	return name;
}
void AMEFObject::setName(const string& name)
{
	this->name = name;
}

vector<AMEFObject*> AMEFObject::getPackets()
{
	return packets;
}
void AMEFObject::setPackets(vector<AMEFObject*> packets)
{
	this->packets = packets;
}

char AMEFObject::getType()
{
	return type;
}
void AMEFObject::setType(char type)
{
	this->type = type;
}

string AMEFObject::getValue()
{
	return value;
}

string AMEFObject::getValueStr()
{
	return value;
}
void AMEFObject::pushChar(char v)
{
	this->value.push_back(v);
}
void AMEFObject::setValue(char* value)
{
	int len = strlen(value);
	for (int var = 0; var < len; var++) {
		this->value.push_back(value[var]);
	}
}
void AMEFObject::setValue(char *value,int len)
{
	this->value.append(value,len);
}
void AMEFObject::setValue(const string& value)
{
	this->value = value;
}

char AMEFObject::getCharValue()
{
	if(type=='c')
		return value[0];
	else
		return '\0';
}

unsigned char AMEFObject::getUCharValue()
{
	if(type=='c')
		return (unsigned char)value[0];
	else
		return '\0';
}

/**
 * @return bool  value of this object if its type is boolean
 */
bool AMEFObject::getBoolValue()
{
	if(type=='b')
		return (value[0]=='1'?true:false);
	else
		return false;
}

/**
 * @return integer value of this object if its type is integer
 */
int AMEFObject::getIntValue()
{
	if(type=='n' || type=='w' || type=='r' || type=='i')
	{
		return (int)charArrayToULongLong(value);
	}
	else
		return -1;
}

unsigned int AMEFObject::getUIntValue()
{
	if(type=='n' || type=='w' || type=='r' || type=='i')
	{
		return (unsigned int)charArrayToULongLong(value);
	}
	else
		return -1;
}

/**
 * @return integer value of this object if its type is integer
 */
short AMEFObject::getShortValue()
{
	if(type=='n' || type=='w' || type=='r' || type=='i')
	{
		return (short)charArrayToULongLong(value);
	}
	else
		return -1;
}

unsigned short AMEFObject::getUShortValue()
{
	if(type=='n' || type=='w' || type=='r' || type=='i')
	{
		return (unsigned short)charArrayToULongLong(value);
	}
	else
		return -1;
}

double AMEFObject::getDoubleValue()
{
	if(type=='u')
		return (CastUtil::lexical_cast<double>(getValueStr()));
	else
		return -1;
}

long double AMEFObject::getLongDoubleValue()
{
	if(type=='u')
		return (CastUtil::lexical_cast<long double>(getValueStr()));
	else
		return -1;
}

/**
 * @return double value of this object if its type is double
 */
float AMEFObject::getFloatValue()
{
	if(type=='u')
		return (CastUtil::lexical_cast<float>(getValueStr()));
	else
		return -1;
}

/**
 * @return long value of this object if its type is long
 */
long AMEFObject::getLongValue()
{
	if(type=='f' || type=='x' || type=='e' || type=='l'
			|| type=='n' || type=='w' || type=='r' || type=='i')
	{
		return (long)charArrayToULongLong(value);
	}
	else
		return -1;
}

unsigned long AMEFObject::getULongValue()
{
	if(type=='f' || type=='x' || type=='e' || type=='l'
			|| type=='n' || type=='w' || type=='r' || type=='i')
	{
		return (unsigned long)charArrayToULongLong(value);
	}
	else
		return -1;
}

long long AMEFObject::getLongLongValue()
{
	if(type=='f' || type=='x' || type=='e' || type=='l'
			|| type=='n' || type=='w' || type=='r' || type=='i')
	{
		return (long long)charArrayToULongLong(value);
	}
	else
		return -1;
}

unsigned long long AMEFObject::getULongLongValue()
{
	if(type=='f' || type=='x' || type=='e' || type=='l'
			|| type=='n' || type=='w' || type=='r' || type=='i')
	{
		return charArrayToULongLong(value);
	}
	else
		return -1;
}

unsigned long long AMEFObject::getNumericValue()
{
	if(type=='f' || type=='x' || type=='e' || type=='l'
			|| type=='n' || type=='w' || type=='r' || type=='i')
	{
		return charArrayToULongLong(value);
	}
	else
		return -1;
}


string AMEFObject::tostring()
{
	return displayObject("\t");
}

string AMEFObject::displayObject(string tab)
{
	string displ = "Object Type = ";
	displ += type;
	displ += "\nObject Name = " + name + "\n";
	for (int i=0;i<(int)getPackets().size();i++)
	{
		AMEFObject* obj = getPackets().at(i);
		displ += tab + "Object Type = ";
		displ += obj->type;
		displ += "\n" + tab + "Object Name = " + obj->name + "\n";
		displ += tab + "Object Value = ";
		if(obj->isStringOrNullString() || obj->isFloatingPoint() || obj->isDate())
			displ += (obj->getValueStr()) + "\n";
		else if(obj->isChar())
		{
			if(obj->type=='b')
				displ += CastUtil::lexical_cast<string>(obj->getBoolValue()) + "\n";
			else
				displ += (char)obj->value[0] + "\n";
		}
		else if(obj->isNumberOrNullNumber())
		{
			displ += CastUtil::lexical_cast<string>(obj->getNumericValue()) + "\n";
		}
		if(obj->type=='o' || obj->type=='p' || obj->type=='q' || obj->type=='m')
		{
			displ += obj->displayObject(tab+"\t");
		}
	}
	return displ;
}

void AMEFObject::addStaticPacket(AMEFObject *obj)
{
	packets.push_back(obj);
}
