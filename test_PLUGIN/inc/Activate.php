<?php
/**
* @package TESTPLUGIN
*/

namespace Inc;

class Activate {
	public static function activate() {
		// echo 'test';
		flush_rewrite_rules();
	}
}