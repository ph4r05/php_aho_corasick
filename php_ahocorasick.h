/*
 * php_ahocorasick.h: PHP Aho Corasick extension header file
 *
    Copyright 2010-2013 Ph4r05 <ph4r05@gmail.com>

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this software.  If not, see <http://www.gnu.org/licenses/>.
  
    This code uses thirdparty code:
        MultiFast (http://sourceforge.net/projects/multifast/?source=dlp)
*/

#ifndef PHP_AHOCORASICK_H
#define PHP_AHOCORASICK_H 1
#ifdef ZTS
#include "TSRM.h"
#endif

// aho corasick import
#include "ahocorasick.h"

ZEND_BEGIN_MODULE_GLOBALS(ahocorasick)
    long counter;
    zend_bool direction;
ZEND_END_MODULE_GLOBALS(ahocorasick)

#ifdef ZTS
#define AHOCORASICK_G(v) TSRMG(ahocorasick_globals_id, zend_ahocorasick_globals *, v)
#else
#define AHOCORASICK_G(v) (ahocorasick_globals.v)
#endif

#define PHP_AHOCORASICK_VERSION "1.0"
#define PHP_AHOCORASICK_EXTNAME "ahocorasick"

/**
 * Sample param to pass to callback handler for AhoCorasick search algorithm
 */
struct aho_callback_payload {
        // found match will be added here
        int retVal;
        zval * resultArray;
};

/**
 * AhoCorasick table initialization
 */
typedef struct _ahostruct {
    char *key;
    char *value;
    int valueLen;
    int ignoreCase;
} ahostruct;

/**
 * Initialized Aho Corasick search structure - resource
 */
typedef struct _ahoMasterStruct {
    // aho corasick main search tree
    AC_AUTOMATA_t * acap;
    // string buffer for aho corasick (pointers to memory)
    ahostruct ** ahostructbuff;
    // length of buffer array above
    long ahobufflen;
    // just testing integer
    int test;
} ahoMasterStruct;

#define PHP_AHOSTRUCT_MASTER_RES_NAME "Ahostruct master data"
#define PHP_AHOSTRUCT_RES_NAME "Ahostruct element data"

PHP_MINIT_FUNCTION(ahocorasick);
PHP_MSHUTDOWN_FUNCTION(ahocorasick);
PHP_RINIT_FUNCTION(ahocorasick);
PHP_FUNCTION(ahocorasick_match);
PHP_FUNCTION(ahocorasick_init);
PHP_FUNCTION(ahocorasick_deinit);
PHP_FUNCTION(ahocorasick_isValid);
extern zend_module_entry ahocorasick_module_entry;
#define phpext_hello_ptr &ahocorasick_module_entry

#endif
