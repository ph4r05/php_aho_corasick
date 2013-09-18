/*
 * php_ahocorasick.c: PHP Aho Corasick extension file
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"
#include "php_ini.h"
#include "php_ahocorasick.h"
#include "ext/standard/php_string.h"
#include "php_variables.h"
#include "php_globals.h"
#include "TSRM.h"
#include "ahocorasick.h"

// counter for aho struct resources
int le_ahostruct_master;

ZEND_DECLARE_MODULE_GLOBALS(ahocorasick)

static zend_function_entry ahocorasick_functions[] = {
    PHP_FE(ahocorasick_match, NULL)
    PHP_FE(ahocorasick_init, NULL)
    PHP_FE(ahocorasick_deinit, NULL)
    PHP_FE(ahocorasick_isValid, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry ahocorasick_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_AHOCORASICK_EXTNAME,
    ahocorasick_functions,
    PHP_MINIT(ahocorasick),
    PHP_MSHUTDOWN(ahocorasick),
    PHP_RINIT(ahocorasick),
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_AHOCORASICK_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_AHOCORASICK
    ZEND_GET_MODULE(ahocorasick)
#endif

//PHP_INI_BEGIN()
    //PHP_INI_ENTRY("ahocorasick.greeting", "Hello World", PHP_INI_ALL, NULL)
    //STD_PHP_INI_ENTRY("helloahocorasick.direction", "1", PHP_INI_ALL, OnUpdateBool, direction, zend_ahocorasick_globals, ahocorasick_globals)
//PHP_INI_END()    
    
/**
 * register some global variables here
 * @param ahocorasick_globals
 */
static void php_ahocorasick_init_globals(zend_ahocorasick_globals *ahocorasick_globals)
{
        return;
}
    
PHP_RINIT_FUNCTION(ahocorasick)
{
    return SUCCESS;
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

PHP_MINIT_FUNCTION(ahocorasick)
{
    // destruction of ahostruct master
    le_ahostruct_master = zend_register_list_destructors_ex(php_ahostruct_master_dtor, NULL, PHP_AHOSTRUCT_MASTER_RES_NAME, module_number);    
    
    //ZEND_INIT_MODULE_GLOBALS(ahocorasick, php_ahocorasick_init_globals, NULL);
    //REGISTER_INI_ENTRIES();
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(ahocorasick)
{
    //UNREGISTER_INI_ENTRIES();
    return SUCCESS;
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

/**
 * AhoCorasick callback handler - MATCH_CALBACK_t type
 */
int match_handler(AC_MATCH_t * m, void * param)
{
    // variable to hold sub array - one found result
    zval * mysubarray;
    unsigned int j;

    /* example of sending parameter to call-back function */
    struct aho_callback_payload * myp = (struct aho_callback_payload *) param;
    if (myp->resultArray == NULL) {
        // invalid condition - result array not initialized
        return 0;
    }

    for (j = 0; j < m->match_num; j++) {
        // dump found matches to result array
        ALLOC_INIT_ZVAL(mysubarray);
        array_init(mysubarray);
        add_assoc_long(mysubarray, "pos", m->position);
        add_assoc_long(mysubarray, "keyIdx", m->patterns[j].rep.number);
        add_assoc_string(mysubarray, "key", m->patterns[j].rep.stringy, 1);
        add_assoc_string(mysubarray, "value", m->patterns[j].astring, 1);
        // add to aggregate array
        add_next_index_zval(myp->resultArray, mysubarray);
    }

    // return 1 if we want to find just first
    // to find all return 0
    return myp->retVal == 0 ? 0 : 1;
}

/**
 * Returns whether current AhoCorasick resource is valid
 * @param 
 * @return 
 */
PHP_FUNCTION(ahocorasick_isValid)
{
    zval *ahostruct;
    ahoMasterStruct * ahoMaster;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &ahostruct) == FAILURE) {
        RETURN_NULL();
    }
    
    // fetch resource passed as parameter
    ahoMaster = (ahoMasterStruct*) zend_fetch_resource(&ahostruct TSRMLS_CC, -1, NULL, NULL, 1, le_ahostruct_master);
    if (ahoMaster==NULL){        
        RETURN_FALSE;
    } else {
        RETURN_TRUE;
    }
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
    zval *uservar, *ahostruct;
    zend_bool findAll = 1;
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
    // strtolower is disabled now, exact match is required
    //lowered = mb_strtolower(Z_STRVAL_P(uservar));    
    
    //*** Reset automata at first - clean position
    /** if you want to do another search with same automata
      * you must reset automata.
      */
    ac_automata_reset(ahoMaster->acap);
    
    //*** 6. Set input text
    //tmp_text.astring = lowered;
    tmp_text.astring = normal;
    tmp_text.length = Z_STRLEN_P(uservar);

    /* Sending parameter to call-back function */
    // initialize return array
    array_init(return_value);
    struct aho_callback_payload my_param;
    my_param.retVal = 0;
    my_param.resultArray = return_value;
    
    // find all defined
    my_param.retVal = findAll ? 0:1;
    
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
        ac_automata_add(acap, &tmp_patt);
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
