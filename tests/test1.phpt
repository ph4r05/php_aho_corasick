--TEST--
Test 1
--SKIPIF--
<?php if (!extension_loaded("ahocorasick")) print "skip"; ?>
--FILE--
<?php
ini_set("xdebug.var_display_max_depth", 10);
ini_set("xdebug.var_display_max_children", 1000);
ini_set("xdebug.var_display_max_data", 1000);

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
unset($data);

// perform search 1
$d1 = ahocorasick_match("alFABETA gamma zetaomegaalfa!", $c);
// deinitialize search structure (will free memory)
ahocorasick_deinit($c);

var_dump($d1);

if (count($d1) != 5){
 throw new Exception("Expected 5 results");
}

$ex = ["pos"=>28, "start_postion"=>25, "value"=>"lfa"];
if ($d1[4] != $ex){
 throw new Exception("Expected");
}

// UTF8 check
$check_word=[
    ['value'=>'你好'],
    ['value'=>'hi'],
    ['value'=>'谢谢'],
    ['value'=>'thanks']
];

$text = "你好，hi，谢谢，thanks";

$c = ahocorasick_init($check_word);

$res = ahocorasick_match($text, $c);
var_dump($res);

ahocorasick_deinit($c);

echo "OK\n";
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
array(4) {
  [0]=>
  array(3) {
    ["pos"]=>
    int(6)
    ["start_postion"]=>
    int(0)
    ["value"]=>
    string(6) "你好"
  }
  [1]=>
  array(3) {
    ["pos"]=>
    int(11)
    ["start_postion"]=>
    int(9)
    ["value"]=>
    string(2) "hi"
  }
  [2]=>
  array(3) {
    ["pos"]=>
    int(20)
    ["start_postion"]=>
    int(14)
    ["value"]=>
    string(6) "谢谢"
  }
  [3]=>
  array(3) {
    ["pos"]=>
    int(29)
    ["start_postion"]=>
    int(23)
    ["value"]=>
    string(6) "thanks"
  }
}
OK
