#ifndef HESCAPE_H
#define HESCAPE_H

#include <sys/types.h>
#include <stdint.h>
#include "string"
#include "string_view"

/*
 * Replace characters according to the following rules.
 * Note that this function can handle only ASCII-compatible string.
 *
 * " => &quot;
 * & => &amp;
 * ' => &#39;
 * < => &lt;
 * > => &gt;
 *
 * @return size of dest. If it's larger than len, dest is required to be freed.
 */
extern std::string_view hesc_escape_html(const uint8_t *buf, size_t size, std::string& data, bool& allocd);

#endif
