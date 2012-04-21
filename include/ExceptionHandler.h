/*
	Copyright 2010, Sumeet Chhetri 
  
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
 * ExceptionHandler.h
 *
 *  Created on: Jun 10, 2010
 *      Author: sumeet
 */

#ifndef EXCEPTIONHANDLER_H_
#define EXCEPTIONHANDLER_H_
#include <execinfo.h>
#include <signal.h>
#include "stdlib.h"
#include <exception>
#include <iostream>
#include <boost/thread/thread.hpp>
#include <pthread.h>

using namespace std;

 /////////////////////////////////////////////

 class ExceptionTracer
 {
 public:
     ExceptionTracer();
 };

template <class SignalExceptionClass> class SignalTranslator
 {
 private:
     class SingleTonTranslator
     {
     public:
         SingleTonTranslator()
         {
             signal(SignalExceptionClass::GetSignalNumber(), SignalHandler);
         }

         static void SignalHandler(int)
         {
             throw SignalExceptionClass();
         }
     };

 public:
     SignalTranslator()
     {
         static SingleTonTranslator s_objTranslator;
     }
 };

 // An example for SIGSEGV
 class SegmentationFault : public ExceptionTracer, public exception
 {
 public:
     static int GetSignalNumber() {return SIGSEGV;}
 };

 SignalTranslator<SegmentationFault> g_objSegmentationFaultTranslator;

 // An example for SIGFPE
 class FloatingPointException : public ExceptionTracer, public exception
 {
 public:
     static int GetSignalNumber() {return SIGFPE;}
 };

 SignalTranslator<FloatingPointException> g_objFloatingPointExceptionTranslator;


class ExceptionHandler
{
 private:
     class SingleTonHandler
     {
     public:
    	 SingleTonHandler()
    	 {
    	 	set_terminate(Handler);
    	 }

    	 static void Handler()
    	 {
    	 	 // Exception from construction/destruction of global variables
    	 	 try
    	 	 {
    	 		 // re-throw
    	 		 throw;
    	 	 }
    	 	 catch (SegmentationFault &)
    	 	 {
    	 		 cout << "SegmentationFault" << endl;
    	 	 }
    	 	 catch (FloatingPointException &)
    	 	 {
    	 		 cout << "FloatingPointException" << endl;
    	 	 }
    	 	 catch (...)
    	 	 {
    	 		 cout << "Unknown Exception" << endl;
    	 	 }

    	 	 //if this is a thread performing some core activity
    	 	 abort();
    	 	 // else if this is a thread used to service requests

    	 }
     };

 public:
     ExceptionHandler();
 };

#endif /* EXCEPTIONHANDLER_H_ */
