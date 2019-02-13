<?php

$s = "aoeu a5 a5 a5 a5 aoeu";
$data = array( array('value'=>'a5'));


for($out = 0; $out < 20; ++$out){
    printf("\nT $out\n");
    $c = ahocorasick_init($data);

    for ($i = 0; $i < 1000; ++$i) {
      $d = ahocorasick_match($s, $c);
      if (!$d || sizeof($d) != 4)
        throw new Exception('Unexpected result!');
      printf('.');
    }
    ahocorasick_deinit($c);
    $c = 0;
}
printf("\n");
