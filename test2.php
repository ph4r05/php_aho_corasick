<?php
$data = array(
  	array('key'=>'ab', 'value'=>'alfa', 'ignoreCase'=>true),
		array('key'=>'ac', 'value'=>'beta', 'ignoreCase'=>true),
		array('key'=>'ad', 'value'=>'gamma', 'ignoreCase'=>true),
		array('key'=>'ae', 'value'=>'delta', 'ignoreCase'=>true),
		array('key'=>'af', 'value'=>'zeta', 'ignoreCase'=>true),
		array('key'=>'ag', 'value'=>'omega', 'ignoreCase'=>true),
		array('key'=>'ah', 'value'=>'lfa', 'ignoreCase'=>true)
	     );
// initialize search structure
$c = ahocorasick_init($data);
// perform search 1
$d1 = ahocorasick_match("alFABETA gammadelta delta delta!", $c);
// perform search 2
$d2 = ahocorasick_match("alfa zeta omegaomegalfa", $c);
// deinitialize search structure (will free memory)
ahocorasick_deinit($c);

var_dump($d1);
var_dump($d2);
