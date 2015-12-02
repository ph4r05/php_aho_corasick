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

/**
 * Sources:
 *  http://www.phpinternalsbook.com/zvals/memory_management.html
 *  http://docstore.mik.ua/orelly/webprog/php/ch14_06.htm
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
#include "actypes.h"

// counter for aho struct resources
int le_ahostruct_master;

ZEND_DECLARE_MODULE_GLOBALS(ahocorasick)

static zend_function_entry ahocorasick_functions[] = {
    PHP_FE(ahocorasick_match, NULL)
    PHP_FE(ahocorasick_init, NULL)
    PHP_FE(ahocorasick_deinit, NULL)
    PHP_FE(ahocorasick_isValid, NULL)
    PHP_FE(ahocorasick_finalize, NULL)
    PHP_FE(ahocorasick_add_patterns, NULL)
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

/**
 * Finalizes searching trie if it was not finalized.
 */
static inline int php_ac_finalize(ahoMasterStruct * ahoMaster){
    if (ahoMaster == NULL
        || ahoMaster->init_ok != 1
        || ahoMaster->ac_finalized == 1)
    {
        return 0;
    }

    ahoMaster->ac_finalized = 1;
    //*** 5. Finalize automata (no more patterns will be added).
    ac_trie_finalize (ahoMaster->acap);
    return 1;
}

/**
 * Resets all pattern fields. Does not perform any deallocation.
 */
static inline int php_ac_reset_pattern(ahostruct * tmpStruct){
    if (tmpStruct == NULL){
        return -1;
    }

    tmpStruct->ignoreCase=0;
    tmpStruct->key=NULL;
    tmpStruct->zKey=NULL;
    tmpStruct->keyId=0;
    tmpStruct->keyType=AC_PATTID_TYPE_DEFAULT;
    tmpStruct->value=NULL;
    tmpStruct->zVal=NULL;
    tmpStruct->auxObj=NULL;
    return 0;
}

/**
 * Deallocates all memory related to the given pattern.
 */
static inline int php_ac_dealloc_pattern(ahostruct * tmpStruct){
    if (tmpStruct == NULL){
        return -1;
    }

    if (tmpStruct->auxObj != NULL){
        zval_ptr_dtor(&(tmpStruct->auxObj));
        tmpStruct->auxObj = NULL;
    }

    if (tmpStruct->key != NULL && tmpStruct->zKey != NULL) {
        zval_ptr_dtor(&(tmpStruct->zKey));
        tmpStruct->key = NULL;
        tmpStruct->zKey = NULL;
    }

    if (tmpStruct->value != NULL && tmpStruct->zVal != NULL) {
        zval_ptr_dtor(&(tmpStruct->zVal));
        tmpStruct->value = NULL;
        tmpStruct->zVal = NULL;
    }

    php_ac_reset_pattern(tmpStruct);
    return 0;
}

/**
 * Reads single pattern definition, construct ahostruct representation of pattern.
 */
static inline int php_ac_process_pattern(ahostruct * tmpStruct, HashTable * arr_hash_sub TSRMLS_DC) {
    php_ac_reset_pattern(tmpStruct);

    // iterate over sub array
    int returnCode = 0;
    unsigned long allKeys = 0;
    zval **data_sub;
    HashPosition pointer_sub;
    for (zend_hash_internal_pointer_reset_ex(arr_hash_sub, &pointer_sub);
         zend_hash_get_current_data_ex(arr_hash_sub, (void**) &data_sub, &pointer_sub) == SUCCESS;
         zend_hash_move_forward_ex(arr_hash_sub, &pointer_sub))
    {
        char *key;
        unsigned int key_len;
        unsigned long index;
        unsigned long keyFound = 0;

        // obtain array key
        if (zend_hash_get_current_key_ex(arr_hash_sub, &key, &key_len, &index, 0, &pointer_sub) != HASH_KEY_IS_STRING) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid structure (bad sub-array key)! Cannot initialize.");
            returnCode = -1;
            break;
        }

        // determine known keys
        if (strcasecmp("key", key)==0){
            keyFound|=1;
        } else if (strcasecmp("value", key)==0){
            keyFound|=2;
        } else if (strcasecmp("ignoreCase", key)==0){
            keyFound|=4;
        } else if (strcasecmp("id", key)==0){
            keyFound|=8;
        } else if (strcasecmp("aux", key)==0){
            keyFound|=0x10;
        } else {
            php_error_docref(NULL TSRMLS_CC, E_WARNING,
                    "Invalid structure (unrecognized sub-array key: [%s])! "
                            "Only allowed are: {key, id, value, aux, ignoreCase}. Cannot initialize.", key);
            returnCode = -2;
            break;
        }
        allKeys |= keyFound;

        // Numeric identifier
        if ((keyFound & 0x8) > 0){
            long keyId = Z_LVAL(**data_sub);
            tmpStruct->keyId = keyId;
            tmpStruct->keyType = AC_PATTID_TYPE_NUMBER;
        }

        // Aux object
        if ((keyFound & 0x10) > 0){
            // No copying using same reference.
            tmpStruct->auxObj = *data_sub;
            Z_ADDREF_P(*data_sub);
        }

        // ignoreCase - deprecated.
        if ((keyFound & 0x4) > 0){
            // convert to boolean
            int tmpBool = Z_BVAL(**data_sub);
            tmpStruct->ignoreCase = tmpBool;
        }

        // key/value present -> process
        if ((keyFound & 0x3) > 0){
            char * stmp = NULL;
            // Avoid string copy, use reference counting.
            stmp = Z_STRVAL(**data_sub);
            Z_ADDREF_P(*data_sub);
            if (keyFound == 0x1){
                // key
                tmpStruct->zKey = *data_sub;
                tmpStruct->key = stmp;
                tmpStruct->keyType = AC_PATTID_TYPE_STRING;
            } else if (keyFound == 0x2){
                // value
                tmpStruct->zVal = *data_sub;
                tmpStruct->value = stmp;
                tmpStruct->valueLen = Z_STRLEN(**data_sub);
            }
        }
    }

    // If everything went well, we can return successfully.
    if (returnCode == 0){
        return 0;
    }

    // Otherwise deallocate this entry.
    php_ac_dealloc_pattern(tmpStruct);
    return returnCode;
}

/**
 * Adds the given list to the pattern list
 */
static inline int php_ac_add_patterns(ahoMasterStruct * master, ahostruct * tmpStruct, ahostruct * tmpStructLast, long sublistSize){
    if (master == NULL || tmpStruct == NULL){
        return -1;
    }

    tmpStruct->prev = NULL;
    tmpStructLast->next = master->ahostructbuff;

    if (master->ahostructbuff){
        master->ahostructbuff->prev = tmpStructLast;
    }
    master->ahostructbuff = tmpStruct;
    master->ahobufflen += sublistSize;
    return 0;
}

/**
 * Adds given pattern to the doubly linked list. Does not copy memory, embbeds given structure directly to the list.
 */
static inline int php_ac_add_pattern(ahoMasterStruct * master, ahostruct * tmpStruct){
    return php_ac_add_patterns(master, tmpStruct, tmpStruct, 1);
}

/**
 * Releases all associated memory in linked list of patterns
 */
static inline int php_ac_release_patterns(ahoMasterStruct * master){
    if (master == NULL){
        return -1;
    }

    ahostruct * p0 = master->ahostructbuff;
    while(p0){
        ahostruct * next = p0->next;
        php_ac_dealloc_pattern(p0);
        efree(p0);
        p0 = next;
    }

    master->ahostructbuff = NULL;
    master->ahobufflen = 0;

    return 0;
}

/**
 * Reads array of patterns, adds them to the search trie.
 */
static inline int php_ac_process_patterns(ahoMasterStruct * master, HashTable * arr_hash TSRMLS_DC){
    int pattern_processing_status = 0;
    zval *arr;
    zval **data;
    HashPosition pointer;
    int array_count;
    int curIdx = 0;

    ahostruct * p0 = NULL;
    ahostruct * p1 = NULL;
    ahostruct * prevPattern = NULL;
    ahostruct * lastPattern = NULL;
    array_count = zend_hash_num_elements(arr_hash);
    // initialize buffer array
    //ahostruct ** ahostructbuff = (ahostruct ** ) emalloc(sizeof(*ahostructbuff) * array_count);
    //memset(ahostructbuff, 0, sizeof(*ahostructbuff) * array_count);

    // iterate input initialized array
    for(zend_hash_internal_pointer_reset_ex(arr_hash, &pointer);
        zend_hash_get_current_data_ex(arr_hash, (void**) &data, &pointer) == SUCCESS;
        zend_hash_move_forward_ex(arr_hash, &pointer), curIdx++) {

        // check structure
        if (Z_TYPE_PP(data) != IS_ARRAY) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid pattern structure! Cannot initialize.");
            pattern_processing_status = -4;
            break;
        }

        // now we know that element is another array - iterate over it again and gain needed info
        ahostruct * tmpStruct = (ahostruct *) emalloc(sizeof(ahostruct));
        if (curIdx == 0){
            lastPattern = tmpStruct;
        }

        // Construct as a doubly linked list.
        tmpStruct->prev = NULL;
        tmpStruct->next = prevPattern;
        if (prevPattern){
            prevPattern->prev = tmpStruct;
        }
        prevPattern = tmpStruct;

        // iterate over sub array
        unsigned long allKeys = 0;
        zval **data_sub;
        HashTable *arr_hash_sub = Z_ARRVAL_P(*data);
        int status_code = php_ac_process_pattern(tmpStruct, arr_hash_sub TSRMLS_CC);

        if (status_code != 0){
            pattern_processing_status = -1;
            break;
        }

        // sanity check, if failed, return false
        if (tmpStruct->value==NULL){
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "No value was specified for pattern!");
            pattern_processing_status = -2;
            break;
        }

        // numeric key and string identifier are mutually exclusive
        if ((allKeys & 0x1) && (allKeys & 0x8)){
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Pattern can have either numeric or string identifier, not both!");
            pattern_processing_status = -3;
            break;
        }
    }

    // if processing failed, free memory.
    if (pattern_processing_status != 0){
        p0 = prevPattern;
        while(p0){
            p1 = p0->next;
            php_ac_dealloc_pattern(p0);
            efree(p0);
            p0 = p1;
        }

        return pattern_processing_status;
    }

    // Nothing to process.
    if (prevPattern == NULL){
        return 0;
    }

    //
    // now is everything OK (input data parsed properly) -> initialize AHO automata
    //

    // Add all patterns at once to the internal data structures
    php_ac_add_patterns(master, prevPattern, lastPattern, curIdx);

    p0 = prevPattern;
    while(p0){
        AC_PATTERN_t tmp_patt;
        p1 = p0->next;

        // Construct search pattern for AhoCorasick library.
        // search string
        tmp_patt.ptext.astring = p0->value;
        tmp_patt.ptext.length = p0->valueLen;

        //The replacement pattern is not applicable in this program, so better
        //to initialize it with 0/
        tmp_patt.rtext.astring = NULL;
        tmp_patt.rtext.length = 0;

        // search value key
        tmp_patt.id.type = p0->keyType;
        if (p0->keyType == AC_PATTID_TYPE_NUMBER){
            tmp_patt.id.u.number = p0->keyId;
        } else if (p0->keyType == AC_PATTID_TYPE_STRING) {
            tmp_patt.id.u.stringy = p0->key;
        }

        // Aux object holds the whole pattern in our representation.
        tmp_patt.aux = (void*)p0;

        // add this pattern to trie. copy pattern to internal memory.
        ac_trie_add(master->acap, &tmp_patt, 1);
        p0 = p1;
    }

    return pattern_processing_status;
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
    if (aho == NULL){
        return;
    }

    if (aho->ahostructbuff != NULL) {
        // release automata here
        if (aho->acap != NULL) {
            ac_trie_release(aho->acap);
        }

        php_ac_release_patterns(aho);
    }

    // release holder structure
    efree(aho);
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

#ifdef AHOCORASICK_USE_LOWER
/**
 * Invokes PHP function that converts string to lower case. 
 * Calling mb_strtolower didn't work properly because of unavailability 
 * of php_unicode.h, thus we are using call_user_function.
 * 
 * Calls PHP function mb_strtolower from user space
 */
char * mb_strtolower(char * input TSRMLS_CC){
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
#endif

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

    for (j = 0; j < m->size; j++) {
        // dump found matches to result array
        ALLOC_INIT_ZVAL(mysubarray);
        array_init(mysubarray);
        add_assoc_long(mysubarray, "pos", m->position);

        ahostruct * curPattern = (ahostruct *) m->patterns[j].aux;
        if (curPattern == NULL){
            continue;
        }

        if (m->patterns[j].id.type == AC_PATTID_TYPE_STRING){
            add_assoc_string(mysubarray, "key", (char*)m->patterns[j].id.u.stringy, 1);

        } else if (m->patterns[j].id.type == AC_PATTID_TYPE_NUMBER){
            add_assoc_long(mysubarray, "keyIdx", m->patterns[j].id.u.number);

        }

        if (curPattern->auxObj != NULL){
            add_assoc_zval(mysubarray, "aux", curPattern->auxObj);
            Z_ADDREF_P(curPattern->auxObj);
        }

        add_assoc_string(mysubarray, "value", (char*)m->patterns[j].ptext.astring, 1);

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
    if (ahoMaster==NULL || ahoMaster->init_ok != 1){
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

    if (ahoMaster->init_ok != 1){
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Not initialized.");
        RETURN_FALSE;
    }

    // finalize trie if not finalized already
    php_ac_finalize(ahoMaster);

    normal = Z_STRVAL_P(uservar);
#ifdef AHOCORASICK_USE_LOWER
    // at first, obtain also lower case variant
    // strtolower is disabled now, exact match is required
    lowered = mb_strtolower(Z_STRVAL_P(uservar) TSRMLS_CC);
    tmp_text.astring = lowered;
#else
    //*** 6. Set input text
    tmp_text.astring = normal;
#endif
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
    ac_trie_search(ahoMaster->acap, &tmp_text, 0, match_handler, (void *)(&my_param));
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
    php_ac_finalize(ahoMaster);
    zend_list_delete(Z_LVAL_P(ahostruct));
    RETURN_TRUE;
}

/**
 * Initializes AhoCorasick search structure with passed data
 * @param 
 * @return
 * TODO: add option to add multiple patterns later, after init. finalize individually or on first matching call.
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

    // Get matching patterns
    arr_hash = Z_ARRVAL_P(arr);

    // create resource in holder structure, fill with data, return
    ahoMasterStruct * ahomaster = emalloc(sizeof(ahoMasterStruct));
    ahomaster->acap = ac_trie_create();
    ahomaster->ac_finalized = 0;
    ahomaster->init_ok = 0;
    ahomaster->ahostructbuff = NULL;
    ahomaster->ahobufflen = 0;

    int pattern_processing_status = php_ac_process_patterns(ahomaster, arr_hash TSRMLS_CC);
    if (pattern_processing_status != 0){
        php_ac_release_patterns(ahomaster);
        ac_trie_release(ahomaster->acap);
        efree(ahomaster);
        RETURN_FALSE;
    }

    // pass ACAP object - holding aho automaton
    ahomaster->init_ok = 1;
    // register this resource for ZEND engine
    ZEND_REGISTER_RESOURCE(return_value, ahomaster, le_ahostruct_master);
        
    // ahostruct build OK.
    // Keep in mind that we are not freeing strings allocated in memory, it is 
    // still used internally in aho structure, this free is postponed to releasing
    // aho structure.
}

/**
 * Finalizes aho corasick search structure
 * @param
 * @return
 */
PHP_FUNCTION(ahocorasick_finalize)
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
        if (php_ac_finalize(ahoMaster)) {
            RETURN_TRUE;
        } else {
            RETURN_FALSE;
        }
    }
}

/**
 * Adds search patterns to the non-finalized search trie.
 * TODO: implement
 * @param
 * @return
 */
PHP_FUNCTION(ahocorasick_add_patterns)
{
    zval *ahostruct;
    zval *arr;
    ahoMasterStruct * ahoMaster;
    HashTable *arr_hash;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "za", &ahostruct, &arr) == FAILURE) {
        RETURN_NULL();
    }

    arr_hash = Z_ARRVAL_P(arr);

    // fetch resource passed as parameter
    ahoMaster = (ahoMasterStruct*) zend_fetch_resource(&ahostruct TSRMLS_CC, -1, NULL, NULL, 1, le_ahostruct_master);
    if (ahoMaster == NULL || ahoMaster->init_ok != 1){
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot add a new pattern, not initialized");
        RETURN_FALSE;
    }

    if (ahoMaster->ac_finalized){
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot add a new pattern to finalized search structure");
        RETURN_FALSE;
    }

    int pattern_processing_status = php_ac_process_patterns(ahoMaster, arr_hash TSRMLS_CC);
    if (pattern_processing_status != 0){
        RETURN_FALSE;
    } else {
        RETURN_TRUE;
    }
}