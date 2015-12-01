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
echo "AhoCorasick struct: ";
var_dump($c);

$str = "alFABETA gammadelta delta delta!";
$d = ahocorasick_match($str, $c);
echo "AhoCorasick match for ahocorasick_match(\"$str\", c): ";
var_dump($d);

$str = "alFABETAABECEDAAAA!";
$d = ahocorasick_match($str, $c);
echo "AhoCorasick match for ahocorasick_match(\"$str\", c): ";
var_dump($d);

$str = "alFABETAABECEDAAAA!";
$d = ahocorasick_match($str, $c, false);
echo "AhoCorasick match for ahocorasick_match(\"$str\", c, false): ";
var_dump($d);

$str = "alFABETAABECEDAAAA!";
$d = ahocorasick_match($str, $c, true);
echo "AhoCorasick match for ahocorasick_match(\"$str\", c, true): ";
var_dump($d);

echo "AhoCorasick isValid(c): ";
var_dump(ahocorasick_isValid($c));

echo "AhoCorasick deinit(c): ";
var_dump(ahocorasick_deinit($c));

echo "AhoCorasick struct: ";
var_dump($c);

if ($c){
	echo "AhoCorasick isValid(c): ";
	var_dump(ahocorasick_isValid($c));

	echo "AhoCorasick deinit(c): ";
	var_dump(ahocorasick_deinit($c));
}
