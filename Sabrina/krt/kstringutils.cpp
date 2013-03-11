#include "kstringutils.hpp"

#include <cstring>

kbool_t krt_strequ(kstring_t s1, kstring_t s2)
{
	for (knuint_t i = 0; s1[i] || s2[i]; ++i)
		if (s1[i] != s2[i])
			return false;

	return true;
}

kstring_t krt_strdup(kstring_t src)
{
	knuint_t len = wcslen(src);
	kchar_t *s = new kchar_t[len + 1];
	for (knuint_t i = 0; i < len; ++i)
		s[i] = src[i];
	s[len] = NULL;
	return s;
}

kstring_t krt_strdup(kstring_t src, knuint_t len)
{
	kchar_t *s = new kchar_t[len + 1];
	for (knuint_t i = 0; i < len; ++i)
		s[i] = src[i];
	s[len] = NULL;
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

	s[len] = NULL;
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

	s[len] = NULL;
	return s;
}
