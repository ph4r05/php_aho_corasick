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
