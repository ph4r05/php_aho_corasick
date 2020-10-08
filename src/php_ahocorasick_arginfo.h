/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: ba403304a9d28e8231b959ba328e0b9ef4ae92c2 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ahocorasick_match, 0, 2, MAY_BE_FALSE|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, needle, IS_STRING, 0)
	ZEND_ARG_INFO(0, id)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, findAll, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ahocorasick_init, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ahocorasick_deinit, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, id)
ZEND_END_ARG_INFO()

#define arginfo_ahocorasick_isValid arginfo_ahocorasick_deinit

#define arginfo_ahocorasick_finalize arginfo_ahocorasick_deinit

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ahocorasick_add_patterns, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, id)
	ZEND_ARG_TYPE_INFO(0, patterns, IS_ARRAY, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(ahocorasick_match);
ZEND_FUNCTION(ahocorasick_init);
ZEND_FUNCTION(ahocorasick_deinit);
ZEND_FUNCTION(ahocorasick_isValid);
ZEND_FUNCTION(ahocorasick_finalize);
ZEND_FUNCTION(ahocorasick_add_patterns);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(ahocorasick_match, arginfo_ahocorasick_match)
	ZEND_FE(ahocorasick_init, arginfo_ahocorasick_init)
	ZEND_FE(ahocorasick_deinit, arginfo_ahocorasick_deinit)
	ZEND_FE(ahocorasick_isValid, arginfo_ahocorasick_isValid)
	ZEND_FE(ahocorasick_finalize, arginfo_ahocorasick_finalize)
	ZEND_FE(ahocorasick_add_patterns, arginfo_ahocorasick_add_patterns)
	ZEND_FE_END
};
