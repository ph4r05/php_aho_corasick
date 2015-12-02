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
  	array('key'=>'ab', 'value'=>'alfa'),
		array('key'=>'ac', 'value'=>'beta'),
		array('key'=>'ad', 'value'=>'gamma', 'aux'=>array(1)),
		array('key'=>'ae', 'value'=>'delta'),
		array('id'=>0, 'value'=>'zeta'),
		array('key'=>'ag', 'value'=>'omega'),
		array('value'=>'lfa')
	     );
// initialize search , returns resourceID for search structure
$c = ahocorasick_init($data);
// perform search 1
$d1 = ahocorasick_match("alFABETA gamma zetaomegaalfa!", $c);
// deinitialize search structure (will free memory)
ahocorasick_deinit($c);

var_dump($d1);
```

Call with:
```bash
php -d extension=modules/ahocorasick.so -f test.php
```

Results with:
```
array(5) {
  [0]=>
  array(4) {
    ["pos"]=>
    int(14)
    ["key"]=>
    string(2) "ad"
    ["aux"]=>
    array(1) {
      [0]=>
      int(1)
    }
    ["value"]=>
    string(5) "gamma"
  }
  [1]=>
  array(3) {
    ["pos"]=>
    int(19)
    ["keyIdx"]=>
    int(0)
    ["value"]=>
    string(4) "zeta"
  }
  [2]=>
  array(3) {
    ["pos"]=>
    int(24)
    ["key"]=>
    string(2) "ag"
    ["value"]=>
    string(5) "omega"
  }
  [3]=>
  array(3) {
    ["pos"]=>
    int(28)
    ["key"]=>
    string(2) "ab"
    ["value"]=>
    string(4) "alfa"
  }
  [4]=>
  array(2) {
    ["pos"]=>
    int(28)
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
  * 256 random haystacks of size 8192 characters
  * 2048 needles with 16 characters.

Principle:
  * Naive approach simply iterates over haystacks and needles, search is performed with strpos().
  * Aho-Corasick approach constructs search structure, then all haystacks are searched for needles.

Results:
```
Classic search; sampleCount: 10; keySize: 2048; timeAvg: 13.060877
AhoCorasick search; sampleCount: 10; keySize: 2048; timeAvg: 0.174326 s, totalTime: 1.743264 s, memory increase: 272 B
AhoCorasick pattern matching is 74.921962 times faster than naive approach
```

Speedup: 74x compared to the naive approach.

