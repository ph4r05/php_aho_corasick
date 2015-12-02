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
$sampleCount = 10;
$haystackSize = 256;
$keySize = 2048;
$randomBuffers = array();
$randomKeys = array();
for($i=0; $i < $keySize; $i++){
    $randomKeys[$i] = genRandomWord("abcdef", 16);
}

for($i=0; $i < $haystackSize; $i++){
    $randomBuffers[$i] = genRandomWord("abcdef", 8192);
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
$avgNaive = $sum/((float)$sampleCount);
printf("Classic search; sampleCount: %d; keySize: %d; timeAvg: %f\n\n", $sampleCount, $keySize, $avgNaive);

// do advanced search - aho corasick
$memStart = memory_get_usage();
$overalTime=array();
$sum = 0;
for($j = 0; $j < $sampleCount; $j++){
    $curTime = microtime(true);
    // init aho structure
    
    $data = array();    
    for($i=0; $i < $keySize; $i++){
        $data[] = array('id'=>$i, 'value'=>$randomKeys[$i], 'aux' => $randomBuffers);
    }
    
    $c = ahocorasick_init($data);
    foreach($randomBuffers as $randomBuffer){
        $d = ahocorasick_match($randomBuffer, $c);
    }

    ahocorasick_deinit($c);
    
    $curTime = microtime(true) - $curTime;
    $sum += $curTime;

    unset($data);
    unset($d);
}

$memStop = memory_get_usage();
$avgAho = $sum/((float)$sampleCount);

printf("AhoCorasick search; sampleCount: %d; keySize: %d; timeAvg: %f s, totalTime: %f s, memory increase: %d B\n\n",
    $sampleCount, $keySize, $avgAho, $sum, $memStop-$memStart);

printf("AhoCorasick pattern matching is %f times faster than naive approach\n", $avgNaive/$avgAho);

