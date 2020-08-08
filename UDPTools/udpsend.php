<?php

error_reporting(0);

if (!isset($_REQUEST['target']) || !isset($_REQUEST['msg']) || $_REQUEST['target']=='' || $_REQUEST['msg']=='')
    die('ERROR: Illegal arguments!');

$eof = "\n";
$target = 'udp://' . $_REQUEST['target'];
$msg = explode($eof, $_REQUEST['msg']);

$socket = stream_socket_client($target, $errno, $errstr);
if (!$socket) die("ERROR: {$errno} - {$errstr}");
foreach ($msg as $m)
    fwrite($socket, $m);
fclose($socket);
echo 'Send success';

?>