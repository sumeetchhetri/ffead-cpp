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
 * MarkerRestController.cpp
 *
 *  Created on: Apr 7, 2011
 *      Author: sumeet
 */

#include "MarkerRestController.h"

MarkerRestController::MarkerRestController() {
	// TODO Auto-generated constructor stub

}

MarkerRestController::~MarkerRestController() {
	// TODO Auto-generated destructor stub
}

int MarkerRestController::addNumbers(int a, int b)
{
	cout << "Processed input request inside MarkerRestController for addNumbers..." << endl;
	return a + b;
}
