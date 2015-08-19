/*=========================================================================
 *
 *  Copyright OSEHRA
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

//Copied from https://github.com/OSEHRA-Sandbox/gtm-bindings/blob/master/Source/GTM.cpp

#include "GTM.h"
//#ifdef INC_GTM
#include "GTMException.h"
#include <iostream>
#include <cstring>

GTM* GTM::instance = NULL;

GTM* GTM::getInstance()
{
	if(instance==NULL) {
		instance = new GTM();
	}
	return instance;
}

#define INITIALIZE_FUNCTION_DESCRIPTOR(functionname) \
		functionname##_cstr = new char[sizeof(#functionname)]; \
		strncpy(functionname##_cstr,#functionname,sizeof(#functionname)); \
		functionname##_str.address = functionname##_cstr;  \
		functionname##_str.length = sizeof(#functionname)-1; \
		functionname.rtn_name=functionname##_str;  \
		functionname.handle = NULL;

#define RELEASE_FUNCTION_DESCRIPTOR(functionname) \
		delete [] functionname##_cstr;

//
// Constructor
//
GTM::GTM()
{

	if (NULL == getenv( "GTMCI" )) putenv((char*)"GTMCI=/home/sumeet/gtm_access/gtm_access.ci");
	//
	// Since GT.M changes the terminal attributes, save the attributes of stderr, stdin and stdout
	// in order to restore them before exit
	//
	tcgetattr( 0, &stdin_sav );
	tcgetattr( 1, &stdout_sav );
	tcgetattr( 2, &stderr_sav );

	//
	// Initialize GTM
	//
	CALLGTM( gtm_init() );

	//
	// Initialization - function descriptors for calling in to GT.M
	//
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmgetS);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmsetS);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmgetI);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmsetI);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmgetUI);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmsetUI);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmgetL);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmsetL);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmgetUL);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmsetUL);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmgetF);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmsetF);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmgetD);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmsetD);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmkill);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmlock);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmorder);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmquery);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmxecute);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmxecuteintrans);
	INITIALIZE_FUNCTION_DESCRIPTOR(gtmtoJSON);

}


//
// Destructor
//
GTM::~GTM()
{
	//
	// Release resources of function descriptors for calling in to GT.M
	//
	RELEASE_FUNCTION_DESCRIPTOR(gtmgetS);
	RELEASE_FUNCTION_DESCRIPTOR(gtmsetS);
	RELEASE_FUNCTION_DESCRIPTOR(gtmgetI);
	RELEASE_FUNCTION_DESCRIPTOR(gtmsetI);
	RELEASE_FUNCTION_DESCRIPTOR(gtmgetUI);
	RELEASE_FUNCTION_DESCRIPTOR(gtmsetUI);
	RELEASE_FUNCTION_DESCRIPTOR(gtmgetL);
	RELEASE_FUNCTION_DESCRIPTOR(gtmsetL);
	RELEASE_FUNCTION_DESCRIPTOR(gtmgetUL);
	RELEASE_FUNCTION_DESCRIPTOR(gtmsetUL);
	RELEASE_FUNCTION_DESCRIPTOR(gtmgetF);
	RELEASE_FUNCTION_DESCRIPTOR(gtmsetF);
	RELEASE_FUNCTION_DESCRIPTOR(gtmgetD);
	RELEASE_FUNCTION_DESCRIPTOR(gtmsetD);
	RELEASE_FUNCTION_DESCRIPTOR(gtmkill);
	RELEASE_FUNCTION_DESCRIPTOR(gtmlock);
	RELEASE_FUNCTION_DESCRIPTOR(gtmorder);
	RELEASE_FUNCTION_DESCRIPTOR(gtmquery);
	RELEASE_FUNCTION_DESCRIPTOR(gtmxecute);
	RELEASE_FUNCTION_DESCRIPTOR(gtmxecuteintrans);
	RELEASE_FUNCTION_DESCRIPTOR(gtmtoJSON);

	// Cleanup GT.M runtime
	CALLGTM( gtm_exit() );

	// Restore terminal attributes
	tcsetattr( 2, 0, &stderr_sav );
	tcsetattr( 1, 0, &stdout_sav );
	tcsetattr( 0, 0, &stdin_sav );
}

std::string GTM::getJsonString( const std::string & globalName)
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	gtm_string_t p_value;
	gtm_char_t valueOfGlobal[maxValueLength];
	p_value.address = static_cast< gtm_char_t *>(valueOfGlobal);
	p_value.length = maxValueLength ;
	CALLGTM( gtm_cip( &(this->gtmtoJSON), globalName.c_str(), &(p_value), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
	return std::string(valueOfGlobal, p_value.length);
}

void GTM::kill( const std::string & globalName)
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	CALLGTM( gtm_cip( &(this->gtmkill), globalName.c_str(), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
}

std::string GTM::order( const std::string & globalName)
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	gtm_string_t p_value;
	gtm_char_t valueOfGlobal[maxValueLength];
	p_value.address = static_cast< gtm_char_t *>(valueOfGlobal);
	p_value.length = maxValueLength ;
	CALLGTM( gtm_cip( &(this->gtmorder), globalName.c_str(), &(p_value), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
	return std::string(valueOfGlobal, p_value.length);
}

std::string GTM::query( const std::string & globalName)
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	gtm_string_t p_value;
	gtm_char_t valueOfGlobal[maxValueLength];
	p_value.address = static_cast< gtm_char_t *>(valueOfGlobal);
	p_value.length = maxValueLength ;
	CALLGTM( gtm_cip( &(this->gtmquery), globalName.c_str(), &(p_value), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
	return std::string(valueOfGlobal, p_value.length);
}

void GTM::lock( const std::string & globalName)
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	CALLGTM( gtm_cip( &(this->gtmlock), globalName.c_str(), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
}

void GTM::execute( const std::string & globalName)
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	CALLGTM( gtm_cip( &(this->gtmxecute), globalName.c_str(), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
}

void GTM::executeInTransaction( const std::string & globalName)
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	CALLGTM( gtm_cip( &(this->gtmxecuteintrans), globalName.c_str(), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
}

std::string GTM::getString( const std::string & globalName)
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	gtm_string_t p_value;
	gtm_char_t valueOfGlobal[maxValueLength];
	p_value.address = static_cast< gtm_char_t *>(valueOfGlobal);
	p_value.length = maxValueLength ;
	CALLGTM( gtm_cip( &(this->gtmgetS), globalName.c_str(), &(p_value), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
	return std::string(valueOfGlobal, p_value.length);
}

int GTM::getInt( const std::string & globalName)
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	gtm_int_t p_value;
	CALLGTM( gtm_cip( &(this->gtmgetI), globalName.c_str(), &(p_value), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
	return p_value;
}

unsigned int GTM::getUInt( const std::string & globalName)
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	gtm_uint_t p_value;
	CALLGTM( gtm_cip( &(this->gtmgetUI), globalName.c_str(), &(p_value), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
	return p_value;
}

long GTM::getLong( const std::string & globalName)
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	gtm_long_t p_value;
	CALLGTM( gtm_cip( &(this->gtmgetL), globalName.c_str(), &(p_value), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
	return p_value;
}

unsigned long GTM::getULong( const std::string & globalName)
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	gtm_ulong_t p_value;
	CALLGTM( gtm_cip( &(this->gtmgetUL), globalName.c_str(), &(p_value), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
	return p_value;
}

float GTM::getFloat( const std::string & globalName)
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	gtm_float_t p_value;
	CALLGTM( gtm_cip( &(this->gtmgetF), globalName.c_str(), &(p_value), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
	return p_value;
}

double GTM::getDouble( const std::string & globalName)
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	gtm_double_t p_value;
	CALLGTM( gtm_cip( &(this->gtmgetD), globalName.c_str(), &(p_value), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
	return p_value;
}

void GTM::set( const std::string & globalName, const std::string & globalValue )
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	gtm_string_t p_value;
	gtm_char_t * value = const_cast< gtm_char_t *>(globalValue.c_str());
	p_value.address = static_cast< gtm_char_t * >( value );
	p_value.length = globalValue.length();
	CALLGTM( gtm_cip( &(this->gtmsetS), globalName.c_str(), &(p_value), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
}

void GTM::set( const std::string & globalName, const int & globalValue )
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	CALLGTM( gtm_cip( &(this->gtmsetI), globalName.c_str(), &(globalValue), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
}

void GTM::set( const std::string & globalName, const unsigned int & globalValue )
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	CALLGTM( gtm_cip( &(this->gtmsetUI), globalName.c_str(), &(globalValue), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
}


void GTM::set( const std::string & globalName, const long & globalValue )
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	CALLGTM( gtm_cip( &(this->gtmsetL), globalName.c_str(), &(globalValue), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
}


void GTM::set( const std::string & globalName, const unsigned long & globalValue )
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	CALLGTM( gtm_cip( &(this->gtmsetUL), globalName.c_str(), &(globalValue), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
}

void GTM::set( const std::string & globalName, const float & globalValue )
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	CALLGTM( gtm_cip( &(this->gtmsetF), globalName.c_str(), &(globalValue), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
}



void GTM::set( const std::string & globalName, const double & globalValue )
{
	gtm_char_t errorMessage[maxMessageLength];
	errorMessage[0] = '\0';
	CALLGTM( gtm_cip( &(this->gtmsetD), globalName.c_str(), &(globalValue), &errorMessage ));
	if( strlen( errorMessage ) != 0 )
	{
		GTMException excp( errorMessage );
		throw(excp);
	}
}

//#endif
