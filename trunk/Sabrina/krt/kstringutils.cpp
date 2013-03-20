#include "kstringutils.hpp"

#include "kconfig.hpp"

#include <cstring>
#include <cwchar>

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
	kchar_t *s = new kchar_t[len];
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
	kchar_t *s = new kchar_t[len];
	knuint_t i, j;

	for (i = 0, j = 0; i < len1; ++i, ++j)
		s[j] = s1[i];

	for (i = 0; i < len2; ++i, ++j)
		s[j] = s2[i];

	s[len] = L'\0';
	return s;
}

kstring_t krt_getdirpath(kstring_t fullPath)
{
#ifdef ISWIN
	const kchar_t *lastSlash = wcsrchr(fullPath, '\\');
#else
	const kchar_t *lastSlash = wcsrchr(fullPath, '/');
#endif

	knuint_t len = (lastSlash) ? (lastSlash - fullPath + 1) : 1;
	return krt_strdup(fullPath, len);
}

kstring_t krt_getfilename(kstring_t fullPath)
{
#ifdef ISWIN
	const kchar_t *lastSlash = wcsrchr(fullPath, '\\');
#else
	const kchar_t *lastSlash = wcsrchr(fullPath, '/');
#endif

	return krt_strdup(lastSlash);
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
