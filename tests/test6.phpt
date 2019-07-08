--TEST--
Test 6
--SKIPIF--
<?php if (!extension_loaded("ahocorasick")) print "skip"; ?>
--FILE--
<?php
ini_set("xdebug.var_display_max_depth", 10);
ini_set("xdebug.var_display_max_children", 1000);
ini_set("xdebug.var_display_max_data", 1000);


$data = [
    ['key' => 'a', 'value' => 'abcd'],
    ['key' => 'b', 'value' => 'ghij'],
    ['key' => 'c', 'value' => 'defg'],
    ['key' => 'd', 'value' => 'defghijkl']
];

$c = ahocorasick_init($data);

$firstText = "abcde";
$secondText = "fghij";
$thirdText = "klmno";

$firstResult = ahocorasick_match($firstText, $c);
print("-----\n");
$secondResult = ahocorasick_match($secondText, $c);
print("-----\n");
$thirdResult = ahocorasick_match($thirdText, $c);
print("-----\n");

ahocorasick_deinit($c);

var_dump($firstResult);
print "\n";
var_dump($secondResult);
print "\n";
var_dump($thirdResult);
?>
--EXPECT--
-----
-----
-----
array(1) {
  [0]=>
  array(4) {
    ["pos"]=>
    int(4)
    ["key"]=>
    string(1) "a"
    ["start_postion"]=>
    int(0)
    ["value"]=>
    string(4) "abcd"
  }
}

array(1) {
  [0]=>
  array(4) {
    ["pos"]=>
    int(5)
    ["key"]=>
    string(1) "b"
    ["start_postion"]=>
    int(1)
    ["value"]=>
    string(4) "ghij"
  }
}

array(0) {
}
