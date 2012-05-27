/*
	Copyright 2011, Sumeet Chhetri

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
#include <boost/lexical_cast.hpp>
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

	static string longTocharArray(long l,int ind)
	{
		string result;
		for (int i = 0; i<ind; i++)
		{
			int offset = (ind - 1 - i) * 8;
			result.push_back((char) ((l >> offset) & 0xFF));
		}
		return result;
	}

	static string intTocharArray(int l,int ind)
	{
		string result;
		for (int i = 0; i<ind; i++)
		{
			int offset = (ind - 1 - i) * 8;
			result.push_back((char) ((l >> offset) & 0xFF));
		}
		return result;
	}

	static char* intTocharArrayWI(int l)
	{
		int ind = 1;
		if(l<256)
			ind =1;
		else if(l<65536)
			ind = 2;
		else if(l<16777216)
			ind =3;
		else
			ind =4;
		char* result = new char[ind];
		for (int i = 0; i<ind; i++)
		{
			int offset = (ind - 1 - i) * 8;
			result[i] = (char) ((l >> offset) & 0xFF);
		}
		return result;
	}

	static int charArrayToInt(string l)
	{
		int t = 0;
		int ind = l.length();
		for (int i = 0; i < ind; i++)
		{
			int offset = (ind -1 - i) * 8;
			t += (l[i] & 0x000000FF) << offset;
		}
		return t;
	}

	static int charArrayToInt(string l,int off,int ind)
	{
		int t = 0;
		for (int i = 0; i < ind; i++)
		{
			int offset = (ind -1 - i) * 8;
			t += (l[off+i] & 0x000000FF) << offset;
		}
		return t;
	}

	static long charArrayToLong(string l)
	{
		long t = 0;
		int ind = l.length();
		for (int i = 0; i < ind; i++)
		{
			int offset = (ind -1 - i) * 8;
			t += (l[i] & 0x000000FF) << offset;
		}
		return t;
	}
	static long charArrayToLong(string l,int off,int ind)
	{
		long t = 0;
		for (int i = 0; i < ind; i++)
		{
			int offset = (ind -1 - i) * 8;
			t += (l[off+i] & 0x000000FF) << offset;
		}
		return t;
	}
	static long charArrayToLong(string l,int ind)
	{
		long t = 0;
		for (int i = 0; i < ind; i++)
		{
			int offset = (ind -1 - i) * 8;
			t += (l[i] & 0x000000FF) << offset;
		}
		return t;
	}

	static string intTocharArrayS(int l, int ind)
	{
		char* result = new char[ind];
		for (int i = 0; i<ind; i++)
		{
			int offset = (ind - 1 - i) * 8;
			result[i] = (char) ((l >> offset) & 0xFF);
		}
		string tem(result);
		return tem;
	}

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

	bool isNull()
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

	static char getEqvNullType(char type)
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


	void clear()
	{
		this->packets.clear();
	}

	/*Create a new AMEF object which will initilaize the values*/
	AMEFObject()
	{
		type = OBJECT_TYPE;
		length = 0;
		name = "";
		namedLength = 0;
	}

	~AMEFObject();

	void addNullPacket(char type)
	{
		AMEFObject* JDBObjectNew = new AMEFObject();
		JDBObjectNew->namedLength += 1;
		length += 1;
		namedLength += 3;
		JDBObjectNew->type = type;
		packets.push_back(JDBObjectNew);
	}

	void addNullPacket(char type,string name)
	{
		AMEFObject* JDBObjectNew = new AMEFObject();
		JDBObjectNew->name = name;
		namedLength += name.length();
		JDBObjectNew->namedLength += name.length() + 1;
		length += 1;
		namedLength += 3;
		JDBObjectNew->type = type;
		packets.push_back(JDBObjectNew);
	}

	/**
	 * @param string
	 * @param name
	 * Add a string property to an Object
	 */
	void addPacket(string stringa,string name)
	{
		AMEFObject* JDBObjectNew = addPacket(stringa);
		JDBObjectNew->name = name;
		//length += name.length();
		namedLength += name.length();
		JDBObjectNew->namedLength += name.length();
	}
	/**
	 * @param string
	 * Add a string property to an Object
	 */
	AMEFObject* addPacket(string stringa)
	{
		AMEFObject* JDBObjectNew = new AMEFObject();
		JDBObjectNew->name = "";

		if(stringa.length()<=256)
		{
			JDBObjectNew->type = STRING_256_TYPE;
			length += stringa.length() + 2;
			namedLength += stringa.length() + 4;
			JDBObjectNew->namedLength = stringa.length() + 2;
		}
		else if(stringa.length()<=65536)
		{
			JDBObjectNew->type = STRING_65536_TYPE;
			length += stringa.length() + 3;
			namedLength += stringa.length() + 5;
			JDBObjectNew->namedLength = stringa.length() + 3;
		}
		else if(stringa.length()<=16777216)
		{
			JDBObjectNew->type = STRING_16777216_TYPE;
			length += stringa.length() + 4;
			namedLength += stringa.length() + 6;
			JDBObjectNew->namedLength = stringa.length() + 4;
		}
		else
		{
			JDBObjectNew->type = STRING_TYPE;
			length += stringa.length() + 5;
			namedLength += stringa.length() + 7;
			JDBObjectNew->namedLength = stringa.length() + 5;
		}
		JDBObjectNew->length = stringa.length();
		JDBObjectNew->value = (char*)stringa.c_str();
		packets.push_back(JDBObjectNew);
		return JDBObjectNew;
	}

	/**
	 * @param string
	 * @param name
	 * Add a string property to an Object
	 */
	void addPacket(char stringa[],string name)
	{
		AMEFObject* JDBObjectNew = addPacket(stringa);
		JDBObjectNew->name = name;
		//length += name.length();
		namedLength += name.length();
		JDBObjectNew->namedLength += name.length();
	}
	/**
	 * @param string
	 * Add a string property to an Object
	 */
	AMEFObject* addPacket(char stringa[])
	{
		AMEFObject* JDBObjectNew = new AMEFObject();
		JDBObjectNew->name = "";

		if(strlen(stringa)<=256)
		{
			JDBObjectNew->type = STRING_256_TYPE;
			length += strlen(stringa) + 2;
			namedLength += strlen(stringa) + 4;
			JDBObjectNew->namedLength = strlen(stringa) + 2;
		}
		else if(strlen(stringa)<=65536)
		{
			JDBObjectNew->type = STRING_65536_TYPE;
			length += strlen(stringa) + 3;
			namedLength += strlen(stringa) + 5;
			JDBObjectNew->namedLength = strlen(stringa) + 3;
		}
		else if(strlen(stringa)<=16777216)
		{
			JDBObjectNew->type = STRING_16777216_TYPE;
			length += strlen(stringa) + 4;
			namedLength += strlen(stringa) + 6;
			JDBObjectNew->namedLength = strlen(stringa) + 4;
		}
		else
		{
			JDBObjectNew->type = STRING_TYPE;
			length += strlen(stringa) + 5;
			namedLength += strlen(stringa) + 7;
			JDBObjectNew->namedLength = strlen(stringa) + 5;
		}
		JDBObjectNew->length = strlen(stringa);
		JDBObjectNew->value = stringa;
		packets.push_back(JDBObjectNew);
		return JDBObjectNew;
	}

	/**
	 * @param bool
	 * @param name
	 * Add a bool  property to an Object
	 */
	void addPacket(bool boole,string name)
	{
		AMEFObject* JDBObjectNew = addPacket(boole);
		JDBObjectNew->name = name;
		//length += name.length();
		namedLength += name.length();
		JDBObjectNew->namedLength += name.length();
	}
	/**
	 * @param bool
	 * Add a bool  property to an Object
	 */
	AMEFObject* addPacket(bool boole)
	{
		AMEFObject* JDBObjectNew = new AMEFObject();
		JDBObjectNew->type = BOOLEAN_TYPE;
		JDBObjectNew->name = "";
		JDBObjectNew->length = 1;
		if(boole==true)
		{
			JDBObjectNew->value = "1";
		}
		else
		{
			JDBObjectNew->value = "0";
		}
		packets.push_back(JDBObjectNew);
		length += 2;
		namedLength += 4;
		JDBObjectNew->namedLength = 2;
		return JDBObjectNew;
	}

	void addPacket(char chr,string name)
	{
		AMEFObject* JDBObjectNew = addPacket(chr);
		JDBObjectNew->name = name;
		//length += name.length();
		namedLength += name.length();
		JDBObjectNew->namedLength += name.length();
	}
	/**
	 * @param bool
	 * Add a bool  property to an Object
	 */
	AMEFObject* addPacket(char chr)
	{
		AMEFObject* JDBObjectNew = new AMEFObject();
		JDBObjectNew->type = 'c';
		JDBObjectNew->name = "";
		JDBObjectNew->length = 1;
		JDBObjectNew->value.push_back(chr);
		packets.push_back(JDBObjectNew);
		length += 2;
		namedLength += 4;
		JDBObjectNew->namedLength = 2;
		return JDBObjectNew;
	}

	AMEFObject* addPacket(char value, char type)
	{
		AMEFObject* JDBObjectNew = new AMEFObject();
		JDBObjectNew->type = type;
		JDBObjectNew->name = "";
		JDBObjectNew->length = 1;
		JDBObjectNew->value.push_back(value);
		packets.push_back(JDBObjectNew);
		length += 2;
		namedLength += 4;
		JDBObjectNew->namedLength = 2;
		return JDBObjectNew;
	}

	/**
	 * @param lon
	 * @param name
	 * Add a long property to an Object
	 */
	void addPacket(long lon,string name)
	{
		AMEFObject* JDBObjectNew = addPacket(lon);
		JDBObjectNew->name = name;
		//length += name.length();
		namedLength += name.length();
		JDBObjectNew->namedLength += name.length();
	}
	/**
	 * @param lon
	 * Add a long property to an Object
	 */
	AMEFObject* addPacket(long lon)
	{
		AMEFObject* JDBObjectNew = new AMEFObject();
		if(lon<256)
		{
			JDBObjectNew->type = VERY_SMALL_INT_TYPE;
			JDBObjectNew->value = longTocharArray(lon, 1);
			length += 2;
			namedLength += 4;
			JDBObjectNew->namedLength  = 2;
			JDBObjectNew->length = 1;
		}
		else if(lon<65536)
		{
			JDBObjectNew->type = SMALL_INT_TYPE;
			JDBObjectNew->value = longTocharArray(lon, 2);
			length += 3;
			namedLength += 5;
			JDBObjectNew->namedLength  = 3;
			JDBObjectNew->length = 2;
		}
		else if(lon<16777216)
		{
			JDBObjectNew->type = BIG_INT_TYPE;
			JDBObjectNew->value = longTocharArray(lon, 3);
			length += 4;
			namedLength += 6;
			JDBObjectNew->namedLength  = 4;
			JDBObjectNew->length = 3;
		}
		else if(lon<4294967296L)
		{
			JDBObjectNew->type = INT_TYPE;
			JDBObjectNew->value = longTocharArray(lon, 4);
			length += 5;
			namedLength += 7;
			JDBObjectNew->namedLength  = 5;
			JDBObjectNew->length = 4;
		}
		else if(lon<1099511627776L)
		{
			JDBObjectNew->type = VS_LONG_INT_TYPE;
			JDBObjectNew->value = longTocharArray(lon, 5);
			length += 6;
			namedLength += 8;
			JDBObjectNew->namedLength  = 6;
			JDBObjectNew->length = 5;
		}
		else if(lon<281474976710656L)
		{
			JDBObjectNew->type = S_LONG_INT_TYPE;
			JDBObjectNew->value = longTocharArray(lon, 6);
			length += 7;
			namedLength += 9;
			JDBObjectNew->namedLength  = 7;
			JDBObjectNew->length = 6;
		}
		else if(lon<72057594037927936L)
		{
			JDBObjectNew->type = B_LONG_INT_TYPE;
			JDBObjectNew->value = longTocharArray(lon, 7);
			length += 8;
			namedLength += 10;
			JDBObjectNew->namedLength  = 8;
			JDBObjectNew->length = 7;
		}
		else
		{
			JDBObjectNew->type = LONG_INT_TYPE;
			JDBObjectNew->value = longTocharArray(lon, 8);
			length += 9;
			namedLength += 11;
			JDBObjectNew->namedLength  = 9;
			JDBObjectNew->length = 8;
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
	void addPacket(float doub,string name)
	{
		AMEFObject* JDBObjectNew = addPacket(doub);
		JDBObjectNew->name = name;
		//length += name.length();
		namedLength += name.length();
		JDBObjectNew->namedLength += name.length();
	}
	/**
	 * @param doub
	 * Add a double property to an Object
	 */
	AMEFObject* addPacket(float doub)
	{
		AMEFObject* JDBObjectNew = new AMEFObject();
		JDBObjectNew->type = DOUBLE_FLOAT_TYPE;
		JDBObjectNew->name = "";
		JDBObjectNew->value = boost::lexical_cast<string>(doub);
		JDBObjectNew->length = JDBObjectNew->value.length();
		length += JDBObjectNew->value.length() + 2;
		namedLength += JDBObjectNew->value.length() + 4;
		JDBObjectNew->namedLength = JDBObjectNew->value.length() + 2;
		packets.push_back(JDBObjectNew);
		return JDBObjectNew;
	}


	/**
	 * @param doub
	 * @param name
	 * Add a double property to an Object
	 */
	void addPacket(double doub,string name)
	{
		AMEFObject* JDBObjectNew = addPacket(doub);
		JDBObjectNew->name = name;
		//length += name.length();
		namedLength += name.length();
		JDBObjectNew->namedLength += name.length();
	}
	/**
	 * @param doub
	 * Add a double property to an Object
	 */
	AMEFObject* addPacket(double doub)
	{
		AMEFObject* JDBObjectNew = new AMEFObject();
		JDBObjectNew->type = DOUBLE_FLOAT_TYPE;
		JDBObjectNew->name = "";
		JDBObjectNew->value = boost::lexical_cast<string>(doub);
		JDBObjectNew->length = JDBObjectNew->value.length();
		length += JDBObjectNew->value.length() + 2;
		namedLength += JDBObjectNew->value.length() + 4;
		JDBObjectNew->namedLength = JDBObjectNew->value.length() + 2;
		packets.push_back(JDBObjectNew);
		return JDBObjectNew;
	}

	/**
	 * @param integer
	 * @param name
	 * Add an integer property to an Object
	 */
	void addPacket(int integer,string name)
	{
		AMEFObject* JDBObjectNew = addPacket(integer);
		JDBObjectNew->name = name;
		//length += name.length();
		namedLength += name.length();
		JDBObjectNew->namedLength += name.length();
	}
	/**
	 * @param integer
	 * Add an integer property to an Object
	 */
	AMEFObject* addPacket(int integer)
	{
		AMEFObject* JDBObjectNew = new AMEFObject();
		if(integer<256)
		{
			JDBObjectNew->type = VERY_SMALL_INT_TYPE;
			JDBObjectNew->value = intTocharArray(integer, 1);
			length += 2;
			namedLength += 4;
			JDBObjectNew->namedLength  = 2;
			JDBObjectNew->length = 1;
		}
		else if(integer<65536)
		{
			JDBObjectNew->type = SMALL_INT_TYPE;
			JDBObjectNew->value = intTocharArray(integer, 2);
			length += 3;
			namedLength += 5;
			JDBObjectNew->namedLength  = 3;
			JDBObjectNew->length = 2;
		}
		else if(integer<16777216)
		{
			JDBObjectNew->type = BIG_INT_TYPE;
			JDBObjectNew->value = intTocharArray(integer, 3);
			length += 4;
			namedLength += 6;
			JDBObjectNew->namedLength  = 4;
			JDBObjectNew->length = 3;
		}
		else
		{
			JDBObjectNew->type = INT_TYPE;
			JDBObjectNew->value = intTocharArray(integer, 4);
			length += 5;
			namedLength += 7;
			JDBObjectNew->namedLength  = 5;
			JDBObjectNew->length = 4;
		}
		JDBObjectNew->name = "";
		packets.push_back(JDBObjectNew);
		return JDBObjectNew;
	}

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
	void addPacket(AMEFObject *packet)
	{
		packets.push_back(packet);
		if(packet->type=='o')
		{
			if(packet->length+1<256)
				packet->type = 'm';
			else if(packet->length+1<65536)
				packet->type = 'q';
			else if(packet->length+1<16777216)
				packet->type = 'p';
			else
				packet->type = 'o';
		}
		length += packet->getLength();
		namedLength += packet->getNamedLength(false);
	}

	/*void set(int i,AMEFObject jdbo)
	{
		packets.set(i,jdbo);
	}*/

	/**
	 * @param packet
	 * Add a JDBObjectNew property to an Object
	 */
	void addPacket(char packet[],char type)
	{
		if(type=='s' || type=='d' || type=='t' || type=='h' || type=='y')
		{
			addPacket(packet);
		}
		else if(type=='n' || type=='w' || type=='r' || type=='i')
		{
			addPacket(charArrayToInt(packet));
		}
		else if(type=='f' || type=='x' || type=='e' || type=='l')
		{
			addPacket(charArrayToLong(packet));
		}
		else if(type=='u')
		{
			//addPacket(Double.parseDouble(new string(packet)));
		}
		else if(type=='b')
		{
			addPacket(packet[0]=='1'?true:false);
		}
		else if(type=='c')
		{
			addPacket((char)packet[0]);
		}
		else if(type=='a' || type=='g' || type=='j' || type=='k' || type=='v' || type=='z')
		{
			addNullPacket(type);
		}
	}

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

	int getlength()
	{
		return length ;
	}

	int getLength()
	{
		if(type=='m')
		{
			return 2 + length;
		}
		else if(type=='q')
		{
			return 3 + length;
		}
		else if(type=='p')
		{
			return 4 + length;
		}
		else if(type=='o')
		{
			return 5 + length;
		}
		else
			return length;
	}


	static bool isstring(char type)
	{
		if(type=='s' || type=='t' || type=='h' || type=='y' || type=='a')
			return true;
		return false;
	}

	static bool isFloatingPoint(char type)
	{
		if(type=='u' || type=='k')
			return true;
		return false;
	}

	static bool isNumber(char type)
	{
		if(type=='n' || type=='w' || type=='r' || type=='i'
			|| type=='f' || type=='x' || type=='e' || type=='l' || type=='g')
			return true;
		return false;
	}

	static bool isInteger(char type)
	{
		if(type=='n' || type=='w' || type=='r' || type=='i')
			return true;
		return false;
	}

	static bool isLong(char type)
	{
		if(type=='n' || type=='w' || type=='r' || type=='i'
			|| type=='f' || type=='x' || type=='e' || type=='l')
			return true;
		return false;
	}

	static bool isChar(char type)
	{
		if(type=='c' ||  type=='z')
			return true;
		return false;
	}

	static bool isBoolean(char type)
	{
		if(type=='b' || type=='v')
			return true;
		return false;
	}

	static bool isDate(char type)
	{
		if(type=='d' || type=='j')
			return true;
		return false;
	}

	bool  isstring()
	{
		if(type=='s' || type=='t' || type=='h' || type=='y' || type=='a')
			return true;
		return false;
	}

	bool  isString()
	{
		if(type=='s' || type=='t' || type=='h' || type=='y')
			return true;
		return false;
	}

	bool  isFloatingPoint()
	{
		if(type=='u' || type=='k')
			return true;
		return false;
	}

	bool  isNumber()
	{
		if(type=='n' || type=='w' || type=='r' || type=='i'
			|| type=='f' || type=='x' || type=='e' || type=='l' || type=='g')
			return true;
		return false;
	}

	bool  isnumber()
	{
		if(type=='n' || type=='w' || type=='r' || type=='i'
			|| type=='f' || type=='x' || type=='e' || type=='l')
			return true;
		return false;
	}

	bool  isChar()
	{
		if(type=='b' || type=='c' || type=='v' || type=='z')
			return true;
		return false;
	}

	bool  isDate()
	{
		if(type=='d' || type=='j')
			return true;
		return false;
	}

	int getNamedLength(bool  ignoreName)
	{
		if(ignoreName)
		{
			if(getType()=='o')
			{
				if(length<256)
					type = 'm';
				else if(length<65536)
					type = 'q';
				else if(length<16777216)
					type = 'p';
				else
					type = 'o';
				return getLength();
			}
			else
			{
				int len = length;
				if(getType()!='n' && getType()!='w' && getType()!='r'
					&& getType()!='i' && getType()!='f' && getType()!='x'
						&& getType()!='e' && getType()!='l' && getType()!='b'
							&& getType()!='c')
				{
					len++;
				}
				if(getType()=='a' || getType()=='g'
					|| getType()=='j' || getType()=='v' || getType()=='z')
					return len;
				if(length<256)
					len++;
				else if(length<65536)
					len+=2;
				else if(length<16777216)
					len+=3;
				else
					len+=4;
				return len;
			}
		}
		else
		{
			if(getType()=='o')
			{
				if(2 + namedLength<256)
				{
					type = 'm';
					namedLength += 2;
				}
				else if(2 + namedLength<65536)
				{
					type = 'q';
					namedLength += 3;
				}
				else if(2 + namedLength<16777216)
				{
					type = 'p';
					namedLength += 4;
				}
				else
				{
					type = 'o';
					namedLength += 5;
				}
				return namedLength;
			}
			else if(getType()=='m' || getType()=='p' || getType()=='q')
			{
				return namedLength;
			}
			else
			{
				return 2 + namedLength;
			}
		}

	}

	void setLength(int length)
	{
		this->length = length;
	}

	char* getName()
	{
		return (char*)name.c_str();
	}
	string getNameStr()
	{
		return name;
	}
	void setName(string name)
	{
		this->name = name;
	}
	void setName(char name[])
	{
		this->name = name;
	}
	vector<AMEFObject*> getPackets()
	{
		return packets;
	}
	void setPackets(vector<AMEFObject*> packets)
	{
		this->packets = packets;
	}

	char getType()
	{
		return type;
	}
	void setType(char type)
	{
		this->type = type;
	}

	string getValue()
	{
		return value;
	}
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
	string getValueStr()
	{
		return value;
	}
	void pushChar(char v)
	{
		this->value.push_back(v);
	}
	void setValue(char value[])
	{
		int len = strlen(value);
		for (int var = 0; var < len; var++) {
			this->value.push_back(value[var]);
		}
		delete[] value;
	}
	void setValue(char *value,int len)
	{
		this->value.append(value,len);
	}
	void setValue(string value)
	{
		this->value = value;
	}

	/**
	 * @return bool  value of this object if its type is boolean
	 */
	bool getBooleanValue()
	{
		if(type=='b')
			return (value[0]=='1'?true:false);
		else
			return false;
	}

	/**
	 * @return integer value of this object if its type is integer
	 */
	int getIntValue()
	{
		if(type=='n' || type=='w' || type=='r' || type=='i')
		{
			return charArrayToInt(value);
		}
		else
			return -1;
	}

	/**
	 * @return double value of this object if its type is double
	 */
	double getDoubleValue()
	{
		if(type=='u')
			return (boost::lexical_cast<double>("12312.123"));
		else
			return -1;
	}

	/**
	 * @return long value of this object if its type is long
	 */
	long getLongValue()
	{
		if(type=='f' || type=='x' || type=='e' || type=='l')
		{
			return charArrayToLong(value);
		}
		else
			return -1;
	}

	long getNumericValue()
	{
		if(type=='f' || type=='x' || type=='e' || type=='l'
			|| type=='n' || type=='w' || type=='r' || type=='i')
		{
			return charArrayToLong(value);
		}
		else
			return -1;
	}

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

	string tostring()
	{
		return displayObject("");
	}

	string displayObject(string tab)
	{
		string displ = "";
		for (int i=0;i<(int)getPackets().size();i++)
		{
			AMEFObject* obj = getPackets().at(i);
			displ += tab + "Object Type = ";
			displ += obj->type;
			displ += "\n" + tab + "Object Name = " + obj->name + "\n";
			displ += tab + "Object Value = ";
			if(obj->isstring() || obj->isFloatingPoint() || obj->isDate())
				displ += (obj->getValueStr()) + "\n";
			else if(obj->isChar())
			{
				if(obj->type=='b')
					displ += boost::lexical_cast<string>(obj->getBooleanValue()) + "\n";
				else
					displ += (char)obj->value[0] + "\n";
			}
			else if(obj->isNumber())
			{
				displ += boost::lexical_cast<string>(obj->getNumericValue()) + "\n";
			}
			if(obj->type=='o' || obj->type=='p' || obj->type=='q' || obj->type=='m')
			{
				displ += obj->displayObject(tab+"\t");
			}
		}
		return displ;
	}

	void addStaticPacket(AMEFObject *obj)
	{
		packets.push_back(obj);
	}
};


#endif /* AMEFOBJECT_H_ */
