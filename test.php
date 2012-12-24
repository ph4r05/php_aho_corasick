<?php
echo "Going to init: \n";

$data = array(
		array('key'=>'ab', 'value'=>'alfa', 'ignoreCase'=>true),
		array('key'=>'ac', 'value'=>'beta', 'ignoreCase'=>true),
		array('key'=>'ad', 'value'=>'gamma', 'ignoreCase'=>true),
		array('key'=>'ae', 'value'=>'delta', 'ignoreCase'=>true),
		array('key'=>'af', 'value'=>'zeta', 'ignoreCase'=>true),
		array('key'=>'ag', 'value'=>'omega', 'ignoreCase'=>true),
		array('key'=>'ah', 'value'=>'lfa', 'ignoreCase'=>true)
	     );

$c = ahocorasick_init($data);
var_dump($c);

$d = ahocorasick_match("alFABETA gammadelta delta delta!", $c);
var_dump($d);

$d = ahocorasick_match("alFABETAABECEDAAAA!", $c);
var_dump($d);

$d = ahocorasick_match("alFABETAABECEDAAAA!", $c, false);
var_dump($d);

$d = ahocorasick_match("alFABETAABECEDAAAA!", $c, true);
var_dump($d);

var_dump(ahocorasick_isValid($c));
var_dump(ahocorasick_deinit($c));
var_dump($c);
if ($c){
	var_dump(ahocorasick_isValid($c));
	var_dump(ahocorasick_deinit($c));
}
