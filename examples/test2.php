<?php
echo "Going to init: \n";
$aux1 = array(array("helloAuxObject", 41));
$aux2 = 0x42;
$aux3 = "simple-aux";

$data = array(
		array('key'=>'ab', 'value'=>'alfa'),
		array('key'=>'ac', 'value'=>'beta'),
		array('key'=>'ad', 'value'=>'gamma', 'aux'=>$aux2),
		array('key'=>'ae', 'value'=>'delta', 'aux'=>$aux3),
		array('key'=>'af', 'value'=>'zeta'),
		array('key'=>'ag', 'value'=>'omega'),
		array('key'=>'ah', 'value'=>'lfa'),
		array('id'=>42, 'value'=>'pie'),
		array('value'=>'simple'),
		array('value'=>'aux', 'aux'=>$aux1),
		array('value'=>'aux2', 'aux'=>$aux2),
		array('value'=>'aux3', 'aux'=>$aux1),
		array('value'=>'ščř+éé'),
		array('value'=>'éé'),
);

$c = ahocorasick_init($data);
$data = array(); // Memoty test.
echo "AhoCorasick struct: ";
var_dump($c);

$str = "alFABETA gammadelta delta delta simple pie! aux ssščř+ééžž ččř é é-é éeéee éé aux2 aux3 aux2";
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
