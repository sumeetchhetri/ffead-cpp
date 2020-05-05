/*
	Copyright 2009-2020, Sumeet Chhetri

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
 * TeBkFortune.h
 *
 *  Created on: 11-Mar-2015
 *      Author: sumeetc
 */

#ifndef TEBKFORTUNE_H_
#define TEBKFORTUNE_H_
#include "string"



#pragma @Entity
#pragma @Table name="fortune"
class TeBkFortune {
	#pragma @Id dbf="id"
	int id;
	#pragma @Column dbf="message"
	std::string message;
public:
	TeBkFortune();
	virtual ~TeBkFortune();
	int getId() const;
	void setId(int id);
	const std::string& getMessage() const;
	void setMessage(const std::string& message);
	bool operator < (const TeBkFortune& other) const;
};

#endif /* TEBKFORTUNE_H_ */
