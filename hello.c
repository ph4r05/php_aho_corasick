/*
 * php_hello.h: PHP Aho Corasick extension file
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"
#include "php_ini.h"
#include "php_hello.h"
#include "ext/standard/php_string.h"
#include "ext/mbstring/libmbfl/mbfl/mbfl_allocators.h"
#include "php_variables.h"
#include "php_globals.h"
#include "rfc1867.h"
#include "php_content_types.h"
#include "SAPI.h"
//#include "php_unicode.h"
#include "TSRM.h"

#include "ahocorasick.h"

int le_hello_person;
int le_hello_person_persist;
int le_ahostruct_master;


ZEND_DECLARE_MODULE_GLOBALS(hello)

static zend_function_entry hello_functions[] = {
    PHP_FE(ahocorasick_match, NULL)
    PHP_FE(ahocorasick_init, NULL)
    PHP_FE(ahocorasick_deinit, NULL)
    PHP_FE(hello_lower, NULL)
    PHP_FE(hello_world, NULL)
    PHP_FE(hello_long, NULL)
    PHP_FE(hello_double, NULL)
    PHP_FE(hello_bool, NULL)
    PHP_FE(hello_null, NULL)
    PHP_FE(hello_greetme, NULL)
    PHP_FE(hello_add, NULL)
    PHP_FE(hello_dump, NULL)
    PHP_FE(hello_array, NULL)
    PHP_FE(hello_array_strings, NULL)
    PHP_FE(hello_array_walk, NULL)
    PHP_FE(hello_array_value, NULL)
    PHP_FE(hello_get_global_var, NULL)
    PHP_FE(hello_set_local_var, NULL)
    PHP_FE(hello_person_new, NULL)
    PHP_FE(hello_person_pnew, NULL)
    PHP_FE(hello_person_greet, NULL)
    PHP_FE(hello_person_delete, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry hello_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_HELLO_WORLD_EXTNAME,
    hello_functions,
    PHP_MINIT(hello),
    PHP_MSHUTDOWN(hello),
    PHP_RINIT(hello),
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_HELLO_WORLD_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_HELLO
    ZEND_GET_MODULE(hello)
#endif

PHP_INI_BEGIN()
    PHP_INI_ENTRY("hello.greeting", "Hello World", PHP_INI_ALL, NULL)
    STD_PHP_INI_ENTRY("hello.direction", "1", PHP_INI_ALL, OnUpdateBool, direction, zend_hello_globals, hello_globals)
PHP_INI_END()

static void php_hello_init_globals(zend_hello_globals *hello_globals)
{
    hello_globals->direction = 1;
}

PHP_RINIT_FUNCTION(hello)
{
    HELLO_G(counter) = 0;

    return SUCCESS;
}

static void php_hello_person_persist_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    php_hello_person *person = (php_hello_person*)rsrc->ptr;

    if (person) {
        if (person->name) {
            pefree(person->name, 1);
        }
        pefree(person, 1);
    }
}

/**
 * Destructor of person resource
 */
static void php_hello_person_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    php_hello_person *person = (php_hello_person*)rsrc->ptr;

    if (person) {
        if (person->name) {
            efree(person->name);
        }
        efree(person);
    }
}

/**
 * Destructor of ahostruct resource
 */
static void php_ahostruct_master_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    long i=0;
    ahoMasterStruct *aho = (ahoMasterStruct*)rsrc->ptr;
    if (aho) {
        // release automata here
        ac_automata_release(aho->acap);
        // now release strings kept inside aho structure
        for (i=0; i<aho->ahobufflen; i++){
            // at first release strings
            efree(aho->ahostructbuff[i]->key);
            efree(aho->ahostructbuff[i]->value);
            // now free element structure
            efree(aho->ahostructbuff[i]);
        }
        // release whole array
        efree(aho->ahostructbuff);
        // release holder structure
        efree(aho);
    }
}

PHP_MINIT_FUNCTION(hello)
{
    // destruction of ahostruct master
    le_ahostruct_master = zend_register_list_destructors_ex(php_ahostruct_master_dtor, NULL, PHP_AHOSTRUCT_MASTER_RES_NAME, module_number);    
    // person destruction
    le_hello_person = zend_register_list_destructors_ex(php_hello_person_dtor, NULL, PHP_HELLO_PERSON_RES_NAME, module_number);
    le_hello_person_persist = zend_register_list_destructors_ex (NULL, php_hello_person_persist_dtor, PHP_HELLO_PERSON_RES_NAME, module_number);

    ZEND_INIT_MODULE_GLOBALS(hello, php_hello_init_globals, NULL);
    REGISTER_INI_ENTRIES();

    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(hello)
{
    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}

PHP_FUNCTION(hello_world)
{
    RETURN_STRING("Hello World", 1);
}

PHP_FUNCTION(hello_long)
{
    if (HELLO_G(direction)) {
        HELLO_G(counter)++;
    } else {
        HELLO_G(counter)--;
    }

    RETURN_LONG(HELLO_G(counter));
}


/**
 * Invokes PHP function that converts string to lower case. 
 * Calling mb_strtolower didn't work properly because of unavailability 
 * of php_unicode.h, thus we are using call_user_function.
 * 
 * Calls PHP function mb_strtolower from user space
 */
char * mb_strtolower(char * input){
    zval ret, function_name, *params[1];
    
    // construct function to call
    ZVAL_STRING(&function_name, "mb_strtolower", 0);
    // construct parameter to pass to target function
    MAKE_STD_ZVAL(params[0]);
    ZVAL_STRING(params[0], input, 1);
    // invoke target function
    if (call_user_function(EG(function_table), NULL, &function_name, &ret, 1, params TSRMLS_CC) == SUCCESS){
        return Z_STRVAL(ret);
    } else {
        return NULL;
    }
}


struct sample_param {
        // found match will be added here
        int retVal;
        zval * resultArray;
};

/**
 * AhoCorasick callback handler - MATCH_CALBACK_t type
 */
int match_handler(AC_MATCH_t * m, void * param)
{
        // variable to hold sub array - one found result
        zval * mysubarray;
	unsigned int j;

	/* example of sending parameter to call-back function */
	struct sample_param * myp = (struct sample_param *)param;
        if (myp->resultArray==NULL){
            // invalid condition - result array not initialized
            return 0;
        }

	
	php_printf ("@ %ld : ", m->position);
	for (j=0; j < m->match_num; j++){
                // dump found matches to result array
                ALLOC_INIT_ZVAL(mysubarray);
                array_init(mysubarray);
                add_assoc_long(mysubarray, "pos", m->position);
                add_assoc_string(mysubarray, "key", m->patterns[j].rep.stringy, 1);
                // add to aggregate array
                add_next_index_zval(myp->resultArray, mysubarray);  
                // just simple dump
		php_printf("%s (%s), ",
				m->patterns[j].rep.stringy,
				m->patterns[j].astring);
        }

	php_printf("\n");
        // return 1 if we want to find just first
        // to find all return 0
        return myp->retVal == 0 ? 0 : 1;
}

/**
 * Testable function for lowercase - calls mb_strtolower function from PHP userspace
 * @param 
 * @return 
 */
PHP_FUNCTION(hello_lower)
{
    char *lowered;
    zval *uservar;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &uservar) == FAILURE) {
        RETURN_NULL();
    }
    
    lowered = mb_strtolower(Z_STRVAL_P(uservar));
    RETURN_STRING(lowered, 1);
}

/**
 * Basic strtolower matcher.
 * Returns array of keys matched (strings).
 * 
 * @param 
 * @return 
 */
PHP_FUNCTION(ahocorasick_match)
{
    char *lowered, *normal;
    zval *uservar, *ahostruct, *findAll;
    ahoMasterStruct * ahoMaster;
    AC_TEXT_t tmp_text;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|b", &uservar, &ahostruct, &findAll) == FAILURE) {
        RETURN_NULL();
    }
    
    // fetch resource passed as parameter
    ZEND_FETCH_RESOURCE(ahoMaster, ahoMasterStruct*, &ahostruct, -1, PHP_AHOSTRUCT_MASTER_RES_NAME, le_ahostruct_master);
    if (ahoMaster==NULL){
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid resource.");
        RETURN_FALSE;
    }
    
    // at first, obtain also lower case variant
    normal = Z_STRVAL_P(uservar);
    lowered = mb_strtolower(Z_STRVAL_P(uservar));
    
    php_printf("Hello mr: %d \n", ahoMaster->test);
    php_printf("normal input: %s lowered: %s \n", normal, lowered);
    
    
    //*** Reset automata at first - clean position
    /** if you want to do another search with same automata
      * you must reset automata.
      */
    ac_automata_reset(ahoMaster->acap);
    
    //*** 6. Set input text
    tmp_text.astring = lowered;
    tmp_text.length = strlen(tmp_text.astring);

    /* Sending parameter to call-back function */
    // initialize return array
    array_init(return_value);
    struct sample_param my_param;
    my_param.retVal = 0;
    my_param.resultArray = return_value;
    
    //*** 7. Do search
    ac_automata_search(ahoMaster->acap, &tmp_text, (void *)(&my_param));
    /* here we pass 0 to our callback function.
     * if there are variables to pass to call-back function,
     * you can define a struct that enclose those variables and
     * send the pointer of the struct as a parameter.
    **/
}

/**
 * De-initializes AhoCorasick master resource
 * 
 * @param 
 * @return 
 */
PHP_FUNCTION(ahocorasick_deinit)
{
    zval *ahostruct;
    ahoMasterStruct * ahoMaster;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &ahostruct) == FAILURE) {
        RETURN_NULL();
    }
    
    // fetch resource passed as parameter
    ZEND_FETCH_RESOURCE(ahoMaster, ahoMasterStruct*, &ahostruct, -1, PHP_AHOSTRUCT_MASTER_RES_NAME, le_ahostruct_master);
    // already empty?
    if (ahoMaster==NULL){
        RETURN_FALSE;
    }
    
    // delete now
    zend_list_delete(Z_LVAL_P(ahostruct));
    RETURN_TRUE;
}

/**
 * Initializes AhoCorasick search structure with passed data
 * @param 
 * @return 
 */
PHP_FUNCTION(ahocorasick_init)
{
    char *lowered, *normal;
    zval *arr;
    zval **data;
    HashTable *arr_hash;
    HashPosition pointer;
    int array_count;
    int curIdx = 0;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &arr) == FAILURE) {
        RETURN_NULL();
    }
    
    arr_hash = Z_ARRVAL_P(arr);
    array_count = zend_hash_num_elements(arr_hash);
    // initialize buffer array
    ahostruct ** ahostructbuff = (ahostruct ** ) emalloc(sizeof(*ahostructbuff) * array_count);
    // iterate input initialized array
    for(zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); 
            zend_hash_get_current_data_ex(arr_hash, (void**) &data, &pointer) == SUCCESS; 
            zend_hash_move_forward_ex(arr_hash, &pointer), curIdx++) {

        // check structure
        if (Z_TYPE_PP(data) != IS_ARRAY) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid structure! Cannot initialize.");
            RETURN_FALSE;
        }
        
        // now we know that element is another array - iterate over it again and gain needed info
        ahostruct * tmpStruct = (ahostruct *) emalloc(sizeof(ahostruct));
        ahostructbuff[curIdx] = tmpStruct;
        tmpStruct->ignoreCase=0;
        tmpStruct->key=NULL;
        tmpStruct->value=NULL;
        
        // iterate over sub array
        zval **data_sub;
        HashTable *arr_hash_sub = Z_ARRVAL_P(*data);
        HashPosition pointer_sub;
        for (zend_hash_internal_pointer_reset_ex(arr_hash_sub, &pointer_sub); 
                zend_hash_get_current_data_ex(arr_hash_sub, (void**) &data_sub, &pointer_sub) == SUCCESS; 
                zend_hash_move_forward_ex(arr_hash_sub, &pointer_sub)) {
            zval temp;
            char *key;
            int key_len;
            long index;
            // flags of found keys
            unsigned char keyFound=0;

            // obtain array key
            if (zend_hash_get_current_key_ex(arr_hash_sub, &key, &key_len, &index, 0, &pointer_sub) == HASH_KEY_IS_STRING) {
                // key is correct - string
            } else {
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid structure (bad sub-array key)! Cannot initialize.");
                RETURN_FALSE;
            }
            
            // determine known keys
            temp = **data_sub;
            if (strcasecmp("key", key)==0){
                keyFound|=1;
            } else if (strcasecmp("value", key)==0){
                keyFound|=2;
            } else if (strcasecmp("ignoreCase", key)==0){
                keyFound|=4;
            } else {
                php_error_docref(NULL TSRMLS_CC, E_WARNING, 
                        "Invalid structure (bad sub-array key: [%s])! "
                        "Only allowed are: {key, value, ignoreCase}. Cannot initialize.", key);
                RETURN_FALSE;
            }
            
            // if boolean value -> process
            if (keyFound==4){
                // convert to boolean
                int bool = Z_BVAL(temp);
                tmpStruct->ignoreCase = bool;
            }
            
            // string value -> process
            if ((keyFound & 0x3) > 0){
                char * stmp=NULL;
                // copy string
                stmp = estrndup(Z_STRVAL(temp), Z_STRLEN(temp));
                if (keyFound==1){
                    // key
                    tmpStruct->key = stmp;
                } else {
                    // value
                    tmpStruct->value = stmp;
                    tmpStruct->valueLen = Z_STRLEN(temp);
                }
            }
        }
        
        // sanity check, if failed, return false
        if (tmpStruct->key==NULL || tmpStruct->value==NULL){
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid structure, struct not complete! ");
                RETURN_FALSE;
        }
    }
    
    //
    // now is everything OK (input data parsed properly) -> initialize AHO automata
    //
    unsigned int i;

    //*** 2. Define AC variables: AC_AUTOMATA_t *, AC_PATTERN_t, and AC_TEXT_t
    AC_AUTOMATA_t * acap;
    AC_PATTERN_t tmp_patt;
    AC_TEXT_t tmp_text;

    //*** 3. Get a new automata
    acap = ac_automata_init (match_handler);

    //*** 4. add patterns to automata
    for (i=0; i<array_count; i++){
        // search string
        tmp_patt.astring = ahostructbuff[i]->value;
        tmp_patt.length = ahostructbuff[i]->valueLen;
        // representative string (key)
        tmp_patt.rep.number = i+1; // optional
        tmp_patt.rep.stringy = ahostructbuff[i]->key;
        // add this pattern to automata
        ac_automata_add (acap, &tmp_patt);
    }

    //*** 5. Finalize automata (no more patterns will be added).
    ac_automata_finalize (acap);
    
    // create resource in holder structure, fill with data, return
    ahoMasterStruct * ahomaster = emalloc(sizeof(ahoMasterStruct));
    // just for testing
    ahomaster->test=3;
    // pass ACAP object - holding aho automaton
    ahomaster->acap = acap;
    // now store pointers to allocated strings in memory - for aho struct in memory
    ahomaster->ahostructbuff = ahostructbuff;
    ahomaster->ahobufflen = array_count;
    // register this resource for ZEND engine
    ZEND_REGISTER_RESOURCE(return_value, ahomaster, le_ahostruct_master);
        
    // ahostruct build OK.
    // Keep in mind that we are not freeing strings allocated in memory, it is 
    // still used internally in aho structure, this free is postponed to releasing
    // aho structure.
}

PHP_FUNCTION(hello_double)
{
    RETURN_DOUBLE(3.1415926535);
}

PHP_FUNCTION(hello_bool)
{
    RETURN_BOOL(1);
}

PHP_FUNCTION(hello_null)
{
    RETURN_NULL();
}

PHP_FUNCTION(hello_greetme)
{
    zval *zname;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zname) == FAILURE) {
        RETURN_NULL();
    }

    convert_to_string(zname);

    php_printf("Hello ");
    PHPWRITE(Z_STRVAL_P(zname), Z_STRLEN_P(zname));
    php_printf("\n");

    RETURN_TRUE;
}

PHP_FUNCTION(hello_add)
{
    long a;
    double b;
    zend_bool return_long = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ld|b", &a, &b, &return_long) == FAILURE) {
        RETURN_NULL();
    }

    if (return_long) {
        RETURN_LONG(a + b);
    } else {
        RETURN_DOUBLE(a + b);
    }
}

PHP_FUNCTION(hello_dump)
{
    zval *uservar;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &uservar) == FAILURE) {
    RETURN_NULL();
    }

    switch (Z_TYPE_P(uservar)) {
    case IS_NULL:
        php_printf("NULL\n");
        break;
    case IS_BOOL:
        php_printf("Boolean: %s\n", Z_LVAL_P(uservar) ? "TRUE" : "FALSE");
        break;
    case IS_LONG:
        php_printf("Long: %ld\n", Z_LVAL_P(uservar));
        break;
    case IS_DOUBLE:
        php_printf("Double: %f\n", Z_DVAL_P(uservar));
        break;
    case IS_STRING:
        php_printf("String: ");
        PHPWRITE(Z_STRVAL_P(uservar), Z_STRLEN_P(uservar));
        php_printf("\n");
        break;
    case IS_RESOURCE:
        php_printf("Resource\n");
        break;
    case IS_ARRAY:
        php_printf("Array\n");
        break;
    case IS_OBJECT:
        php_printf("Object\n");
        break;
    default:
        php_printf("Unknown\n");
    }

    RETURN_TRUE;
}

PHP_FUNCTION(hello_array)
{
    char *mystr;
    zval *mysubarray;

    array_init(return_value);

    add_index_long(return_value, 42, 123);
    add_next_index_string(return_value, "I should now be found at index 43", 1);
    add_next_index_stringl(return_value, "I'm at 44!", 10, 1);
    mystr = estrdup("Forty Five");
    add_next_index_string(return_value, mystr, 0);
    add_assoc_double(return_value, "pi", 3.1415926535);

    ALLOC_INIT_ZVAL(mysubarray);
    array_init(mysubarray);

    add_next_index_string(mysubarray, "hello", 1);
    php_printf("mysubarray->refcount = %d\n", mysubarray->refcount__gc);
    mysubarray->refcount__gc = 2;
    php_printf("mysubarray->refcount = %d\n", mysubarray->refcount__gc);
    add_assoc_zval(return_value, "subarray", mysubarray);
    php_printf("mysubarray->refcount = %d\n", mysubarray->refcount__gc);
}

PHP_FUNCTION(hello_array_strings)
{
    zval *arr, **data;
    HashTable *arr_hash;
    HashPosition pointer;
    int array_count;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &arr) == FAILURE) {
        RETURN_NULL();
    }

    arr_hash = Z_ARRVAL_P(arr);
    array_count = zend_hash_num_elements(arr_hash);

    php_printf("The array passed contains %d elements\n", array_count);

    for(zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); zend_hash_get_current_data_ex(arr_hash, (void**) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(arr_hash, &pointer)) {

        zval temp;
        char *key;
        int key_len;
        long index;

        if (zend_hash_get_current_key_ex(arr_hash, &key, &key_len, &index, 0, &pointer) == HASH_KEY_IS_STRING) {
            PHPWRITE(key, key_len);
        } else {
            php_printf("%ld", index);
        }

        php_printf(" => ");

        temp = **data;
        zval_copy_ctor(&temp);
        convert_to_string(&temp);
        PHPWRITE(Z_STRVAL(temp), Z_STRLEN(temp));
        php_printf("\n");
        zval_dtor(&temp);
    }

    RETURN_TRUE;
}

static int php_hello_array_walk(zval **element TSRMLS_DC)
{
    zval temp;
    temp = **element;
    zval_copy_ctor(&temp);
    convert_to_string(&temp);
    PHPWRITE(Z_STRVAL(temp), Z_STRLEN(temp));
    php_printf("\n");
    zval_dtor(&temp);

    return ZEND_HASH_APPLY_KEEP;
}

static int php_hello_array_walk_arg(zval **element, char *greeting TSRMLS_DC)
{
    php_printf("%s", greeting);
    php_hello_array_walk(element TSRMLS_CC);

    return ZEND_HASH_APPLY_KEEP;
}

static int php_hello_array_walk_args(zval **element, int num_args, va_list args, zend_hash_key *hash_key)
{
    char *prefix = va_arg(args, char*);
    char *suffix = va_arg(args, char*);
    TSRMLS_FETCH();

    php_printf("%s", prefix);
    php_hello_array_walk(element TSRMLS_CC);
    php_printf("%s\n", suffix);

    return ZEND_HASH_APPLY_KEEP;
}

PHP_FUNCTION(hello_array_walk)
{
    zval *zarray;
    int print_newline = 1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &zarray) == FAILURE) {
        RETURN_NULL();
    }

    zend_hash_apply(Z_ARRVAL_P(zarray), (apply_func_t)php_hello_array_walk TSRMLS_CC);
    zend_hash_internal_pointer_reset(Z_ARRVAL_P(zarray));
    zend_hash_apply_with_argument(Z_ARRVAL_P(zarray), (apply_func_arg_t)php_hello_array_walk_arg, "Hello " TSRMLS_CC);
    zend_hash_apply_with_arguments(Z_ARRVAL_P(zarray), (apply_func_args_t)php_hello_array_walk_args, 2, "Hello ", "Welcome to my extension!");

    RETURN_TRUE;
}

PHP_FUNCTION(hello_array_value)
{
    zval *zarray, *zoffset, **zvalue;
    long index = 0;
    char *key = NULL;
    int key_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "az", &zarray, &zoffset) == FAILURE) {
        RETURN_NULL();
    }

    switch (Z_TYPE_P(zoffset)) {
    case IS_NULL:
        index = 0;
        break;
    case IS_DOUBLE:
        index = (long)Z_DVAL_P(zoffset);
        break;
    case IS_BOOL:
    case IS_LONG:
    case IS_RESOURCE:
        index = Z_LVAL_P(zoffset);
        break;
    case IS_STRING:
        key = Z_STRVAL_P(zoffset);
        key_len = Z_STRLEN_P(zoffset);
        break;
    case IS_ARRAY:
        key = "Array";
        key_len = sizeof("Array") - 1;
        break;
    case IS_OBJECT:
        key = "Object";
        key_len = sizeof("Object") - 1;
        break;
    default:
        key = "Unknown";
        key_len = sizeof("Unknown") - 1;
    }

    if (key && zend_hash_find(Z_ARRVAL_P(zarray), key, key_len + 1, (void**)&zvalue) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Undefined index: %s", key);
        RETURN_NULL();
    } else if (!key && zend_hash_index_find(Z_ARRVAL_P(zarray), index, (void**)&zvalue) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Undefined index: %ld", index);
        RETURN_NULL();
    }

    *return_value = **zvalue;
    zval_copy_ctor(return_value);
}

PHP_FUNCTION(hello_get_global_var)
{
    char *varname;
    int varname_len;
    zval **varvalue;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &varname, &varname_len) == FAILURE) {
        RETURN_NULL();
    }

    if (zend_hash_find(&EG(symbol_table), varname, varname_len + 1, (void**)&varvalue) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Undefined variable: %s", varname);
        RETURN_NULL();
    }

    *return_value = **varvalue;
    zval_copy_ctor(return_value);
}

PHP_FUNCTION(hello_set_local_var)
{
    zval *newvar;
    char *varname;
    int varname_len;
    zval *value;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &varname, &varname_len, &value) == FAILURE) {
        RETURN_NULL();
    }

    ALLOC_INIT_ZVAL(newvar);
    *newvar = *value;
    zval_copy_ctor(newvar);

    zend_hash_add(EG(active_symbol_table), varname, varname_len + 1, &newvar, sizeof(zval*), NULL);

    RETURN_TRUE;
}

PHP_FUNCTION(hello_person_new)
{
    php_hello_person *person;
    char *name;
    int name_len;
    long age;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &name, &name_len, &age) == FAILURE) {
        RETURN_FALSE;
    }

    if (name_len < 1) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "No name given, person resource not created.");
        RETURN_FALSE;
    }

    if (age < 0 || age > 255) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Nonsense age (%d) given, person resource not created.", age);
        RETURN_FALSE;
    }

    person = emalloc(sizeof(php_hello_person));
    person->name = estrndup(name, name_len);
    person->name_len = name_len;
    person->age = age;

    ZEND_REGISTER_RESOURCE(return_value, person, le_hello_person);
}

PHP_FUNCTION(hello_person_pnew)
{
    php_hello_person *person;
    char *name, *key;
    int name_len, key_len;
    long age;
    zend_rsrc_list_entry *le, new_le;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &name, &name_len, &age) == FAILURE) {
        RETURN_FALSE;
    }

    if (name_len < 1) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "No name given, person resource not created.");
        RETURN_FALSE;
    }

    if (age < 0 || age > 255) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Nonsense age (%d) given, person resource not created.", age);
        RETURN_FALSE;
    }

    /* Look for an established resource */
    key_len = spprintf(&key, 0, "hello_person_%s_%d", name, age);
    if (zend_hash_find(&EG(persistent_list), key, key_len + 1, (void**)&le) == SUCCESS) {
        /* An entry for this person already exists */
        ZEND_REGISTER_RESOURCE(return_value, le->ptr, le_hello_person_persist);
        efree(key);
        return;
    }

    /* New person, allocate a structure */
    person = pemalloc(sizeof(php_hello_person), 1);
    person->name = pemalloc(name_len + 1, 1);
    memcpy(person->name, name, name_len + 1);
    person->name_len = name_len;
    person->age = age;

    ZEND_REGISTER_RESOURCE(return_value, person, le_hello_person_persist);

    /* Store a reference in the persistence list */
    new_le.ptr = person;
    new_le.type = le_hello_person_persist;
    zend_hash_add(&EG(persistent_list), key, key_len + 1, &new_le, sizeof(zend_rsrc_list_entry), NULL);

    efree(key);
}

PHP_FUNCTION(hello_person_greet)
{
    php_hello_person *person;
    zval *zperson;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zperson) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE2(person, php_hello_person*, &zperson, -1, PHP_HELLO_PERSON_RES_NAME, le_hello_person, le_hello_person_persist);

    php_printf("Hello ");
    PHPWRITE(person->name, person->name_len);
    php_printf("!According to my records, you are %d years old.", person->age);

    RETURN_TRUE;
}

PHP_FUNCTION(hello_person_delete)
{
    zval *zperson;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zperson) == FAILURE) {
        RETURN_FALSE;
    }

    zend_list_delete(Z_LVAL_P(zperson));
    RETURN_TRUE;
}
