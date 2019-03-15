--TEST--
Test 2
--SKIPIF--
<?php if (!extension_loaded("ahocorasick")) print "skip"; ?>
--FILE--
<?php
ini_set("xdebug.var_display_max_depth", 10);
ini_set("xdebug.var_display_max_children", 1000);
ini_set("xdebug.var_display_max_data", 1000);

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

if ($c){
	echo "AhoCorasick isValid(c): ";
	var_dump(ahocorasick_isValid($c));

	echo "AhoCorasick deinit(c): ";
	var_dump(ahocorasick_deinit($c));
}
?>
--EXPECT--

AhoCorasick match for ahocorasick_match("alFABETA gammadelta delta delta simple pie! aux ssščř+ééžž ččř é é-é éeéee éé aux2 aux3 aux2", c): array(16) {
  [0]=>
  array(5) {
    ["pos"]=>
    int(14)
    ["key"]=>
    string(2) "ad"
    ["aux"]=>
    int(66)
    ["start_postion"]=>
    int(9)
    ["value"]=>
    string(5) "gamma"
  }
  [1]=>
  array(5) {
    ["pos"]=>
    int(19)
    ["key"]=>
    string(2) "ae"
    ["aux"]=>
    string(10) "simple-aux"
    ["start_postion"]=>
    int(14)
    ["value"]=>
    string(5) "delta"
  }
  [2]=>
  array(5) {
    ["pos"]=>
    int(25)
    ["key"]=>
    string(2) "ae"
    ["aux"]=>
    string(10) "simple-aux"
    ["start_postion"]=>
    int(20)
    ["value"]=>
    string(5) "delta"
  }
  [3]=>
  array(5) {
    ["pos"]=>
    int(31)
    ["key"]=>
    string(2) "ae"
    ["aux"]=>
    string(10) "simple-aux"
    ["start_postion"]=>
    int(26)
    ["value"]=>
    string(5) "delta"
  }
  [4]=>
  array(3) {
    ["pos"]=>
    int(38)
    ["start_postion"]=>
    int(32)
    ["value"]=>
    string(6) "simple"
  }
  [5]=>
  array(4) {
    ["pos"]=>
    int(42)
    ["keyIdx"]=>
    int(42)
    ["start_postion"]=>
    int(39)
    ["value"]=>
    string(3) "pie"
  }
  [6]=>
  array(4) {
    ["pos"]=>
    int(47)
    ["aux"]=>
    array(1) {
      [0]=>
      array(2) {
        [0]=>
        string(14) "helloAuxObject"
        [1]=>
        int(41)
      }
    }
    ["start_postion"]=>
    int(44)
    ["value"]=>
    string(3) "aux"
  }
  [7]=>
  array(3) {
    ["pos"]=>
    int(61)
    ["start_postion"]=>
    int(50)
    ["value"]=>
    string(11) "ščř+éé"
  }
  [8]=>
  array(3) {
    ["pos"]=>
    int(61)
    ["start_postion"]=>
    int(57)
    ["value"]=>
    string(4) "éé"
  }
  [9]=>
  array(3) {
    ["pos"]=>
    int(94)
    ["start_postion"]=>
    int(90)
    ["value"]=>
    string(4) "éé"
  }
  [10]=>
  array(4) {
    ["pos"]=>
    int(98)
    ["aux"]=>
    array(1) {
      [0]=>
      array(2) {
        [0]=>
        string(14) "helloAuxObject"
        [1]=>
        int(41)
      }
    }
    ["start_postion"]=>
    int(95)
    ["value"]=>
    string(3) "aux"
  }
  [11]=>
  array(4) {
    ["pos"]=>
    int(99)
    ["aux"]=>
    int(66)
    ["start_postion"]=>
    int(95)
    ["value"]=>
    string(4) "aux2"
  }
  [12]=>
  array(4) {
    ["pos"]=>
    int(103)
    ["aux"]=>
    array(1) {
      [0]=>
      array(2) {
        [0]=>
        string(14) "helloAuxObject"
        [1]=>
        int(41)
      }
    }
    ["start_postion"]=>
    int(100)
    ["value"]=>
    string(3) "aux"
  }
  [13]=>
  array(4) {
    ["pos"]=>
    int(104)
    ["aux"]=>
    array(1) {
      [0]=>
      array(2) {
        [0]=>
        string(14) "helloAuxObject"
        [1]=>
        int(41)
      }
    }
    ["start_postion"]=>
    int(100)
    ["value"]=>
    string(4) "aux3"
  }
  [14]=>
  array(4) {
    ["pos"]=>
    int(108)
    ["aux"]=>
    array(1) {
      [0]=>
      array(2) {
        [0]=>
        string(14) "helloAuxObject"
        [1]=>
        int(41)
      }
    }
    ["start_postion"]=>
    int(105)
    ["value"]=>
    string(3) "aux"
  }
  [15]=>
  array(4) {
    ["pos"]=>
    int(109)
    ["aux"]=>
    int(66)
    ["start_postion"]=>
    int(105)
    ["value"]=>
    string(4) "aux2"
  }
}
AhoCorasick match for ahocorasick_match("alFABETAABECEDAAAA!", c): array(0) {
}
AhoCorasick match for ahocorasick_match("alFABETAABECEDAAAA!", c, false): array(0) {
}
AhoCorasick match for ahocorasick_match("alFABETAABECEDAAAA!", c, true): array(0) {
}
AhoCorasick isValid(c): bool(true)
AhoCorasick deinit(c): bool(true)
AhoCorasick isValid(c): bool(false)
AhoCorasick deinit(c): bool(false)
