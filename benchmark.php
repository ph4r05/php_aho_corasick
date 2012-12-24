<?php

/**
 * Generates random word from given alphabet
 * @param type $alphabet
 * @param type $length
 */
function genRandomWord($alphabet, $length){
    $alen = strlen($alphabet);
    $out = "";
    
    for($i=0; $i<$length; $i++){
        $out .= $alphabet[rand(0, $alen-1)];
    }
    
    return $out;
}

// generate random data
$sampleCount = 5;
$haystackSize = 256;
$keySize = 2048;
$randomBuffers = array();
$randomKeys = array();
for($i=0; $i < $keySize; $i++){
    $randomKeys[$i] = genRandomWord("abcdef", 16);
}

for($i=0; $i < $haystackSize; $i++){
    $randomBuffers[$i] = genRandomWord("abcdef", 2048);
}


// do classical strpos search
$overalTime=array();
$sum = 0;
for($j = 0; $j < $sampleCount; $j++){
    $curTime = microtime(true);
    foreach($randomBuffers as $randomBuffer){
        for($i=0; $i < $keySize; $i++){
            $f = strpos($randomBuffer, $randomKeys[$i]);
        }
    }
    
    $curTime = microtime(true) - $curTime;
    $sum += $curTime;
}
// average?
printf("Classic search; sampleCount: %d; keySize: %d; time: %f\n\n", $sampleCount, $keySize, $sum/((float)$sampleCount));


// do advanced search - aho corasick
$overalTime=array();
$sum = 0;
for($j = 0; $j < $sampleCount; $j++){
    $curTime = microtime(true);
    // init aho structure
    
    $data = array();    
    for($i=0; $i < $keySize; $i++){
        $data[] = array('key'=>"".$i, 'value'=>$randomKeys[$i], 'ignoreCase'=>true);
    }
    
    $c = ahocorasick_init($data);
    foreach($randomBuffers as $randomBuffer){
        $d = ahocorasick_match($randomBuffer, $c);
    }
    ahocorasick_deinit($c);
    
    $curTime = microtime(true) - $curTime;
    $sum += $curTime;
}
// average?
printf("AhoCorasick search; sampleCount: %d; keySize: %d; time: %f\n\n", $sampleCount, $keySize, $sum/((float)$sampleCount));



