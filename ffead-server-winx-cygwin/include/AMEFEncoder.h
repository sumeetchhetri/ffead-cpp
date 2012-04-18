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

#ifndef AMEFENCODER_H_
#define AMEFENCODER_H_

#include "AMEFObject.h"
#include "iostream"

class AMEFEncoder
{
	/*The default delimiter for single object representation*/
	//string delim = ",";

	/**
	 * @param packet
	 * @return char*
	 * @throws AMEFEncodeException
	 * encode the AMEFObject to the charstream for wire transmission
	 */
	/*char* encode(AMEFObject packet,boolean ignoreName) throws AMEFEncodeException
	{
		string dat = encodeSinglePacket(packet,ignoreName);
		int l = dat.length();
		return (intTocharArrayS(l, 4) + dat).getchars();
	}	*/
public:
	AMEFEncoder();
	~AMEFEncoder();
	static char* longTocharArray(long l,int ind)
		{
			char* result = new char[ind];
			for (int i = 0; i<ind; i++)
			{
				int offset = (ind - 1 - i) * 8;
	            result[i] = (char) ((l >> offset) & 0xFF);
			}
	        return result;
	    }

		static string longTocharArrayS(long l,int ind)
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

		static string intTocharArrayWI(int l)
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
			string result;
			for (int i = 0; i<ind; i++)
			{
				int offset = (ind - 1 - i) * 8;
	            result.push_back((char) ((l >> offset) & 0xFF));
			}
	        return result;
	    }

		static int charArrayToInt(char l[])
		{
			int t = 0;
			int ind = sizeof l;
	        for (int i = 0; i < ind; i++)
			{
	        	int offset = (ind -1 - i) * 8;
	        	t += (l[i] & 0x000000FF) << offset;
			}
	        return t;
	    }

		static int charArrayToInt(char* l,int off,int ind)
		{
			int t = 0;
			for (int i = 0; i < ind; i++)
			{
	        	int offset = (ind -1 - i) * 8;
	        	t += (l[off+i] & 0x000000FF) << offset;
			}
	        return t;
	    }

		static long charArrayToLong(char l[])
		{
			long t = 0;
			int ind = sizeof l;
	        for (int i = 0; i < ind; i++)
			{
	        	int offset = (ind -1 - i) * 8;
	        	t += (l[i] & 0x000000FF) << offset;
			}
	        return t;
	    }
		static long charArrayToLong(char* l,int off,int ind)
		{
			long t = 0;
			for (int i = 0; i < ind; i++)
			{
	        	int offset = (ind -1 - i) * 8;
	        	t += (l[off+i] & 0x000000FF) << offset;
			}
	        return t;
	    }
		static long charArrayToLong(char* l,int ind)
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
			string tem;
			for (int i = 0; i < ind; i++)
				tem.push_back(result[ind]);
			delete result;
	        return tem;
	    }

	string encodeB(AMEFObject* packet,bool ignoreName)
	{
		//char* dat = new char[packet->getNamedLength(ignoreName) + 4];
		string enc = encodeSinglePacketB(packet, ignoreName);
		string len = intTocharArray(enc.length(), 4);
		string temp;
		temp.append(len);
		temp.append(enc);
		return temp;
	}

	string encodeWL(AMEFObject* packet,bool ignoreName)
	{
		//char* dat = new char[packet->getNamedLength(ignoreName)];
		string enc = encodeSinglePacketB(packet, ignoreName);
		return enc;
	}

	/*char* encodeWL(AMEFObject packet,boolean ignoreName) throws AMEFEncodeException
	{
		string dat = encodeSinglePacket(packet,ignoreName);
		int l = dat.length();
		return dat.getchars();
	}

	*//**
	 * @param packet
	 * @return char*
	 * @throws AMEFEncodeException
	 * encode the AMEFObject to the charstream for wire transmission
	 *//*
	string encodeS(AMEFObject packet,boolean ignoreName) throws AMEFEncodeException
	{
		string dat = null;
		if(ignoreName)
			dat = encodeJDBObject(packet);
		else
			dat = encodeSinglePacket(packet,ignoreName);
		int l = dat.length();
		return (intTocharArrayS(l, 4) + dat);
	}

	string encodeJDBObject(AMEFObject packet) throws AMEFEncodeException
	{
		string dat = encodeSingleJDBPacket(packet);
		int l = dat.length();
		return (dat);
		//return dat;
	}

	string getValue(int val,int ind)
	{
		char* buf = new char[ind];
		for (int i = 0; i < buf.length; i++)
		{

		}
	}*/


	void getValue(string value,char type,string buffer)
	{
		/*if(type==AMEFObject::DATE_TYPE || type==AMEFObject::STRING_65536_TYPE
					|| type==AMEFObject::STRING_256_TYPE || type==AMEFObject::DOUBLE_FLOAT_TYPE
					|| type==AMEFObject::STRING_16777216_TYPE || type==AMEFObject::STRING_TYPE
					|| type==AMEFObject::BOOLEAN_TYPE || type==AMEFObject::CHAR_TYPE)
		{
			buffer.append(value);
		}
		else if(type==AMEFObject::VERY_SMALL_INT_TYPE)
		{
			buffer.append(value);
		}
		else if(type==AMEFObject::SMALL_INT_TYPE)
		{
			int intvalue = Integer.parseInt(value);
			buffer.append(intTocharArray(intvalue, 2));
		}
		else if(type==AMEFObject::BIG_INT_TYPE)
		{
			int intvalue = Integer.parseInt(value);
			buffer.append(new string(intTocharArray(intvalue, 3)));
		}
		else if(type==AMEFObject::INT_TYPE)
		{
			int intvalue = Integer.parseInt(value);
			buffer.append(new string(intTocharArray(intvalue, 4)));
		}
		else if(type==AMEFObject::VS_LONG_INT_TYPE)
		{
			long l = Long.parseLong(value);
			buffer.append(new string(longTocharArray(l, 5)));
		}
		else if(type==AMEFObject::S_LONG_INT_TYPE)
		{
			long l = Long.parseLong(value);
			buffer.append(new string(longTocharArray(l, 6)));
		}
		else if(type==AMEFObject::B_LONG_INT_TYPE)
		{
			long l = Long.parseLong(value);
			buffer.append(new string(longTocharArray(l, 7)));
		}
		else if(type==AMEFObject::LONG_INT_TYPE)
		{
			long l = Long.parseLong(value);
			buffer.append(new string(longTocharArray(l, 8)));
		}*/
		buffer.append(value);
	}

	string getFinalVal(char type,string buffer,int length,string delim, string name)
	{
		string retval = type + delim + name + delim;
		if(type==AMEFObject::DATE_TYPE || type==AMEFObject::STRING_256_TYPE
				|| type==AMEFObject::DOUBLE_FLOAT_TYPE)
		{
			retval += intTocharArrayS(length, 1) + buffer;
		}
		else if(type==AMEFObject::STRING_65536_TYPE)
		{
			retval += intTocharArrayS(length, 2) + buffer;
		}
		else if(type==AMEFObject::STRING_16777216_TYPE)
		{
			retval += intTocharArrayS(length, 3) + buffer;
		}
		else if(type==AMEFObject::STRING_TYPE)
		{
			retval += intTocharArrayS(length, 4) + buffer;
		}
		else if(type==AMEFObject::BOOLEAN_TYPE || type==AMEFObject::CHAR_TYPE
				|| type==AMEFObject::SMALL_INT_TYPE || type==AMEFObject::VERY_SMALL_INT_TYPE
				|| type==AMEFObject::BIG_INT_TYPE || type==AMEFObject::INT_TYPE
				|| type==AMEFObject::VS_LONG_INT_TYPE || type==AMEFObject::S_LONG_INT_TYPE
				|| type==AMEFObject::B_LONG_INT_TYPE || type==AMEFObject::LONG_INT_TYPE)
		{
			retval += buffer;
		}
		else if(type==AMEFObject::OBJECT_TYPE)
		{
			if(length<256)
			{
				retval = AMEFObject::VS_OBJECT_TYPE + delim + name + delim + intTocharArrayS(length, 1) + buffer;
			}
			else if(length<65536)
			{
				retval = AMEFObject::S_OBJECT_TYPE + delim + name + delim + intTocharArrayS(length, 2) + buffer;
			}
			else if(length<16777216)
			{
				retval = AMEFObject::B_OBJECT_TYPE + delim + name + delim + intTocharArrayS(length, 3) + buffer;
			}
			else
			{
				retval = AMEFObject::OBJECT_TYPE + delim + name + delim + intTocharArrayS(length, 4) + buffer;
			}
		}
		else if(type==AMEFObject::VS_OBJECT_TYPE)
		{
			retval = AMEFObject::VS_OBJECT_TYPE + delim + name + delim + intTocharArrayS(length, 1) + buffer;
		}
		else if(type==AMEFObject::S_OBJECT_TYPE)
		{
			retval = AMEFObject::S_OBJECT_TYPE + delim + name + delim + intTocharArrayS(length, 2) + buffer;
		}
		else if(type==AMEFObject::B_OBJECT_TYPE)
		{
			retval = AMEFObject::B_OBJECT_TYPE + delim + name + delim + intTocharArrayS(length, 3) + buffer;
		}
		//else
		//{
		//	throw new AMEFEncodeException("Not a valid AMEF Object type,only types string,number,boolean,character,date allowed");
		//}
		return retval;
	}

	/*string encodeSingleJDBPacket(AMEFObject packet) throws AMEFEncodeException
	{
		stringBuilder buffer = new stringBuilder();
		if(packet==null)
		{
			throw new AMEFEncodeException("Objcet to be encoded is null");
		}
		int length = packet->getLength();
		for (AMEFObject pack : packet->getPackets())
		{
			buffer.append(encodeSingleJDBPacket(pack));
		}
		if(packet->getPackets().size()==0)
			getValue(new string(packet->getValue()),packet->getType(), buffer);
		if(bufferlength()>0)
		{
			length = bufferlength();
		}
		return getFinalVal(packet->getType(), buffer, length, "","");
	}

	*//**
	 * @param packet
	 * @return string
	 * @throws AMEFEncodeException
	 * encode a given AMEF Object to its transmission form
	 *//*
	string encodeSinglePacket(AMEFObject packet,boolean ignoreName) throws AMEFEncodeException
	{
		stringBuilder buffer = new stringBuilder();
		if(packet==null)
		{
			throw new AMEFEncodeException("Objcet to be encoded is null");
		}
		int length = packet->getLength();
		for (AMEFObject pack : packet->getPackets())
		{
			buffer.append(encodeSinglePacket(pack,ignoreName));
		}
		if(packet->getPackets().size()==0)
			getValue(new string(packet->getValue()),packet->getType(), buffer);
		if(bufferlength()>0)
		{
			length = bufferlength();
		}
		string retval = "";
		if(!ignoreName)
			retval +=  packet->getName();
		return getFinalVal(packet->getType(), buffer, length, delim, retval);
	}


	char* encodeSinglePacketB(AMEFObject packet,boolean ignoreName) throws AMEFEncodeException
	{
		charArrayOutputStream buffer = new charArrayOutputStream();
		if(packet==null)
		{
			throw new AMEFEncodeException("Objcet to be encoded is null");
		}
		int length = packet->getLength();
		for (AMEFObject pack : packet->getPackets())
		{
			buffer.write(encodeSinglePacketB(pack,ignoreName));
		}
		if(packet->getPackets().size()==0)
		{
			buffer.write(packet->getValue().getchars());
		}
		string retval = "";
		if(!ignoreName)
			retval +=  packet->getName();
		char type = packet->getType();
		if(type==AMEFObject::DATE_TYPE || type==AMEFObject::STRING_256_TYPE
				|| type==AMEFObject::DOUBLE_FLOAT_TYPE)
		{
			buffer.write(intTocharArray(length, 1));
		}
		else if(type==AMEFObject::STRING_65536_TYPE)
		{
			buffer.write(intTocharArray(length, 2));
		}
		else if(type==AMEFObject::STRING_16777216_TYPE)
		{
			buffer.write(intTocharArray(length, 3));
		}
		else if(type==AMEFObject::STRING_TYPE)
		{
			buffer.write(intTocharArray(length, 4));
		}
		else if(type==AMEFObject::BOOLEAN_TYPE || type==AMEFObject::CHAR_TYPE
				|| type==AMEFObject::SMALL_INT_TYPE || type==AMEFObject::VERY_SMALL_INT_TYPE
				|| type==AMEFObject::BIG_INT_TYPE || type==AMEFObject::INT_TYPE
				|| type==AMEFObject::VS_LONG_INT_TYPE || type==AMEFObject::S_LONG_INT_TYPE
				|| type==AMEFObject::B_LONG_INT_TYPE || type==AMEFObject::LONG_INT_TYPE)
		{
			retval += buffer;
		}
		else if(type==AMEFObject::OBJECT_TYPE)
		{
			if(length<256)
			{
				retval = AMEFObject::VS_OBJECT_TYPE + delim + name + delim + intTocharArrayS(length, 1) + buffer;
			}
			else if(length<65536)
			{
				retval = AMEFObject::S_OBJECT_TYPE + delim + name + delim + intTocharArrayS(length, 2) + buffer;
			}
			else if(length<16777216)
			{
				retval = AMEFObject::B_OBJECT_TYPE + delim + name + delim + intTocharArrayS(length, 3) + buffer;
			}
			else
			{
				retval = AMEFObject::OBJECT_TYPE + delim + name + delim + intTocharArrayS(length, 4) + buffer;
			}
		}
		return getFinalVal(packet->getType(), buffer, length, delim, retval);
	}*/


	string getPacketValue(string value)
	{
		string arr;
		char type;
		if(value.length()<=256)
			type = AMEFObject::STRING_256_TYPE;
		else if(value.length()<=65536)
			type = AMEFObject::STRING_65536_TYPE;
		else if(value.length()<=16777216)
			type = AMEFObject::STRING_16777216_TYPE;
		else
			type = AMEFObject::STRING_TYPE;

		if(type==AMEFObject::DATE_TYPE || type==AMEFObject::STRING_256_TYPE
				|| type==AMEFObject::DOUBLE_FLOAT_TYPE)
		{
			arr.push_back((char)type);
			string len = intTocharArray(value.length(), 1);
			arr.append(len);
			arr.append(value);
		}
		else if(type==AMEFObject::STRING_65536_TYPE)
		{
			arr.push_back((char)type);
			string len = intTocharArray(value.length(), 2);
			arr.append(len);arr.append(value);
		}
		else if(type==AMEFObject::STRING_16777216_TYPE)
		{
			arr.push_back((char)type);
			string len = intTocharArray(value.length(), 3);
			arr.append(len);arr.append(value);
		}
		else if(type==AMEFObject::STRING_TYPE)
		{
			arr.push_back((char)type);
			string len = intTocharArray(value.length(), 4);
			arr.append(len);arr.append(value);
		}
		else if(type==AMEFObject::BOOLEAN_TYPE || type==AMEFObject::CHAR_TYPE
				|| type==AMEFObject::SMALL_INT_TYPE || type==AMEFObject::VERY_SMALL_INT_TYPE
				|| type==AMEFObject::BIG_INT_TYPE || type==AMEFObject::INT_TYPE
				|| type==AMEFObject::VS_LONG_INT_TYPE || type==AMEFObject::S_LONG_INT_TYPE
				|| type==AMEFObject::B_LONG_INT_TYPE || type==AMEFObject::LONG_INT_TYPE)
		{

		}
		else if(type==AMEFObject::OBJECT_TYPE)
		{
			int length = value.length();
			if(length<256)
			{
				arr[0] = (char)AMEFObject::VS_OBJECT_TYPE;
				string len = intTocharArray(value.length(), 1);
				arr.append(len);arr.append(value);
			}
			else if(length<65536)
			{
				arr[0] = (char)AMEFObject::S_OBJECT_TYPE;
				string len = intTocharArray(value.length(), 2);
				arr.append(len);
			}
			else if(length<16777216)
			{
				arr[0] = (char)AMEFObject::B_OBJECT_TYPE;
				string len = intTocharArray(value.length(),3);
				arr.append(len);
			}
			else
			{
				arr[0] = (char)AMEFObject::OBJECT_TYPE;
				string len = intTocharArray(value.length(),4);
				arr.append(len);
			}
		}
		return arr;
	}

	string getPacketValue(int integer)
	{
		char type;
		int ind = 1;
		if(integer<256)
			type = AMEFObject::VERY_SMALL_INT_TYPE;
		else if(integer<65536)
		{
			type = AMEFObject::SMALL_INT_TYPE;
			ind = 2;
		}
		else if(integer<16777216)
		{
			type = AMEFObject::BIG_INT_TYPE;
			ind = 3;
		}
		else
		{
			type = AMEFObject::INT_TYPE;
			ind = 4;
		}
		string arr;
		arr.push_back((char)type);
		string len = intTocharArray(integer, ind);
		arr.append(len);
		return arr;
	}

	string getPacketValue(long lon)
	{
		char type;
		int ind = 1;
		if(lon<256)
			type = AMEFObject::VERY_SMALL_INT_TYPE;
		else if(lon<65536)
		{
			type = AMEFObject::SMALL_INT_TYPE;
			ind = 2;
		}
		else if(lon<16777216)
		{
			type = AMEFObject::BIG_INT_TYPE;
			ind = 3;
		}
		else if(lon<4294967296L)
		{
			type = AMEFObject::INT_TYPE;
			ind = 4;
		}
		else if(lon<1099511627776L)
		{
			type = AMEFObject::VS_LONG_INT_TYPE;
			ind = 5;
		}
		else if(lon<281474976710656L)
		{
			type = AMEFObject::S_LONG_INT_TYPE;
			ind = 6;
		}
		else if(lon<72057594037927936L)
		{
			type = AMEFObject::B_LONG_INT_TYPE;
			ind = 7;
		}
		else
		{
			type = AMEFObject::LONG_INT_TYPE;
			ind = 8;
		}
		string arr;
		arr.push_back((char)type);
		char* len = longTocharArray(lon, ind);
		arr.append(len);
		return arr;
	}

	string getPacketValue(double lon)
	{
		string buffer;
		char type = AMEFObject::DOUBLE_FLOAT_TYPE;
		//getValue(string.valueOf(lon),type, buffer);
		string retVal = getFinalVal(type, buffer, 1, "", "");
		return retVal;
	}

	string getPacketValue(float lon)
	{
		string buffer;
		char type = AMEFObject::DOUBLE_FLOAT_TYPE;
		//getValue(string.valueOf(lon),type, buffer);
		string retVal = getFinalVal(type, buffer, 1, "", "");
		return retVal;
	}

	string getPacketValue(bool lon)
	{
		return (AMEFObject::BOOLEAN_TYPE+(lon?"1":"0"));
	}

	string getPacketValue(char lon)
	{
		return (AMEFObject::CHAR_TYPE+""+lon);
	}









	//---------------------------------------------------------------------------------------//
	string encodeSinglePacketB(AMEFObject *packet,bool ignoreName)
	{
		int bufferlength = packet->getNamedLength(ignoreName);
		string buffer;
		int pos = 0;
		if(!ignoreName)
		{
			if(packet->getType()=='o')
			{
				if(bufferlength+4<256)
				{
					buffer.push_back(AMEFObject::VS_OBJECT_TYPE);
				}
				else if(bufferlength+4<65536)
				{
					buffer.push_back(AMEFObject::S_OBJECT_TYPE);
				}
				else if(bufferlength+4<16777216)
				{
					buffer.push_back(AMEFObject::B_OBJECT_TYPE);
				}
			}
			else
				buffer.push_back((char)packet->getType());
			buffer.push_back((char)',');
			buffer.append(packet->getName());
			//System.arraycopy(packet->getName(), 0, buffer, 2, packet->getName().length);
			//pos = packet->getName().length() + 2;
			buffer.push_back((char)',');
			pos ++;
			if(packet->getType()!='n' && packet->getType()!='w' && packet->getType()!='r'
				&& packet->getType()!='i' && packet->getType()!='f' && packet->getType()!='x'
					&& packet->getType()!='e' && packet->getType()!='l' && packet->getType()!='b'
						&& packet->getType()!='c' && packet->getType()!='a' && packet->getType()!='g'
							&& packet->getType()!='j' && packet->getType()!='v' && packet->getType()!='z')
			{
				string lengthb = intTocharArrayWI(packet->getlength());
				buffer.append(lengthb);
				//System.arraycopy(lengthb, 0, buffer, pos, lengthb.length);
				//pos += lengthb.length;
			}
		}
		else
		{
			buffer.push_back((char)packet->getType());
			pos++;
			if(packet->getType()!='n' && packet->getType()!='w' && packet->getType()!='r'
				&& packet->getType()!='i' && packet->getType()!='f' && packet->getType()!='x'
					&& packet->getType()!='e' && packet->getType()!='l' && packet->getType()!='b'
						&& packet->getType()!='c' && packet->getType()!='a' && packet->getType()!='g'
							&& packet->getType()!='j' && packet->getType()!='v' && packet->getType()!='z')
			{
				string lengthb = intTocharArrayWI(packet->getlength());
				buffer.append(lengthb);
				//System.arraycopy(lengthb, 0, buffer, pos, lengthb.length);
				//pos += lengthb.length;
			}
		}

		for (int i=0;i<packet->getPackets().size();i++)
		{
			AMEFObject *pack = packet->getPackets().at(i);
			string val = encodeSinglePacketB(pack,ignoreName);
			buffer.append(val);
			//System.arraycopy(val, 0, buffer, pos, val.length);
			//pos += val.length;
		}
		if(packet->getPackets().size()==0 && packet->getType()!='a' && packet->getType()!='g'
			&& packet->getType()!='j' && packet->getType()!='v' && packet->getType()!='z')
		{
			buffer.append(packet->getValue());
			//System.arraycopy(packet->getValue(), 0, buffer, pos, packet->getValue().length);
		}
		return buffer;
	}
}
;

#endif /* AMEFENCODER_H_ */
