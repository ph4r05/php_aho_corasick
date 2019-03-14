--TEST--
Test 3
--SKIPIF--
<?php if (!extension_loaded("ahocorasick")) print "skip"; ?>
--FILE--
<?php

// initialize search , returns resourceID for search structure
$c = ahocorasick_init(array());

ahocorasick_add_patterns($c, array(array('key'=>'ab', 'value'=>'alfa')));
ahocorasick_add_patterns($c, array(array('key'=>'ac', 'value'=>'beta')));
ahocorasick_add_patterns($c, array(array('key'=>'ad', 'value'=>'gamma', 'aux'=>array(1))));
ahocorasick_add_patterns($c, array(array('key'=>'ae', 'value'=>'delta')));
ahocorasick_add_patterns($c, array(array('id'=>0, 'value'=>'zeta'),
                                   		array('key'=>'ag', 'value'=>'omega'),
                                   		array('value'=>'lfa')));


// perform search 1
$d1 = ahocorasick_match("alFABETA gamma zetaomegaalfa!", $c);
//unset($d1);

// deinitialize search structure (will free memory)
ahocorasick_deinit($c);

var_dump($d1);
?>
--EXPECT--

array(5) {
  [0]=>
  array(5) {
    ["pos"]=>
    int(14)
    ["key"]=>
    string(2) "ad"
    ["aux"]=>
    array(1) {
      [0]=>
      int(1)
    }
    ["start_postion"]=>
    int(9)
    ["value"]=>
    string(5) "gamma"
  }
  [1]=>
  array(4) {
    ["pos"]=>
    int(19)
    ["keyIdx"]=>
    int(0)
    ["start_postion"]=>
    int(15)
    ["value"]=>
    string(4) "zeta"
  }
  [2]=>
  array(4) {
    ["pos"]=>
    int(24)
    ["key"]=>
    string(2) "ag"
    ["start_postion"]=>
    int(19)
    ["value"]=>
    string(5) "omega"
  }
  [3]=>
  array(4) {
    ["pos"]=>
    int(28)
    ["key"]=>
    string(2) "ab"
    ["start_postion"]=>
    int(24)
    ["value"]=>
    string(4) "alfa"
  }
  [4]=>
  array(3) {
    ["pos"]=>
    int(28)
    ["start_postion"]=>
    int(25)
    ["value"]=>
    string(3) "lfa"
  }
}
