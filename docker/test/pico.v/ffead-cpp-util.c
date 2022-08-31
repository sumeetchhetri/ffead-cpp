#include <string.h>

void* fc_memcpy(void* ptr, char* c, size_t len) {
	memcpy(ptr, (const void *)c, len);
	ptr += len;
	return ptr;
}

const char* get_date_2() {
	time_t t;
	struct tm tm;
	static const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	static __thread char date[52] = "Date: Thu, 01 Jan 1970 00:00:00 GMT\r\nServer: fcpv\r\n";

	time(&t);
	gmtime_r(&t, &tm);
	strftime(date, 51, "Date: ---, %d --- %Y %H:%M:%S GMT\r\nServer: fcpv\r\n", &tm);
	memcpy(date + 6, days[tm.tm_wday], 3);
	memcpy(date + 14, months[tm.tm_mon], 3);

	return date;
}
