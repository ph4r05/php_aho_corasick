php_aho_corasick
================
[![Build Status](https://travis-ci.org/ph4r05/php_aho_corasick.svg?branch=master)](https://travis-ci.org/ph4r05/php_aho_corasick)

PHP extension implementing Aho-Corasick algorithm (see http://en.wikipedia.org/wiki/Aho%E2%80%93Corasick_string_matching_algorithm ).

Is especially effective if there is a large database of needles (=strings to be searched, for example virus signatures). 
Another advantage is that built search structure is initialized before search in separate call thus it can be called
more times with different haystack, saving time.

Effectivity of this algorithm is also supported by speed of PHP Extensions (compared to pure php implementation).

Dependencies
=============
This project is simple PHP wrapper of (or interface to) another project:
MultiFast: http://sourceforge.net/projects/multifast/?source=dlp

Source of inspiration for this project was a great tutorial:
http://devzone.zend.com/446/extension-writing-part-iii-resources/

Aho-Corasick C library is wrapped as PHP extension loadable to PHP.

Build
=====
```bash
phpize
./configure --enable-ahocorasick
make
```

Usage
=====
This extension is case sensitive, thus if you want case insensitive, convert every string input to this algorithm to lowercase (use mb_strtolower() for example).

If you want to detect equality, compare also length of needle and haystack (for example).

test.php:
```php
$data = array(
  	array('key'=>'ab', 'value'=>'alfa', 'ignoreCase'=>true),
		array('key'=>'ac', 'value'=>'beta', 'ignoreCase'=>true),
		array('key'=>'ad', 'value'=>'gamma', 'ignoreCase'=>true),
		array('key'=>'ae', 'value'=>'delta', 'ignoreCase'=>true),
		array('key'=>'af', 'value'=>'zeta', 'ignoreCase'=>true),
		array('key'=>'ag', 'value'=>'omega', 'ignoreCase'=>true),
		array('key'=>'ah', 'value'=>'lfa', 'ignoreCase'=>true)
	     );
// initialize search , returns resourceID for search structure
$c = ahocorasick_init($data);
// perform search 1
$d1 = ahocorasick_match("alFABETA gammadelta delta delta!", $c);
// perform search 2
$d2 = ahocorasick_match("alfa zeta omegaomegalfa", $c);
// deinitialize search structure (will free memory)
ahocorasick_deinit($c);

var_dump($d1);
var_dump($d2);
```

Call with:
```bash
php -d extension=modules/ahocorasick.so -f test.php
```

Results with:
```
array(4) {
  [0]=>
  array(4) {
    ["pos"]=>
    int(14)
    ["keyIdx"]=>
    int(30536592)
    ["key"]=>
    string(2) "ad"
    ["value"]=>
    string(5) "gamma"
  }
  [1]=>
  array(4) {
    ["pos"]=>
    int(19)
    ["keyIdx"]=>
    int(30536696)
    ["key"]=>
    string(2) "ae"
    ["value"]=>
    string(5) "delta"
  }
  [2]=>
  array(4) {
    ["pos"]=>
    int(25)
    ["keyIdx"]=>
    int(30536696)
    ["key"]=>
    string(2) "ae"
    ["value"]=>
    string(5) "delta"
  }
  [3]=>
  array(4) {
    ["pos"]=>
    int(31)
    ["keyIdx"]=>
    int(30536696)
    ["key"]=>
    string(2) "ae"
    ["value"]=>
    string(5) "delta"
  }
}
array(7) {
  [0]=>
  array(4) {
    ["pos"]=>
    int(4)
    ["keyIdx"]=>
    int(30536384)
    ["key"]=>
    string(2) "ab"
    ["value"]=>
    string(4) "alfa"
  }
  [1]=>
  array(4) {
    ["pos"]=>
    int(4)
    ["keyIdx"]=>
    int(30537008)
    ["key"]=>
    string(2) "ah"
    ["value"]=>
    string(3) "lfa"
  }
  [2]=>
  array(4) {
    ["pos"]=>
    int(9)
    ["keyIdx"]=>
    int(30536800)
    ["key"]=>
    string(2) "af"
    ["value"]=>
    string(4) "zeta"
  }
  [3]=>
  array(4) {
    ["pos"]=>
    int(15)
    ["keyIdx"]=>
    int(30536904)
    ["key"]=>
    string(2) "ag"
    ["value"]=>
    string(5) "omega"
  }
  [4]=>
  array(4) {
    ["pos"]=>
    int(20)
    ["keyIdx"]=>
    int(30536904)
    ["key"]=>
    string(2) "ag"
    ["value"]=>
    string(5) "omega"
  }
  [5]=>
  array(4) {
    ["pos"]=>
    int(23)
    ["keyIdx"]=>
    int(30536384)
    ["key"]=>
    string(2) "ab"
    ["value"]=>
    string(4) "alfa"
  }
  [6]=>
  array(4) {
    ["pos"]=>
    int(23)
    ["keyIdx"]=>
    int(30537008)
    ["key"]=>
    string(2) "ah"
    ["value"]=>
    string(3) "lfa"
  }
}
```

Benchmark
==========
In this repo you can find benchmark.php file, with this you can perform your own benchmark and measure speed up.

My setup generates random haystacks and needles from alphabet="abcdef". There is performed 5 measurements of time spent by search and average is computed.
Search structure construction is conted to time measurements.

Script generates:
  * 256 random haystacks of size 2048 characters
  * 2048 needles with 16 characters.

Principle:
  * Naive approach simply iterates over haystacks and needles, search is performed with strpos().
  * Aho-Corasick approach constructs search structure, then all haystacks are searched for needles.

Results:
```
Classic search; sampleCount: 5; keySize: 2048; time: 6.479004
AhoCorasick search; sampleCount: 5; keySize: 2048; time: 0.107578
```

Speedup: 60.2X compared to naive approach.

