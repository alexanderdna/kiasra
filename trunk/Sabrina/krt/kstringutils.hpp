#pragma once

#include "kni.h"

kbool_t krt_strequ(kstring_t s1, kstring_t s2);

// Returns true if s1 == (s2 + s3)
kbool_t krt_strequ(kstring_t s1, kstring_t s2, kstring_t s3);

kstring_t krt_strdup(kstring_t src);
kstring_t krt_strdup(kstring_t src, knuint_t len);

kstring_t krt_strcat(kstring_t s1, kstring_t s2);
kstring_t krt_strcat(kstring_t s1, knuint_t len1, kstring_t s2, knuint_t len2);

kstring_t krt_getdirpath(kstring_t fullPath);
kstring_t krt_getfilename(kstring_t fullPath);

const char * krt_wcstostr(kstring_t src);
