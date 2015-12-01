PHP_ARG_ENABLE(ahocorasick, whether to enable AhoCorasick support, [ --enable-ahocorasick   Enable Aho Corasick support])
if test "$PHP_AHOCORASICK" = "yes"; then
  AC_DEFINE(HAVE_AHOCORASICK, 1, [Whether you have Aho Corasick])
  PHP_NEW_EXTENSION(ahocorasick, node.c ahocorasick.c php_ahocorasick.c, $ext_shared)
fi
