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

//Copied from https://github.com/OSEHRA-Sandbox/gtm-bindings/blob/master/Source/GTM.h
#ifndef __GTM_h
#define __GTM_h
#include "AppDefines.h"
#ifdef INC_GTM
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cstddef>
#include <string>

extern "C" {
#include "gtmxc_types.h"
#include <termios.h>
}

// maximum length of a GT.M message
#define maxMessageLength 2048
#define maxValueLength 1048576

// GT.M call wrapper - if an error in call or untrappable
// error in GT.M, print error on STDERR, clean up and exit
#define CALLGTM(functioncall) \
  this->functionStatus = functioncall ;		\
  if (0 != this->functionStatus ) {				\
    gtm_zstatus( this->statusMessage, maxMessageLength );			\
    /*std::cerr << this->statusMessage << std::endl;		\*/
    tcsetattr( 2, 0, &stderr_sav );		\
    tcsetattr( 1, 0, &stdout_sav );		\
    tcsetattr( 0, 0, &stdin_sav );		\
    gtm_exit();					\
  }

//
//
//  C++ Interface to GTM API.
//
//
class GTM {
public:
	static GTM* getInstance();
	virtual ~GTM();

	//
	//  Methods using std::string for arguments and Exceptions for error management.
	//
	void kill(const std::string & nameOfGlobal);
	void lock(const std::string & nameOfGlobal);
	void execute(const std::string & textOfCode);
	void executeInTransaction(const std::string & textOfCode);
	std::string order(const std::string & nameOfGlobal);
	std::string query(const std::string & nameOfGlobal);
	std::string getJsonString(const std::string & nameOfGlobal);

	//
	//  Methods that return values by copy.
	//
	void set(const std::string & nameOfGlobal, const std::string & valueOfGlobal);
	std::string getString(const std::string & nameOfGlobal);

	//
	//  Methods using int for arguments and Exceptions for error management.
	//
	void set(const std::string & nameOfGlobal, const int & valueOfGlobal);
	int getInt(const std::string & nameOfGlobal);

	//
	//  Methods using unsigned int for arguments and Exceptions for error management.
	//
	void set(const std::string & nameOfGlobal, const unsigned int & valueOfGlobal);
	unsigned int getUInt(const std::string & nameOfGlobal);

	//
	//  Methods using long for arguments and Exceptions for error management.
	//
	void set(const std::string & nameOfGlobal, const long & valueOfGlobal);
	long getLong(const std::string & nameOfGlobal);

	//
	//  Methods using unsigned long for arguments and Exceptions for error management.
	//
	void set(const std::string & nameOfGlobal, const unsigned long & valueOfGlobal);
	unsigned long getULong(const std::string & nameOfGlobal);

	//
	//  Methods using float for arguments and Exceptions for error management.
	//
	void set(const std::string & nameOfGlobal, const float & valueOfGlobal);
	float getFloat(const std::string & nameOfGlobal);

	//
	//  Methods using double for arguments and Exceptions for error management.
	//
	void set(const std::string & nameOfGlobal, const double & valueOfGlobal);
	double getDouble(const std::string & nameOfGlobal);

private:

	//
	//   Member variables used to interact with GT.M API
	//

	// return of GT.M functions
	gtm_status_t functionStatus;

	//
	//   The use of these member variables assumes that
	//   this class is not used in a multi-threaded environment.
	//
	gtm_char_t statusMessage[maxMessageLength];
	//gtm_char_t errorMessage[maxMessageLength];
	//gtm_char_t nameOfGlobal[maxValueLength];
	//gtm_char_t valueOfGlobal[maxValueLength];
	//gtm_char_t valueOfIndex[maxValueLength];

	//
	//  Function Descriptors
	//
	ci_name_descriptor gtmgetS;
	ci_name_descriptor gtmsetS;
	ci_name_descriptor gtmgetI;
	ci_name_descriptor gtmsetI;
	ci_name_descriptor gtmgetUI;
	ci_name_descriptor gtmsetUI;
	ci_name_descriptor gtmgetL;
	ci_name_descriptor gtmsetL;
	ci_name_descriptor gtmgetUL;
	ci_name_descriptor gtmsetUL;
	ci_name_descriptor gtmgetF;
	ci_name_descriptor gtmsetF;
	ci_name_descriptor gtmgetD;
	ci_name_descriptor gtmsetD;
	ci_name_descriptor gtminit;
	ci_name_descriptor gtmkill;
	ci_name_descriptor gtmlock;
	ci_name_descriptor gtmorder;
	ci_name_descriptor gtmquery;
	ci_name_descriptor gtmxecute;
	ci_name_descriptor gtmxecuteintrans;
	ci_name_descriptor gtmtoJSON;

	gtm_string_t gtmgetS_str;
	gtm_string_t gtmsetS_str;
	gtm_string_t gtmgetI_str;
	gtm_string_t gtmsetI_str;
	gtm_string_t gtmgetUI_str;
	gtm_string_t gtmsetUI_str;
	gtm_string_t gtmgetL_str;
	gtm_string_t gtmsetL_str;
	gtm_string_t gtmgetUL_str;
	gtm_string_t gtmsetUL_str;
	gtm_string_t gtmgetF_str;
	gtm_string_t gtmsetF_str;
	gtm_string_t gtmgetD_str;
	gtm_string_t gtmsetD_str;
	gtm_string_t gtminit_str;
	gtm_string_t gtmkill_str;
	gtm_string_t gtmlock_str;
	gtm_string_t gtmorder_str;
	gtm_string_t gtmquery_str;
	gtm_string_t gtmxecute_str;
	gtm_string_t gtmxecuteintrans_str;
	gtm_string_t gtmtoJSON_str;

	char * gtmgetS_cstr;
	char * gtmsetS_cstr;
	char * gtmgetI_cstr;
	char * gtmsetI_cstr;
	char * gtmgetUI_cstr;
	char * gtmsetUI_cstr;
	char * gtmgetL_cstr;
	char * gtmsetL_cstr;
	char * gtmgetUL_cstr;
	char * gtmsetUL_cstr;
	char * gtmgetF_cstr;
	char * gtmsetF_cstr;
	char * gtmgetD_cstr;
	char * gtmsetD_cstr;
	char * gtminit_cstr;
	char * gtmkill_cstr;
	char * gtmlock_cstr;
	char * gtmorder_cstr;
	char * gtmquery_cstr;
	char * gtmxecute_cstr;
	char * gtmxecuteintrans_cstr;
	char * gtmtoJSON_cstr;

	//
	//  Structure to return values
	//
	//gtm_string_t p_value;

	//
	// Terminal Structures
	//
	struct termios stderr_sav;
	struct termios stdin_sav;
	struct termios stdout_sav;

	GTM();
	static GTM* instance;
};
#endif
#endif
