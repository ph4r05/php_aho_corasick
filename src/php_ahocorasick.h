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

#ifndef PHP_MAJOR_VERSION
#define PHP_MAJOR_VERSION 7
#endif

// Compatibility
#if PHP_MAJOR_VERSION < 7
#define PHP7 0
typedef long zend_long;
typedef char zend_string;
typedef int strsize_t;
#define COMPAT_ZVAL zval *
#define COMPAT_ZVAL_UNDEF(x) x = NULL
#define COMPAT_Z_ISUNDEF(x) (x == NULL)
#define COMPAT_Z_RESOURCE zval
#define COMPAT_ZEND_HASH_FOREACH_KEY_VAL(arr_hash, idx, key, data) \
  { HashPosition pointer; \
  for(zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); \
        zend_hash_get_current_data_ex(arr_hash, (void**) &(data), &pointer) == SUCCESS;  \
        zend_hash_move_forward_ex(arr_hash, &pointer), (idx)+=1)
#define COMPAT_ZEND_HASH_FOREACH_END() }

#define COMPAT_STR_EQUALS_CI(s, x) strcasecmp(s, x) == 0
#define COMPAT_RESOURCE_PARAM(x) zend_rsrc_list_entry *(x) TSRMLS_DC
#define COMPAT_ZVAL_STRING(str, len) ZVAL_STRING(str, len, 0)
#define COMPAT_RETURN_STRING(str) RETURN_STRING(str, 0)
#define COMPAT_Z_TYPE_PP(x) Z_TYPE_PP(x)
#define COMPAT_Z_TYPE_P(x) Z_TYPE_P(x)
#define COMPAT_Z_LVAL(x) Z_LVAL(*(x))
#define COMPAT_Z_STRVAL(x) Z_STRVAL(*(x))
#define COMPAT_Z_ARRVAL_P(x) Z_ARRVAL_P(*(x))
#define COMPAT_Z_ADDREF_P(x) Z_ADDREF_P(x)
#define COMPAT_Z_STRLEN(x) Z_STRLEN(*(x))
#define COMPAT_Z_STRLEN_P(x) Z_STRLEN_P(x)
#define COMPAT_Z_STRLEN_PP(x) Z_STRLEN_P(x)
#define COMPAT_Z_STRVAL_P(x) Z_STRVAL_P(x)
#define COMPAT_Z_ARREF(x) (x)
#define COMPAT_Z_REFCOUNTED(zval) 1
#define COMPAT_TRY_ADDREF_P(SRC) COMPAT_Z_ADDREF_P(SRC)

#define COMPAT_MAKE_STD_ZVAL(x) MAKE_STD_ZVAL(x)
#define COMPAT_DECLARE_ZVAL(name) zval *name
#define COMPAT_ALLOC_INIT_ZVAL(name) ALLOC_INIT_ZVAL(name)
#define COMPAT_ALLOC_INIT_ZVAL2(name) ALLOC_INIT_ZVAL(name)
#define COMPAT_ZVAL_COPY(z, v) do { \
  Z_ADDREF_P(*(v));                 \
  *z = *v;                          \
} while(0)
#define hp_ptr_dtor(val) zval_ptr_dtor(&(val))

#define COMPAT_ADD_ASSOC_ZVAL(ARR, KEY, SRC) do {                 \
  COMPAT_TRY_ADDREF_P(SRC);                                       \
  add_assoc_zval(COMPAT_Z_ARREF(ARR), KEY, COMPAT_Z_ARREF(SRC));  \
} while(0)

#else
#define PHP7 1
typedef size_t strsize_t;
#define COMPAT_ZVAL zval
#define COMPAT_ZVAL_UNDEF(x) ZVAL_UNDEF(&(x))
#define COMPAT_Z_ISUNDEF(x) Z_ISUNDEF(x)
#define COMPAT_Z_RESOURCE zend_resource
#define COMPAT_ZEND_HASH_FOREACH_KEY_VAL(arr_hash, idx, key, data) ZEND_HASH_FOREACH_KEY_VAL(arr_hash, idx, key, data)
#define COMPAT_ZEND_HASH_FOREACH_END() ZEND_HASH_FOREACH_END()
#define COMPAT_STR_EQUALS_CI(s, x) zend_string_equals_ci(zend_string_init(ZEND_STRL(s), 0), x)
#define COMPAT_RESOURCE_PARAM(x) zend_resource *(x)
#define COMPAT_ZVAL_STRING(str, len) ZVAL_STRING(str, len)
#define COMPAT_RETURN_STRING(str) RETURN_STRING(str)
#define COMPAT_Z_TYPE_PP(x) Z_TYPE_P(x)
#define COMPAT_Z_TYPE_P(x) Z_TYPE(x)
#define COMPAT_Z_LVAL(x) Z_LVAL(x)
#define COMPAT_Z_STRVAL(x) Z_STRVAL(x)
#define COMPAT_Z_ARRVAL_P(x) Z_ARRVAL_P(x)
#define COMPAT_Z_ADDREF_P(x) Z_ADDREF(x)
#define COMPAT_Z_STRLEN(x) Z_STRLEN(x)
#define COMPAT_Z_STRLEN_P(x) ZSTR_LEN(x)
#define COMPAT_Z_STRLEN_PP(x) Z_STRLEN_P(x)
#define COMPAT_Z_STRVAL_P(x) ZSTR_VAL(x)
#define COMPAT_Z_ARREF(x) &(x)
#define COMPAT_Z_REFCOUNTED(zval) ((Z_TYPE_FLAGS(zval) & IS_TYPE_REFCOUNTED) != 0)
#define COMPAT_TRY_ADDREF_P(SRC) Z_TRY_ADDREF_P(SRC)

#define COMPAT_MAKE_STD_ZVAL(x)
#define COMPAT_DECLARE_ZVAL(name) zval name ## _v; zval * name = &name ## _v
#define COMPAT_ALLOC_INIT_ZVAL(name)
#define COMPAT_ALLOC_INIT_ZVAL2(name) ZVAL_NULL(name)
#define COMPAT_ZVAL_COPY(z, v) ZVAL_COPY(z, v)
#define hp_ptr_dtor(val) zval_ptr_dtor(val)

#define COMPAT_ADD_ASSOC_ZVAL(ARR, KEY, SRC) do {      \
  COMPAT_TRY_ADDREF_P(&SRC);                           \
  add_assoc_zval(COMPAT_Z_ARREF(ARR), KEY, &SRC);      \
} while(0)

#endif

// Aho-Corasick import
#include "multifast/ahocorasick.h"
#include "multifast/actypes.h"

ZEND_BEGIN_MODULE_GLOBALS(ahocorasick)
    long counter;
    zend_bool direction;
ZEND_END_MODULE_GLOBALS(ahocorasick)

#ifdef ZTS
#define AHOCORASICK_G(v) TSRMG(ahocorasick_globals_id, zend_ahocorasick_globals *, v)
#else
#define AHOCORASICK_G(v) (ahocorasick_globals.v)
#endif

#define PHP_AHOCORASICK_VERSION "0.0.4"
#define PHP_AHOCORASICK_EXTNAME "ahocorasick"

/**
 * Sample param to pass to callback handler for AhoCorasick search algorithm
 */
struct ahocorasick_callback_payload_t {
        // found match will be added here
        int retVal;
        COMPAT_ZVAL resultArray;
};

/**
 * AhoCorasick table initialization
 */
typedef struct ahocorasick_pattern_t {
    char *key;
    long keyId;
    enum ac_pattid_type keyType;
    COMPAT_ZVAL zKey;

    char *value;
    int valueLen;
    COMPAT_ZVAL zVal;

    int ignoreCase;
    COMPAT_ZVAL auxObj;

    struct ahocorasick_pattern_t * next;
    struct ahocorasick_pattern_t * prev;
} ahocorasick_pattern_t;

/**
 * Initialized Aho Corasick search structure - resource
 */
typedef struct ahocorasick_master_t {
    // aho corasick main search tree
    AC_TRIE_t * acap;
    // if structure was finalized, is set to 1
    unsigned char ac_finalized;
    // if initialization was ok, is set to 1.
    unsigned char init_ok;
    // root of the doubly linked list for search patterns
    ahocorasick_pattern_t * patterns;
    // size of the search pattern list
    long pattern_count;
} ahocorasick_master_t;

#define PHP_AHOSTRUCT_MASTER_RES_NAME "AhoCorasick search"
#define PHP_AHOSTRUCT_RES_NAME "Ahostruct element data"

PHP_MINIT_FUNCTION(ahocorasick);
PHP_MSHUTDOWN_FUNCTION(ahocorasick);
PHP_RINIT_FUNCTION(ahocorasick);
PHP_FUNCTION(ahocorasick_match);
PHP_FUNCTION(ahocorasick_init);
PHP_FUNCTION(ahocorasick_deinit);
PHP_FUNCTION(ahocorasick_isValid);
PHP_FUNCTION(ahocorasick_finalize);
PHP_FUNCTION(ahocorasick_add_patterns);
extern zend_module_entry ahocorasick_module_entry;
#define phpext_hello_ptr &ahocorasick_module_entry

#endif
