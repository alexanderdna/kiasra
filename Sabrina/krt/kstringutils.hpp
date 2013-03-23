#pragma once

#include "kni.h"

kbool_t krt_strequ(kstring_t s1, kstring_t s2);
kbool_t krt_strequ(kstring_t s1, kstring_t s2, knuint_t length1, knuint_t length2);

// Returns true if s1 == (s2 + s3)
kbool_t krt_strequ(kstring_t s1, kstring_t s2, kstring_t s3);

kstring_t krt_strdup(kstring_t src);
kstring_t krt_strdup(kstring_t src, knuint_t len);

kstring_t krt_strcat(kstring_t s1, kstring_t s2);
kstring_t krt_strcat(kstring_t s1, knuint_t len1, kstring_t s2, knuint_t len2);

const char * krt_pathdup(const char *path);
const char * krt_getdirpath(const char *fullpath);
const char * krt_getfilename(const char *fullpath);
const char * krt_getfileext(const char *filename);

bool krt_pathequ(const char *s1, const char *s2);

const char * krt_getcwd(void);
const char * krt_combpath(const char *dir, const char *file);

const char * krt_wcstostr(kstring_t src);
kstring_t krt_strtowcs(const char *src);
