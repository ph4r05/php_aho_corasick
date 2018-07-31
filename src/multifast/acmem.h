//
// Created by Dusan Klinec on 01.12.15.
//

#ifndef PHP_AHO_CORASICK_ACMEM_H
#define PHP_AHO_CORASICK_ACMEM_H

#ifdef ZTS
#include "TSRM.h"
#endif

#include "php.h"
#include "php_ini.h"

// Override memory allocator for Aho-Corasick library
#define AC_MALLOC emalloc
#define AC_MFREE efree

#endif //PHP_AHO_CORASICK_ACMEM_H
