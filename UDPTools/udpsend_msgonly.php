<?php

error_reporting(0);

if (!isset($_REQUEST['msg']) || $_REQUEST['msg']=='')
    die('ERROR: need message!');

$eof = "\n";
$target = 'udp://127.0.0.1:18888';
$orimsg = $_REQUEST['msg'];

$msg = explode($eof, $orimsg);

$socket = stream_socket_client($target, $errno, $errstr);
if (!$socket) die("ERROR: {$errno} - {$errstr}");
foreach ($msg as $m)
    fwrite($socket, $m);
fclose($socket);
echo 'Send success';

?>