<?php
$config = array(
    'host'   => 'dsapp.yz.zjwtj.com',
    'port'   => '8010',
    'format' => "http://%s:%s/initinfo/stock/%s",
    'list'   => array(
        '1101.txt',
        '1201.txt',
        '1206.txt',
        '120b.txt'
    ),
    'type' => array(
        '0x1101',
        '0x1201',
        '0x1206',
        '0x120b'
    ),
);
$list = array();
$index = 0;

$url1 = sprintf($config["format"], $config["host"], $config["port"], $config["list"][0]);
$url2 = sprintf($config["format"], $config["host"], $config["port"], $config["list"][1]);
$url3 = sprintf($config["format"], $config["host"], $config["port"], $config["list"][2]);
$url4 = sprintf($config["format"], $config["host"], $config["port"], $config["list"][3]);

$content1 = file_get_contents($url1);
//file_put_contents("./txt/".$config["list"][0], $content1);
$content2 = file_get_contents($url2);
//file_put_contents("./txt/".$config["list"][1], $content2);
$content3 = file_get_contents($url3);
//file_put_contents("./txt/".$config["list"][2], $content3);
$content4 = file_get_contents($url4);
//file_put_contents("./txt/".$config["list"][3], $content4);
//exit(1);

$content1 = mb_convert_encoding($content1, "utf8", "gbk");
$content2 = mb_convert_encoding($content2, "utf8", "gbk");
$content3 = mb_convert_encoding($content3, "utf8", "gbk");
$content4 = mb_convert_encoding($content4, "utf8", "gbk");

$json1 = json_decode(trim($content1, chr(239).chr(187).chr(191)), true);
$json2 = json_decode(trim($content2, chr(239).chr(187).chr(191)), true);
$json3 = json_decode(trim($content3, chr(239).chr(187).chr(191)), true);
$json4 = json_decode(trim($content4, chr(239).chr(187).chr(191)), true);

$json = array(
    $json1, $json2, $json3, $json4
);

$index = 1;
for($i = 0; $i < 4; $i++){
foreach($json[$i]["list"] as $v){
$list['list'][] = array(
    'code_type' => hexdec($config['type'][$i]),
        'code' => $v["code"],
        'preclose' => $v["preclose"],
    );
$index ++;
}
}

echo "count:".$index."\n";
$result_str = json_encode($list);
file_put_contents("./txt/1101.txt", $result_str);
