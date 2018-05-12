/*
 * CompressionUtil.cpp
 *
 *  Created on: 19-Apr-2013
 *      Author: sumeetc
 */

#include "CompressionUtil.h"

CompressionUtil::CompressionUtil() {
}


std::string CompressionUtil::_compress(char* infile, const bool& isGz, const bool& retu, char* toFile)
{
	int chunkSize = 8192;

	unsigned char in[chunkSize];
	unsigned char out[chunkSize];
	memset(in, 0, sizeof(in));
	memset(out, 0, sizeof(out));

    int ret, flush;
    unsigned int have;
    z_stream strm;

    std::string s;
    FILE* ofs;
    FILE* ifs;
    if(infile!=NULL)
    {
    	ifs = fopen(infile, "rb");
    }
    if(toFile!=NULL)
    {
    	ofs = fopen(toFile, "wb");
    }

    /* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	if(isGz)
	{
		ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, (15+16), 8, Z_DEFAULT_STRATEGY);
	}
	else
	{
		ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
	}

	if (ret != Z_OK)
	{
		if(ofs!=NULL)
		{
			fclose(ofs);
		}
		if(ifs!=NULL)
		{
			fclose(ifs);
		}
		return s;
	}

	/* compress until end of file */
	do {
        strm.avail_in = fread(in, 1, chunkSize, ifs);
        if (ferror(ifs)) {
            (void)deflateEnd(&strm);
            return s;
        }
        flush = feof(ifs) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = in;

		/* run deflate() on input until output buffer not full, finish
		   compression if all of source has been read in */
		do {
			strm.avail_out = chunkSize;
			strm.next_out = out;
			ret = deflate(&strm, flush);    /* no bad return value */
			assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			have = chunkSize - strm.avail_out;
			if(ofs!=NULL)
			{
				if (fwrite(out, 1, have, ofs) != have || ferror(ofs)) {
					(void)deflateEnd(&strm);
					return s;
				}
				if(retu)
				{
					s.append((char*)&out[0], have);
				}
			}
			else
			{
				s.append((char*)&out[0], have);
			}
		} while (strm.avail_out == 0);
		assert(strm.avail_in == 0);     /* all input will be used */

		/* done when last data in file processed */
	} while (flush != Z_FINISH);
	assert(ret == Z_STREAM_END);        /* stream will be complete */

	/* clean up and return */
	(void)deflateEnd(&strm);

	if(ofs!=NULL)
	{
		fclose(ofs);
	}
	if(ifs!=NULL)
	{
		fclose(ifs);
	}

    return s;
}

std::string CompressionUtil::_uncompress(char* infile, const bool& isGz, const bool& retu, char* toFile)
{
	int chunkSize = 8192;

	unsigned char in[chunkSize];
	unsigned char out[chunkSize];
	memset(in, 0, sizeof(in));
	memset(out, 0, sizeof(out));

    int ret;
    unsigned int have;
    z_stream strm;

    std::string s;
    FILE* ofs;
    FILE* ifs;
    if(infile!=NULL)
    {
    	ifs = fopen(infile, "rb");
    }
    if(toFile!=NULL)
    {
    	ofs = fopen(toFile, "wb");
    }

    /* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	if(isGz)
	{
		ret = inflateInit2(&strm, (15+16));
	}
	else
	{
		ret = inflateInit(&strm);
	}

	if (ret != Z_OK)
	{
		if(ofs!=NULL)
		{
			fclose(ofs);
		}
		if(ifs!=NULL)
		{
			fclose(ifs);
		}
		return s;
	}

	/* decompress until deflate stream ends or end of file */
	do {
        strm.avail_in = fread(in, 1, chunkSize, ifs);
        if (ferror(ifs)) {
            (void)deflateEnd(&strm);
            return s;
        }
        //flush = feof(ifs) ? Z_FINISH : Z_NO_FLUSH;

		if (strm.avail_in == 0)
			break;

		strm.next_in = in;

		/* run inflate() on input until output buffer not full */
		do {
			strm.avail_out = chunkSize;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret) {
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
				break;
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				return s;
			}
			have = chunkSize - strm.avail_out;
			if(ofs!=NULL)
			{
				if (fwrite(out, 1, have, ofs) != have || ferror(ofs)) {
					(void)deflateEnd(&strm);
					return s;
				}
				if(retu)
				{
					s.append((char*)&out[0], have);
				}
			}
			else
			{
				s.append((char*)&out[0], have);
			}
		} while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	} while (ret != Z_STREAM_END);

	/* clean up and return */
	(void)inflateEnd(&strm);

	if(ofs!=NULL)
	{
		fclose(ofs);
	}
	if(ifs!=NULL)
	{
		fclose(ifs);
	}

    return s;
}


/* Compress from file source to file dest until EOF on source.
   def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_STREAM_ERROR if an invalid compression
   level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
   version of the library linked do not match, or Z_ERRNO if there is
   an error reading or writing the files. */
std::string CompressionUtil::_compress(char* datain, const size_t& insize, const bool& eostream, const bool& isGz, const int& chunkSize, const bool& retu, char* toFile)
{
	unsigned char out[chunkSize];
	memset(out, 0, sizeof(out));

    int ret, flush;
    unsigned int have;
    z_stream strm;

    std::string s;
    std::ofstream ofs;
    if(toFile!=NULL)
    {
    	ofs.open(toFile, std::ios::binary);
    }

    float parts = (float)insize/chunkSize;
    parts = (floor(parts)<parts?floor(parts)+1:floor(parts));
    if(parts==1)
    {
    	/* allocate deflate state */
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		if(isGz)
		{
			ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, (15+16), 8, Z_DEFAULT_STRATEGY);
		}
		else
		{
			ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
		}
		if (ret != Z_OK)
			return s;//return ret;

		strm.avail_in = insize;
		strm.next_in = (unsigned char*)&datain[0];

		if(eostream)
		{
			flush = Z_FINISH;
		}
		else
		{
			flush = Z_NO_FLUSH;
		}

		/* run deflate() on input until output buffer not full, finish
		   compression if all of source has been read in */
		do {
			strm.avail_out = chunkSize;
			strm.next_out = out;
			ret = deflate(&strm, flush);    /* no bad return value */
			assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			have = chunkSize - strm.avail_out;
			if(ofs.is_open())
			{
				ofs.write((char*)&out[0], have);
				if(retu)
				{
					s.append((char*)&out[0], have);
				}
			}
			else
			{
				s.append((char*)&out[0], have);
			}
		} while (strm.avail_out == 0);
		assert(strm.avail_in == 0);

		/* clean up and return */
		(void)deflateEnd(&strm);
	}
    else
    {
		/* allocate deflate state */
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		//ret = deflateInit(&strm, level);
		if(isGz)
		{
			ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, (15+16), 8, Z_DEFAULT_STRATEGY);
		}
		else
		{
			ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
		}
		if (ret != Z_OK)
			return s;//return ret;

		flush = Z_NO_FLUSH;

		for (int var = 0; var < parts; ++var) {

			if(var==parts-1)
			{
				if(eostream)
				{
					flush = Z_FINISH;
				}
				else
				{
					flush = Z_NO_FLUSH;
				}
			}

			unsigned int len = insize - chunkSize*var;
			if(len>8192)
			{
				len = chunkSize;
			}

			strm.avail_in = len;
			strm.next_in = (unsigned char*)&datain[chunkSize*var];

			/* run deflate() on input until output buffer not full, finish
			   compression if all of source has been read in */
			do {
				strm.avail_out = chunkSize;
				strm.next_out = out;
				ret = deflate(&strm, flush);    /* no bad return value */
				assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
				have = chunkSize - strm.avail_out;
				if(ofs.is_open())
				{
					ofs.write((char*)&out[0], have);
					if(retu)
					{
						s.append((char*)&out[0], have);
					}
				}
				else
				{
					s.append((char*)&out[0], have);
				}
			} while (strm.avail_out == 0);
			assert(strm.avail_in == 0);
		}

		/* clean up and return */
		(void)deflateEnd(&strm);
    }

	if(ofs.is_open())
	{
		ofs.close();
	}

    return s;
}

/* Decompress from file source to file dest until stream ends or EOF.
   inf() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_DATA_ERROR if the deflate data is
   invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
   the version of the library linked do not match, or Z_ERRNO if there
   is an error reading or writing the files. */
std::string CompressionUtil::_uncompress(char* datain, const size_t& insize, const bool& isGz, const bool& retu, char* toFile)
{
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char out[CHUNK];
	memset(out, 0, sizeof(out));

    std::string s;
    std::ofstream ofs;
    if(toFile!=NULL)
    {
    	ofs.open(toFile, std::ios::binary);
    }

	float parts = (float)insize/CHUNK;
	parts = (floor(parts)<parts?floor(parts)+1:floor(parts));

	if(parts==1)
	{
		/* allocate inflate state */
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		strm.avail_in = 0;
		strm.next_in = Z_NULL;

		if(isGz)
		{
			ret = inflateInit2(&strm, (15+16));
		}
		else
		{
			ret = inflateInit(&strm);
		}
		if (ret != Z_OK)
			return s;//return ret;

		strm.avail_in = insize;
		if (strm.avail_in == 0)
			return s;
		strm.next_in = (unsigned char*)&datain[0];

		/* run inflate() on input until output buffer not full */
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret) {
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
				break;
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				return s;
			}
			have = CHUNK - strm.avail_out;
			if(ofs.is_open())
			{
				ofs.write((char*)&out[0], have);
				if(retu)
				{
					s.append((char*)&out[0], have);
				}
			}
			else
			{
				s.append((char*)&out[0], have);
			}
		} while (strm.avail_out == 0);

		/* clean up and return */
		(void)inflateEnd(&strm);
	}
	else
	{
		/* allocate inflate state */
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		strm.avail_in = 0;
		strm.next_in = Z_NULL;

		if(isGz)
		{
			ret = inflateInit2(&strm, (15+16));
		}
		else
		{
			ret = inflateInit(&strm);
		}
		if (ret != Z_OK)
			return s;//return ret;

		for (int var = 0; var < parts; ++var) {
			unsigned int len = insize - CHUNK*var;
			if(len>8192)
			{
				len = CHUNK;
			}

			strm.avail_in = len;
			if (strm.avail_in == 0)
				break;
			strm.next_in = (unsigned char*)&datain[CHUNK*var];

			/* run inflate() on input until output buffer not full */
			do {
				strm.avail_out = CHUNK;
				strm.next_out = out;
				ret = inflate(&strm, Z_NO_FLUSH);
				assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
				switch (ret) {
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;     /* and fall through */
					break;
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);
					return s;
				}
				have = CHUNK - strm.avail_out;
				if(ofs.is_open())
				{
					ofs.write((char*)&out[0], have);
					if(retu)
					{
						s.append((char*)&out[0], have);
					}
				}
				else
				{
					s.append((char*)&out[0], have);
				}
			} while (strm.avail_out == 0);
		}

		/* clean up and return */
		(void)inflateEnd(&strm);
	}

	if(ofs.is_open())
	{
		ofs.close();
	}

	return s;
}


std::string CompressionUtil::zlibCompress(char* input, const size_t& siz, const bool& eostream, const int& chunkSize, const bool& ret, char* toFile) {
	std::string compData = _compress(input, siz, eostream, false, chunkSize, ret, toFile);
	if(toFile==NULL)
	{
		return compData;
	}
	else
	{
		if(ret)
		{
			return compData;
		}
		else
		{
			return "";
		}
	}
}

std::string CompressionUtil::zlibCompress(const std::string& input, const bool& eostream, const int& chunkSize, const bool& ret, char* toFile) {
	return zlibCompress((char*)input.c_str(), input.length(), eostream, chunkSize, ret, toFile);
}

std::string CompressionUtil::gzipCompress(char* input, const size_t& siz, const bool& eostream, const int& chunkSize, const bool& ret, char* toFile) {
	std::string compData = _compress(input, siz, eostream, true, chunkSize, ret, toFile);
	if(toFile==NULL)
	{
		return compData;
	}
	else
	{
		if(ret)
		{
			return compData;
		}
		else
		{
			return "";
		}
	}
}

std::string CompressionUtil::gzipCompress(const std::string& input, const bool& eostream, const int& chunkSize, const bool& ret, char* toFile) {
	return gzipCompress((char*)input.c_str(), input.length(), eostream, chunkSize, ret, toFile);
}

std::string CompressionUtil::zlibUnCompress(char* input, const size_t& siz, const bool& ret, char* toFile) {
	std::string compData = _uncompress(input, siz, false, ret, toFile);
	if(toFile==NULL)
	{
		return compData;
	}
	else
	{
		if(ret)
		{
			return compData;
		}
		else
		{
			return "";
		}
	}
}

std::string CompressionUtil::zlibUnCompress(const std::string& input, const bool& ret, char* toFile) {
	return zlibUnCompress((char*)input.c_str(), input.length(), ret, toFile);
}

std::string CompressionUtil::gzipUnCompress(char* input, const size_t& siz, const bool& ret, char* toFile) {
	std::string compData = _uncompress(input, siz, true, ret, toFile);
	if(toFile==NULL)
	{
		return compData;
	}
	else
	{
		if(ret)
		{
			return compData;
		}
		else
		{
			return "";
		}
	}
}

std::string CompressionUtil::gzipUnCompress(const std::string& input, const bool& ret, char* toFile) {
	return gzipUnCompress((char*)input.c_str(), input.length(), ret, toFile);
}

std::string CompressionUtil::zlibCompressFile(char* infile, const bool& retu, char* toFile) {
	return _compress(infile, false, retu, toFile);
}

std::string CompressionUtil::gzipCompressFile(char* infile, const bool& retu, char* toFile) {
	return _compress(infile, true, retu, toFile);
}

std::string CompressionUtil::zlibUnCompressFile(char* infile, const bool& retu, char* toFile) {
	return _uncompress(infile, false, retu, toFile);
}

std::string CompressionUtil::gzipUnCompressFile(char* infile, const bool& retu, char* toFile) {
	return _uncompress(infile, true, retu, toFile);
}

CompressionUtil::~CompressionUtil() {
	// TODO Auto-generated destructor stub
}

