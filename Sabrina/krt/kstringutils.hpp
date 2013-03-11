#pragma once

#include "kni.h"

kbool_t krt_strequ(kstring_t s1, kstring_t s2);

kstring_t krt_strdup(kstring_t src);
kstring_t krt_strdup(kstring_t src, knuint_t len);

kstring_t krt_strcat(kstring_t s1, kstring_t s2);
kstring_t krt_strcat(kstring_t s1, knuint_t len1, kstring_t s2, knuint_t len2);
