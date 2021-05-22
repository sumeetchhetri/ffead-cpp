#include <string.h>

void* fc_memcpy(void* ptr, char* c, size_t len) {
	memcpy(ptr, (const void *)c, len);
	ptr += len;
	return ptr;
}
