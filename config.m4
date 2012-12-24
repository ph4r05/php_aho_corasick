PHP_ARG_ENABLE(hello, whether to enable Hello World support, [ --enable-hello   Enable Hello World support])
if test "$PHP_HELLO" = "yes"; then
  AC_DEFINE(HAVE_HELLO, 1, [Whether you have Hello World])
  PHP_NEW_EXTENSION(hello, node.c ahocorasick.c hello.c, $ext_shared)
fi

PHP_ARG_ENABLE(ahocorasick, whether to enable AhoCorasick support, [ --enable-ahocorasick   Enable Aho Corasick support])
if test "$PHP_AHOCORASICK" = "yes"; then
  AC_DEFINE(HAVE_AHOCORASICK, 1, [Whether you have Aho Corasick])
  PHP_NEW_EXTENSION(ahocorasick, node.c ahocorasick.c php_ahocorasick.c, $ext_shared)
fi
