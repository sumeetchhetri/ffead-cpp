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

// date
#include <time.h>

const char* get_date() {
	time_t t;
	struct tm tm;
	static const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	static __thread char date[30] = "Thu, 01 Jan 1970 00:00:00 GMT";

	time(&t);
	gmtime_r(&t, &tm);
	strftime(date, 30, "---, %d --- %Y %H:%M:%S GMT", &tm);
	memcpy(date, days[tm.tm_wday], 3);
	memcpy(date + 8, months[tm.tm_mon], 3);

	return date;
}

