/*
 * php_hello.h: PHP Aho Corasick extension header file
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
    Inspiration taken from:
        http://devzone.zend.com/446/extension-writing-part-iii-resources/
*/

#ifndef PHP_HELLO_H
#define PHP_HELLO_H 1
#ifdef ZTS
#include "TSRM.h"
#endif

// aho corasick import
#include "ahocorasick.h"

ZEND_BEGIN_MODULE_GLOBALS(hello)
    long counter;
    zend_bool direction;
ZEND_END_MODULE_GLOBALS(hello)

#ifdef ZTS
#define HELLO_G(v) TSRMG(hello_globals_id, zend_hello_globals *, v)
#else
#define HELLO_G(v) (hello_globals.v)
#endif

#define PHP_HELLO_WORLD_VERSION "1.0"
#define PHP_HELLO_WORLD_EXTNAME "hello"

typedef struct _php_hello_person {
    char *name;
    int name_len;
    long age;
} php_hello_person;

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
#define PHP_HELLO_PERSON_RES_NAME "Person Data"

PHP_MINIT_FUNCTION(hello);
PHP_MSHUTDOWN_FUNCTION(hello);
PHP_RINIT_FUNCTION(hello);
PHP_FUNCTION(ahocorasick_match);
PHP_FUNCTION(ahocorasick_init);
PHP_FUNCTION(ahocorasick_deinit);
PHP_FUNCTION(hello_lower);
PHP_FUNCTION(hello_world);
PHP_FUNCTION(hello_long);
PHP_FUNCTION(hello_double);
PHP_FUNCTION(hello_bool);
PHP_FUNCTION(hello_null);
PHP_FUNCTION(hello_greetme);
PHP_FUNCTION(hello_add);
PHP_FUNCTION(hello_dump);
PHP_FUNCTION(hello_array);
PHP_FUNCTION(hello_array_strings);
PHP_FUNCTION(hello_array_walk);
PHP_FUNCTION(hello_array_value);
PHP_FUNCTION(hello_get_global_var);
PHP_FUNCTION(hello_set_local_var);
PHP_FUNCTION(hello_person_new);
PHP_FUNCTION(hello_person_pnew);
PHP_FUNCTION(hello_person_greet);
PHP_FUNCTION(hello_person_delete);

extern zend_module_entry hello_module_entry;
#define phpext_hello_ptr &hello_module_entry

#endif