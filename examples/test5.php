<?php

$data = [
    ["key" => "熊本県熊本市北区四方寄町", "value" => "北区四方寄町"],
    ["key" => "熊本県熊本市北区立福寺町", "value" => "北区立福寺町"]
];

$list = [
    "東京都東京都",
    "兵庫県兵庫県",
    "奈良県奈良県",
    "兵庫県兵庫県",
    "兵庫県兵庫県",
    "兵庫県兵庫県",
    "兵庫県兵庫県",
    "埼玉県埼玉県",
    "兵庫県兵庫県",
    "兵庫県兵庫県",
    "兵庫県兵庫県",
    "東京都東京都",
    "愛知県、大阪府愛知県",
    "墨田区錦糸町駅前東京都墨田区錦糸町駅",
    "東京都渋谷区東京都渋谷区"
];

$c = ahocorasick_init($data);
foreach ($list as $keyword) {
    var_dump($keyword);
    $matchedAddressList = ahocorasick_match($keyword, $c);
}
