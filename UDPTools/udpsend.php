<?php

error_reporting(0);

$eof = "\n";
if (!isset($_REQUEST['target']) || !isset($_REQUEST['msg']) || $_REQUEST['target']=='' || $_REQUEST['msg']=='')
    die('ERROR: Illegal arguments!');
$target = 'udp://' . $_REQUEST['target'];
$orimsg = $_REQUEST['msg'];

$msg = explode($eof, $orimsg);

$socket = stream_socket_client($target, $errno, $errstr);
if (!$socket) die("ERROR: {$errno} - {$errstr}");
foreach ($msg as $m)
    fwrite($socket, $m);
fclose($socket);
echo 'Send success';

?>