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

#ifndef AMEFOBJECT_H_
#define AMEFOBJECT_H_

#include "string"
#include "cstring"
#include "vector"
#include "CastUtil.h"
#include "Constants.h"
using namespace std;

class AMEFObject
{
	/*The type of the Object can be string, number, date, boolean, character or any complex object*/
	char type;

	/*The name of the Object if required can be used to represent object properties*/
	string name;

	/*The Object value in string format*/
	string value;

	/*The properties of a complex object*/
	vector<AMEFObject*> packets;

	AMEFObject* addStringPacket(string stringa);
	AMEFObject* addCharStringPacket(char* stringa);
	AMEFObject* addBoolPacket(bool boole);
	AMEFObject* addCharPacket(char chr);
	AMEFObject* addULLPacket(unsigned long long lon);
	AMEFObject* addFloatPacket(float doub);
	AMEFObject* addDoublePacket(double doub);
	AMEFObject* addLDoublePacket(long double doub);
public:
	static int charArrayToInt(string l,int off,int ind);
	static string intTocharArray(int l, int ind);
	static string ulonglongTocharArray(unsigned long long lon);
	static unsigned long long charArrayToULongLong(string l);
	static unsigned long long charArrayToULongLong(string l, int);
	static string ulonglongTocharArrayWithLI(unsigned long long lon);

	static const char NULL_STRING = 'a';

	static const char NULL_NUMBER = 'g';

	static const char NULL_DATE = 'j';

	static const char NULL_FPN = 'k';

	static const char NULL_BOOL = 'v';

	static const char NULL_CHAR = 'z';

	/*The Date type*/
	static const char DATE_TYPE = 'd';

	/*The 4GB string type*/
	static const char STRING_TYPE = 's';

	/*The max 256 length string type*/
	static const char STRING_256_TYPE = 't';

	/*The max 65536 length string type*/
	static const char STRING_65536_TYPE = 'h';

	static const char STRING_16777216_TYPE = 'y';

	/*The bool  type*/
	static const char BOOLEAN_TYPE = 'b';

	/*The character type*/
	static const char CHAR_TYPE = 'c';

	/*The Number types*/
	static const char VERY_SMALL_INT_TYPE = 'n';

	static const char SMALL_INT_TYPE = 'w';

	static const char BIG_INT_TYPE = 'r';

	static const char INT_TYPE = 'i';

	static const char VS_LONG_INT_TYPE = 'f';

	static const char S_LONG_INT_TYPE = 'x';

	static const char B_LONG_INT_TYPE = 'e';

	static const char LONG_INT_TYPE = 'l';

	static const char DOUBLE_FLOAT_TYPE = 'u';

	/*The Object type*/
	static const char VS_OBJECT_TYPE = 'm';

	/*The Object type*/
	static const char S_OBJECT_TYPE = 'q';

	/*The Object type*/
	static const char B_OBJECT_TYPE = 'p';

	/*The Object type*/
	static const char OBJECT_TYPE = 'o';

	/*Create a new AMEF object which will initilaize the values*/
	AMEFObject();

	~AMEFObject();

	bool isNull();

	void addNullPacket(char type);

	void addNullPacket(char type,string name);

	void addPacket(string stringa, const string name = "");

	void addPacket(char* stringa, const string name = "");

	void addPacket(bool boole, const string name = "");

	void addPacket(char chr, const string name = "");

	void addPacket(unsigned char chr, const string name = "");

	void addPacket(short integer, const string name = "");

	void addPacket(unsigned short integer, const string name = "");

	void addPacket(int integer, const string name = "");

	void addPacket(unsigned int integer, const string name = "");

	void addPacket(long lon, const string name = "");

	void addPacket(unsigned long lon, const string name = "");

	void addPacket(long long lon, const string name = "");

	void addPacket(unsigned long long lon, const string name = "");

	void addPacket(float doub, const string name = "");

	void addPacket(double doub, const string name = "");

	void addPacket(long double doub, const string name = "");

	void addPacket(AMEFObject *packet);

	static char getEqvNullType(char type);

	void clear();

	static bool isstring(char type);

	static bool isFloatingPoint(char type);

	static bool isNumber(char type);

	static bool isInteger(char type);

	static bool isLong(char type);

	static bool isChar(char type);

	static bool isBoolean(char type);

	static bool isDate(char type);

	bool  isStringOrNullString();

	bool  isString();

	bool  isFloatingPoint();

	bool  isNumber();

	bool  isNumberOrNullNumber();

	bool  isChar();

	bool  isDate();

	char* getName();

	string getNameStr();

	void setName(const string& name);

	vector<AMEFObject*> getPackets();

	void setPackets(vector<AMEFObject*> packets);

	char getType();

	void setType(char type);

	string getValue();

	string getValueStr();

	void pushChar(char v);

	void setValue(char* value);

	void setValue(char *value,int len);

	void setValue(const string& value);

	char getCharValue();

	unsigned char getUCharValue();

	bool getBoolValue();

	int getIntValue();

	unsigned int getUIntValue();

	short getShortValue();

	unsigned short getUShortValue();

	long getLongValue();

	unsigned long getULongValue();

	unsigned long long getNumericValue();

	long long getLongLongValue();

	unsigned long long getULongLongValue();

	double getDoubleValue();

	long double getLongDoubleValue();

	float getFloatValue();

	string tostring();

	string displayObject(string tab);

	void addStaticPacket(AMEFObject *obj);

	/*template<class T> T getObjectValue()
	{
		T t;

		string desiredType = CastUtil::getTypeName<T>();
		if(!CastUtil::isPrimitiveDataType<T>())
		{
			throw "Invalid Object value requested";
		}

		if(desiredType=="short" || desiredType=="short int" || desiredType=="signed short" || desiredType=="signed short int"
				|| desiredType=="unsigned short" || desiredType=="unsigned short int"
				|| desiredType=="signed" || desiredType=="int" || desiredType=="signed int"
				|| desiredType=="unsigned" || desiredType=="unsigned int" || desiredType=="long"
				|| desiredType=="long int" || desiredType=="signed long" || desiredType=="signed long int"
				|| desiredType=="unsigned long" || desiredType=="unsigned long int"
				|| desiredType=="long long" || desiredType=="long long int" || desiredType=="signed long long"
				|| desiredType=="signed long long int" || desiredType=="unsigned long long"
				|| desiredType=="unsigned long long int")
		{
			if((type=='f' || type=='x' || type=='e' || type=='l'
						|| type=='n' || type=='w' || type=='r' || type=='i') && value.length()<=8)
			{
			}
			else
			{
				throw "Invalid Object value requested";
			}
			if(desiredType=="short")
			{
				short v = (short)charArrayToULongLong(value);
				t = v;
			}
			else if(desiredType=="unsigned short")
			{
				unsigned short v = (unsigned short)charArrayToULongLong(value);
				t = v;
			}
			else if(desiredType=="int")
			{
				int v = (int)charArrayToULongLong(value);
				t = v;
			}
			else if(desiredType=="unsigned int")
			{
				unsigned int v = (unsigned int)charArrayToULongLong(value);
				t = v;
			}
			else if(desiredType=="long")
			{
				long v = (long)charArrayToULongLong(value);
				t = v;
			}
			else if(desiredType=="unsigned long")
			{
				unsigned long v = (unsigned long)charArrayToULongLong(value);
				t = v;
			}
			else if(desiredType=="long long")
			{
				long long v = (long long)charArrayToULongLong(value);
				t = v;
			}
			else if(desiredType=="unsigned long long")
			{
				unsigned long long v = (unsigned long long)charArrayToULongLong(value);
				t = v;
			}
		}
		else if(desiredType=="long double" || desiredType=="float" || desiredType=="double"
				|| desiredType=="string" || desiredType=="std::string")
		{
			t = CastUtil::lexical_cast<T>(getValue());
		}
		else if(desiredType=="bool")
		{
			bool v = getBoolValue();
			t = v;
		}
		return t;
	}*/
};


#endif /* AMEFOBJECT_H_ */
