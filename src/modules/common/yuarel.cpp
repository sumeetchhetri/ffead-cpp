/**
 * Copyright (C) 2016,2017 Jack Engqvist Johansson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "yuarel.h"

/**
 * Parse a non null terminated string into an integer.
 *
 * str: the string containing the number.
 * len: Number of characters to parse.
 */
static inline int
natoi(const char *str, size_t len)
{
	int i, r = 0;
	for (i = 0; i < (int)len; i++) {
		r *= 10;
		r += str[i] - '0';
	}

	return r;
}

/**
 * Check if a URL is relative (no scheme and hostname).
 *
 * url: the string containing the URL to check.
 *
 * Returns 1 if relative, otherwise 0.
 */
static inline int
is_relative(const char *url)
{
	return (url[0] == '/') ? 1 : 0;
}

/**
 * Parse the scheme of a URL by inserting a null terminator after the scheme.
 *
 * str: the string containing the URL to parse. Will be modified.
 *
 * Returns a pointer to the hostname on success, otherwise NULL.
 */
static inline char *
parse_scheme(char *str, size_t ulen, size_t* fl)
{
	char *s;

	/* If not found or first in string, return error */
	s = (char*)memchr(str, ':', ulen);
	if (s == NULL || s == str) {
		*fl = 0;
		return NULL;
	}

	/* If not followed by two slashes, return error */
	if (s[1] == '\0' || s[1] != '/' || s[2] == '\0' || s[2] != '/') {
		return NULL;
	}

	//*s = '\0'; // Replace ':' with NULL
	*fl = s - str;
	return s + 3;
}

/**
 * Find a character in a string, replace it with '\0' and return the next
 * character in the string.
 *
 * str: the string to search in.
 * find: the character to search for.
 *
 * Returns a pointer to the character after the one to search for. If not
 * found, NULL is returned.
 */
static inline char *
find_and_terminate(char *str, char find, size_t ulen, size_t* fl)
{
	char* str1 = (char*)memchr(str, find, ulen);
	if (NULL == str1) {
		*fl = 0;
		return NULL;
	}

	//*str = '\0';
	*fl = str + ulen - str1 - 1;
	return str1+1;
}

/* Yes, the following functions could be implemented as preprocessor macros
     instead of inline functions, but I think that this approach will be more
     clean in this case. */
static inline char *
find_fragment(char *str, size_t ulen, size_t* fl)
{
	return find_and_terminate(str, '#', ulen, fl);
}

static inline char *
find_query(char *str, size_t ulen, size_t* fl)
{
	return find_and_terminate(str, '?', ulen, fl);
}

static inline char *
find_path(char *str, size_t ulen, size_t* fl)
{
	return find_and_terminate(str, '/', ulen, fl);
}

/**
 * Parse a URL string to a struct.
 *
 * url: pointer to the struct where to store the parsed URL parts.
 * u:   the string containing the URL to be parsed.
 *
 * Returns 0 on success, otherwise -1.
 */
int
yuarel_parse(struct yuarel *url, struct yuarel_param* params, int* num_params, char *u, size_t ulen)
{
	if (NULL == url || NULL == u) {
		return -1;
	}

	memset(url, 0, sizeof (struct yuarel));

	/* (Fragment) */
	url->fragment = find_fragment(u, ulen, &url->fragment_len);

	/* (Query) */
	url->query = find_query(u, ulen, &url->query_len);
	if(url->fragment_len>0) url->query_len -= url->fragment_len + 1;

	/* Relative URL? Parse scheme and hostname */
	if (!is_relative(u)) {
		/* Scheme */
		size_t tr = 0;
		url->scheme = u;
		u = parse_scheme(u, ulen, &url->scheme_len);
		tr += url->scheme_len + 3;

		if (u == NULL) {
			return -1;
		}

		/* Host */
		if ('\0' == *u) {
			return -1;
		}
		url->host = u;

		/* (Path) */
		url->path = find_path(u, ulen-tr, &url->path_len);
		url->path = url->path - 1;
		url->path_len += 1;
		if(url->fragment_len>0) url->path_len -= url->fragment_len + 1;
		if(url->query_len>0) url->path_len -= url->query_len + 1;

		tr += url->query_len + 1 + url->fragment_len + 1 + url->path_len;

		// (Credentials)
		u = (char*)memchr(url->host, '@', ulen-tr);
		if (NULL != u) {
			// Missing credentials?
			if (u == url->host) {
				return -1;
			}

			url->username = url->host;
			url->username_len = u - url->username;
			url->host = u + 1;

			u = (char*)memchr(url->username, ':', url->username_len);
			if (NULL == u) {
				return -1;
			}

			url->password = u + 1;
			url->password_len = url->username_len - (url->password - url->username);
			url->username_len -= url->password_len + 1;
		}

		/* Missing hostname? */
		if ('\0' == *url->host) {
			return -1;
		}

		tr += url->password_len + 1 + url->username_len;

		/* (Port) */
		u = (char*)memchr(url->host, ':', ulen-tr);
		if (NULL != u) {
			url->port = natoi(u+1, ulen - tr - (u - url->host) - 2);
		}

		url->host_len = u - url->host;

		/* Missing hostname? */
		if ('\0' == *url->host) {
			return -1;
		}
	} else {
		/* (Path) */
		url->path = find_path(u, ulen, &url->path_len);
		url->path = url->path - 1;
		url->path_len += 1;
		if(url->fragment_len>0) url->path_len -= url->fragment_len;
		if(url->query_len>0) url->path_len -= url->query_len;
	}

	if(params!=NULL) {
		*num_params = yuarel_parse_query(url->query, url->query_len, params, 100);
	}

	return 0;
}

/**
 * Split a path into several strings.
 *
 * No data is copied, the slashed are used as null terminators and then
 * pointers to each path part will be stored in **parts. Double slashes will be
 * treated as one.
 *
 * path: the path to split.
 * parts: a pointer to an array of (char *) where to store the result.
 * max_parts: max number of parts to parse.
 */
int
yuarel_split_path(char *path, char **parts, int max_parts)
{
	int i = 0;

	if (NULL == path || '\0' == *path) {
		return -1;
	}

	do {
		/* Forward to after slashes */
		while (*path == '/') path++;

		if ('\0' == *path) {
			break;
		}

		parts[i++] = path;

		path = strchr(path, '/');
		if (NULL == path) {
			break;
		}

		*(path++) = '\0';
	} while (i < max_parts);

	return i;
}

int
yuarel_parse_query(char *query, size_t query_len, struct yuarel_param *params, int max_params)
{
	int i = 0;
	char* oq = query;
	if (NULL == query || '\0' == *query) {
		return -1;
	}

	while (i < max_params && NULL != (query = (char*)memchr(query, '&', query_len))) {
		params[i].key = oq;
		params[i].key_len = query++ - oq;

		oq = query;

		if ((params[i].val = (char*)memchr(params[i].key, '=', params[i].key_len)) != NULL) {
			params[i].val++;
			params[i].val_len = params[i].key_len - (params[i].val - params[i].key);
			params[i].key_len -= (params[i].val_len + 1);
		}
		query_len = query_len - params[i].val_len - params[i].key_len - 1 - 1;
		i++;
	}

	params[i].key = oq;
	params[i].key_len = query_len;

	if ((params[i].val = (char*)memchr(params[i].key, '=', params[i].key_len)) != NULL) {
		params[i].val++;
		params[i].val_len = params[i].key_len - (params[i].val - params[i].key);
		params[i].key_len -= (params[i].val_len + 1);
	}
	i++;

	return i;
}
