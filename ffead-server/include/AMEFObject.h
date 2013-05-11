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

	/*The Length of the Object value*/
	int length;

	/*The Length of the Object value*/
	int namedLength;

	/*The Object value in string format*/
	string value;

	/*The properties of a complex object*/
	vector<AMEFObject*> packets;

	static string longTocharArray(long l,int ind);
	static string intTocharArray(int l,int ind);
	static char* intTocharArrayWI(int l);
	static int charArrayToInt(string l);
	static int charArrayToInt(string l,int off,int ind);
	static long charArrayToLong(string l);
	static long charArrayToLong(string l,int off,int ind);
	static long charArrayToLong(string l,int ind);
	static string intTocharArrayS(int l, int ind);
public:
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

	bool isNull();

	/**
	 * @return Array of JDBObjectNew
	 *
	 */

	static char getEqvNullType(char type);
	void clear();
	/*Create a new AMEF object which will initilaize the values*/
	AMEFObject();
	~AMEFObject();

	void addNullPacket(char type);
	void addNullPacket(char type,string name);
	/**
	 * @param string
	 * @param name
	 * Add a string property to an Object
	 */
	void addPacket(const string& stringa,const string& name);
	/**
	 * @param string
	 * Add a string property to an Object
	 */
	AMEFObject* addPacket(const string& stringa);

	/**
	 * @param string
	 * @param name
	 * Add a string property to an Object
	 */
	void addPacket(char* stringa,string name);
	/**
	 * @param string
	 * Add a string property to an Object
	 */
	AMEFObject* addPacket(char* stringa);

	/**
	 * @param bool
	 * @param name
	 * Add a bool  property to an Object
	 */
	void addPacket(bool boole,string name);
	/**
	 * @param bool
	 * Add a bool  property to an Object
	 */
	AMEFObject* addPacket(bool boole);

	void addPacket(char chr,string name);
	/**
	 * @param bool
	 * Add a bool  property to an Object
	 */
	AMEFObject* addPacket(char chr);

	AMEFObject* addPacket(char value, char type);
	/**
	 * @param lon
	 * @param name
	 * Add a long property to an Object
	 */
	void addPacket(long lon,string name);
	/**
	 * @param lon
	 * Add a long property to an Object
	 */
	#ifdef IS_64_BIT
		AMEFObject* addPacket(unsigned long long lon);
		AMEFObject* addPacket(long lon);
	#else
		AMEFObject* addPacket(unsigned long long lon);
		AMEFObject* addPacket(long lon);
	#endif
	/**
	 * @param doub
	 * @param name
	 * Add a double property to an Object
	 */
	void addPacket(float doub,string name);
	/**
	 * @param doub
	 * Add a double property to an Object
	 */
	AMEFObject* addPacket(float doub);


	/**
	 * @param doub
	 * @param name
	 * Add a double property to an Object
	 */
	void addPacket(double doub,string name);
	/**
	 * @param doub
	 * Add a double property to an Object
	 */
	AMEFObject* addPacket(double doub);

	/**
	 * @param integer
	 * @param name
	 * Add an integer property to an Object
	 */
	void addPacket(int integer,string name);
	/**
	 * @param integer
	 * Add an integer property to an Object
	 */
	AMEFObject* addPacket(int integer);

	/**
	 * @param date
	 * @param name
	 * Add a Date property to an Object
	 */
	/*void addPacket(Date date,string name)
	{
		AMEFObject* JDBObjectNew = addPacket(date);
		JDBObjectNew->name = name;
		namedLength += name.length();
		JDBObjectNew->namedLength += name.length();
	}*/
	/**
	 * @param date
	 * Add a Date property to an Object
	 */
	/*AMEFObject* addPacket(Date date)
	{
		AMEFObject* JDBObjectNew = new AMEFObject();
		JDBObjectNew->type = DATE_TYPE;
		JDBObjectNew->name = "";
		SimpleDateFormat format = new SimpleDateFormat("ddMMyyyy HHmmss");
		JDBObjectNew->length = 15;
		JDBObjectNew->value = format.format(date).getchars();
		JDBObjectNew->namedLength += JDBObjectNew->value.length;
		length += JDBObjectNew->value.length + 2;
		namedLength += JDBObjectNew->value.length + 4;
		JDBObjectNew->namedLength  = JDBObjectNew->value.length + 2;
		JDBObjectNew->length = JDBObjectNew->value.length;
		packets.push_back(JDBObjectNew);
		return JDBObjectNew;
	}*/

	/**
	 * @param packet
	 * Add a JDBObjectNew property to an Object
	 */
	void addPacket(AMEFObject *packet);

	/*void set(int i,AMEFObject jdbo)
	{
		packets.set(i,jdbo);
	}*/

	/**
	 * @param packet
	 * Add a JDBObjectNew property to an Object
	 */
	void addPacket(char* packet,char type);

	/*void addPacket(Object obj)
	{
		if(obj instanceof Long)
			addPacket(((Long)obj).longValue());
		else if(obj instanceof Double)
			addPacket(((Double)obj).doubleValue());
		else if(obj instanceof string)
			addPacket((string)obj);
		else if(obj instanceof char)
			addPacket(((char)obj).charValue());
	}*/

	int getlength();

	int getLength();


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

	int getNamedLength(bool  ignoreName);

	void setLength(int length);

	char* getName();
	string getNameStr();
	void setName(const string& name);
	/*void setName(char* name)
	{
		this->name = name;
	}*/
	vector<AMEFObject*> getPackets();
	void setPackets(vector<AMEFObject*> packets);

	char getType();
	void setType(char type);

	string getValue();
	/*Object getTValue()
	{
		if(type=='s' || type=='t' || type=='d' || type=='h' || type=='y' || type=='u')
			return value;
		else if(getType()!='n' && getType()!='w' && getType()!='r'
				&& getType()!='i')
		{
			return getIntValue();
		}
		else if(getType()!='f' && getType()!='x'
					&& getType()!='e' && getType()!='l')
		{
			return getLongValue();
		}
		else if(getType()!='b')
		{
			return getBooleanValue();
		}
		else if(getType()!='c')
		{
			return (char)value[0];
		}
		return this;
	}*/
	string getValueStr();
	void pushChar(char v);
	void setValue(char* value);
	void setValue(char *value,int len);
	void setValue(const string& value);

	/**
	 * @return bool  value of this object if its type is boolean
	 */
	bool getBoolValue();

	/**
	 * @return integer value of this object if its type is integer
	 */
	int getIntValue();

	/**
	 * @return integer value of this object if its type is integer
	 */
	short getShortValue();

	/**
	 * @return double value of this object if its type is double
	 */
	double getDoubleValue();

	/**
	 * @return double value of this object if its type is double
	 */
	float getFloatValue();
	/**
	 * @return long value of this object if its type is long
	 */
	long getLongValue();
	long getNumericValue();
	/**
	 * @return Date value of this object if its type is Date
	 */
	/*Date getDateValue()
	{
		if(type=='b')
		{
			try
			{
				return new SimpleDateFormat("ddMMyyyy HHmmss").parse(new string(value));
			}
			catch (ParseException e)
			{
				return new Date();
			}
		}
		else
			return new Date();
	}*/

	string tostring();
	string displayObject(string tab);
	void addStaticPacket(AMEFObject *obj);
};


#endif /* AMEFOBJECT_H_ */
