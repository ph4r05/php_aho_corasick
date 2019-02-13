<?php

$s = "aoeu a5 a5 a5 a5 aoeu";
$data = array( array('value'=>'a5'));

$c = ahocorasick_init($data);

for ($i = 0; $i < 5000; ++$i) {
  $d = ahocorasick_match($s, $c);
  printf("$i %s\n", $d);
}
