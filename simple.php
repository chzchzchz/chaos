<?php

if (!isset($_GET) || !isset($_GET['x']) || !isset($_GET['y']))
	exit;

$x = (int)($_GET['x']);
$y = (int)($_GET['y']);
if ($x <= 0 || $y <= 0 || $x > 9000 || $y > 9000)
	exit;

header("Content-Type: image/jpeg");
passthru("/var/www/localhost/htdocs/chaos/chaos $x $y |  convert jpg: -normalize jpg:- ");

?>
