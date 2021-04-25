#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hescape.h"

#ifdef __SSE4_2__
# ifdef _MSC_VER
#  include <nmmintrin.h>
# else
#  include <x86intrin.h>
# endif
#endif

#if __GNUC__ >= 3
# define likely(x) __builtin_expect(!!(x), 1)
# define unlikely(x) __builtin_expect(!!(x), 0)
#else
# define likely(x) (x)
# define unlikely(x) (x)
#endif

static const uint8_t ESCAPED_STRING[6][7] = {
  "",
  "&quot;",
  "&amp;",
  "&#39;",
  "&lt;",
  "&gt;",
};

// This is strlen(ESCAPED_STRING[x]) optimized specially.
// Mapping: 1 => 6, 2 => 5, 3 => 5, 4 => 4, 5 => 4
#define ESC_LEN(x) ((13 - x) / 2)

/*
 * Given ASCII-compatible character, return index of ESCAPED_STRING.
 *
 * " (34) => 1 (&quot;)
 * & (38) => 2 (&amp;)
 * ' (39) => 3 (&#39;)
 * < (60) => 4 (&lt;)
 * > (62) => 5 (&gt;)
 */
static const char HTML_ESCAPE_TABLE[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 5, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static uint8_t*
ensure_allocated(uint8_t *buf, size_t size, size_t *asize)
{
  if (size < *asize)
    return buf;

  size_t new_size;
  if (*asize == 0) {
    new_size = size;
  } else {
    new_size = *asize;
  }

  // Increase buffer size by 1.5x if realloced multiple times.
  while (new_size < size)
    new_size = (new_size << 1) - (new_size >> 1);

  *asize = new_size;
  return (uint8_t*)realloc(buf, new_size);
}

#ifdef __SSE4_2__
static size_t
find_char_fast(const char *buf, size_t i, size_t size, __m128i range, size_t range_size, int *found)
{
  size_t left = (size - i) & ~15;
  do {
    __m128i b16 = _mm_loadu_si128((void *)(buf + i));
    int index = _mm_cmpestri(range, range_size, b16, 16, _SIDD_CMP_EQUAL_ANY);
    if (unlikely(index != 16)) {
      i += index;
      *found = 1;
      break;
    }
    i += 16;
    left -= 16;
  } while(likely(left != 0));

  return i;
}
#endif

static inline size_t
append_pending_buf(uint8_t *rbuf, size_t rbuf_i, const uint8_t *buf, size_t buf_i, size_t esize)
{
  memcpy(rbuf + rbuf_i, buf + (rbuf_i - esize), buf_i - (rbuf_i - esize));
  return buf_i + esize;
}

static inline size_t
append_escaped_buf(uint8_t *rbuf, size_t rbuf_i, size_t esc_i, size_t *esize)
{
  memcpy(rbuf + rbuf_i, ESCAPED_STRING[esc_i], ESC_LEN(esc_i));
  *esize += ESC_LEN(esc_i) - 1;
  return rbuf_i + ESC_LEN(esc_i);
}

std::string_view hesc_escape_html(std::string& data, bool& allocd)
{
  const uint8_t *buf = (uint8_t *)data.data();
  size_t size = data.size();
  size_t asize = 0, esc_i, esize = 0, i = 0, rbuf_i = 0;
  //const uint8_t *esc;
  uint8_t *rbuf = NULL;

# ifdef __SSE4_2__
  __m128i escapes5 = _mm_loadu_si128((const __m128i *)"\"&'<>");
  while (likely(size - i >= 16)) {
    int found = 0;
    if (unlikely((esc_i = HTML_ESCAPE_TABLE[buf[i]]) == 0)) {
      i = find_char_fast(buf, i, size, escapes5, 5, &found);
      if (!found) break;
      esc_i = HTML_ESCAPE_TABLE[buf[i]];
    }
    rbuf = ensure_allocated(rbuf, sizeof(uint8_t) * (size + esize + ESC_LEN(esc_i) + 1), &asize);
    rbuf_i = append_pending_buf(rbuf, rbuf_i, buf, i, esize);
    rbuf_i = append_escaped_buf(rbuf, rbuf_i, esc_i, &esize);
    i++;
  }
# endif

  while (i < size) {
    // Loop here to skip non-escaped characters fast.
    while (i < size && (esc_i = HTML_ESCAPE_TABLE[buf[i]]) == 0)
      i++;

    if (esc_i) {
      rbuf = ensure_allocated(rbuf, sizeof(uint8_t) * (size + esize + ESC_LEN(esc_i) + 1), &asize);
      rbuf_i = append_pending_buf(rbuf, rbuf_i, buf, i, esize);
      rbuf_i = append_escaped_buf(rbuf, rbuf_i, esc_i, &esize);
    }
    i++;
  }

  if (rbuf_i == 0) {
	allocd = false;
    // Return given buf and size if there are no escaped characters.
    return std::string_view(data);
  } else {
    append_pending_buf(rbuf, rbuf_i, buf, size, esize);
    //rbuf[size + esize] = '\0';
    allocd = true;
    //*dest = rbuf;
    return std::string_view{(char*)rbuf, size + esize};
  }
}
