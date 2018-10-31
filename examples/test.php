<?php
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
    ['你好'],['hi'],
    ['谢谢'],['thanks']
];

$text = "你好，hi，谢谢，thanks";

$c = ahocorasick_init($check_word);

$res = ahocorasick_match($text, $c);
var_dump($res);

ahocorasick_deinit($c);

echo "OK\n";
