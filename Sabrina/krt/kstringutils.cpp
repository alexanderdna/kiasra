#include "kstringutils.hpp"

#include "kconfig.hpp"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cwchar>
#include <cerrno>

#ifdef ISWIN
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <direct.h>
#else
	#include <unistd.h>
	#include <linux/limits.h>
#endif

kbool_t krt_strequ(kstring_t s1, kstring_t s2)
{
	for (knuint_t i = 0; s1[i] || s2[i]; ++i)
		if (s1[i] != s2[i])
			return false;

	return true;
}

kbool_t krt_strequ(kstring_t s1, kstring_t s2, knuint_t length1, knuint_t length2)
{
	if (length1 != length2)
		return false;

	for (knuint_t i = 0; i < length1; ++i)
		if (s1[i] != s2[i])
			return false;

	return true;
}

kbool_t krt_strequ(kstring_t s1, kstring_t s2, kstring_t s3)
{
	knuint_t i, j;

	for (i = 0; s1[i] && s2[i]; ++i)
		if (s1[i] != s2[i])
			return false;

	for (j = 0; s1[i] || s3[j]; ++i, ++j)
		if (s1[i] != s3[j])
			return false;

	return true;
}

kstring_t krt_strdup(kstring_t src)
{
	knuint_t len = wcslen(src);
	kchar_t *s = new kchar_t[len + 1];
	for (knuint_t i = 0; i < len; ++i)
		s[i] = src[i];
	s[len] = L'\0';
	return s;
}

kstring_t krt_strdup(kstring_t src, knuint_t len)
{
	kchar_t *s = new kchar_t[len + 1];
	for (knuint_t i = 0; i < len; ++i)
		s[i] = src[i];
	s[len] = L'\0';
	return s;
}

kstring_t krt_strcat(kstring_t s1, kstring_t s2)
{
	knuint_t len1 = wcslen(s1), len2 = wcslen(s2);
	knuint_t len = len1 + len2;
	kchar_t *s = new kchar_t[len + 1];
	knuint_t i, j;

	for (i = 0, j = 0; i < len1; ++i, ++j)
		s[j] = s1[i];

	for (i = 0; i < len2; ++i, ++j)
		s[j] = s2[i];

	s[len] = L'\0';
	return s;
}

kstring_t krt_strcat(kstring_t s1, knuint_t len1, kstring_t s2, knuint_t len2)
{
	knuint_t len = len1 + len2;
	kchar_t *s = new kchar_t[len + 1];
	knuint_t i, j;

	for (i = 0, j = 0; i < len1; ++i, ++j)
		s[j] = s1[i];

	for (i = 0; i < len2; ++i, ++j)
		s[j] = s2[i];

	s[len] = L'\0';
	return s;
}

const char * krt_pathdup(const char *path)
{
	knuint_t len = strlen(path);

	char *s = new char[len + 1];
	strncpy(s, path, len);
	s[len] = '\0';
	
	return s;
}

const char * krt_getdirpath(const char *fullpath)
{
	const char *lastSlash = strrchr(fullpath, PATH_SLASH);
	knuint_t len = (lastSlash) ? (lastSlash - fullpath) : 0;

	char *s = new char[len + 1];
	strncpy(s, fullpath, len);
	s[len] = '\0';
	
	return s;
}

const char * krt_getfilename(const char *fullpath)
{
	const char *lastSlash = strrchr(fullpath, PATH_SLASH);
	kuint_t len = lastSlash ? (strlen(fullpath) - (lastSlash - fullpath) - 1) : strlen(fullpath);

	char *s = new char[len + 1];
	strncpy(s, lastSlash + 1, len);
	s[len] = '\0';
	
	return s;
}

const char * krt_getfileext(const char *filename)
{
	const char *dot = strrchr(filename, '.');
	if (dot)
		return dot+1;
	else
		return NULL;
}

bool krt_pathequ(const char *s1, const char *s2)
{
	for (knuint_t i = 0; s1[i] || s2[i]; ++i)
		if (s1[i] != s2[i])
			return false;

	return true;
}

const char * krt_getcwd(void)
{
	knuint_t bufferSize = 512;
	char *buffer = new char[bufferSize];
#ifdef ISWIN
	char *p = _getcwd(buffer, bufferSize-1);
#else
	char *p = getcwd(buffer, bufferSize-1);
#endif
	while (!p)
	{
		if (errno == ERANGE)
		{
			delete []buffer;
			bufferSize *= 2;
			buffer = new char[bufferSize];
#ifdef ISWIN
			p = _getcwd(buffer, bufferSize-1);
#else
			p = getcwd(buffer, bufferSize-1);
#endif
		}
		else
		{
			delete []buffer;
			return NULL;
		}
	}

	kuint_t cwdlen = strlen(buffer);

	// append trailing slash if not found
	if (buffer[cwdlen - 1] != PATH_SLASH)
	{
		buffer[cwdlen] = PATH_SLASH;
		buffer[++cwdlen] = '\0';
	}

	return buffer;
}

const char * krt_combpath(const char *dir, const char *file)
{
	kuint_t lenDir = strlen(dir);
	kuint_t lenFile = strlen(file);
	kuint_t len = lenDir + lenFile;

	char *path = new char[len + 1];
	strncpy(path, dir, lenDir);
	strncpy(path + lenDir, file, lenFile);

	path[len] = '\0';
	return path;
}

const char * krt_wcstostr(kstring_t src)
{
	knuint_t len = wcslen(src);
	char *s = new char[len + 1];
	for (knuint_t i = 0; i < len; ++i)
		s[i] = (char)src[i];
	s[len] = '\0';

	return s;
}

kstring_t krt_strtowcs(const char *src)
{
	knuint_t len = strlen(src);
	kchar_t *s = new kchar_t[len + 1];
	for (knuint_t i = 0; i < len; ++i)
		s[i] = (kchar_t)src[i];
	s[len] = L'\0';

	return s;
}
