<?php 
/**
* @package TESTPLUGIN
*/

/*
Plugin Name: TESTPLUGIN
Plugin URI: https://gamers-community.network
Description: test plugin to test my php coding skills nibba
Version: 0.0.1 
Author: GC_ARCHERY
Author URI: https://gamers-community.network
License: GPLv2 or later
Text Domain: test_PLUGIN 
*/

defined('ABSPATH') or die('No nooo, naughty naughty.');

if(file_exists(dirname(__FILE__) . '/vendor/autoload.php')){
	require_once dirname(__FILE__) . '/vendor/autoload.php';
}

use Inc\Activate;
use Inc\Deactivate;
use Inc\Admin\AdminPages;

if(!class_exists('TestPlugin')) {

class TestPlugin {

	public $plugin;

	function __construct() {
		add_action('init',array($this,'custom_post_type'));
		$this->plugin = plugin_basename( __FILE__ );
	}

	function register() {
		add_action('admin_enqueue_scripts', array($this, 'enqueue'));
		add_action( 'admin_menu', array($this, 'add_admin_menu') );
		add_filter( "plugin_action_links_$this->plugin", array($this, 'settings'));
	}

	public function settings($links) {
		$settings_link = '<a href ="admin.php?page=test_plugin">Settings</a>';
		array_push($links, $settings_link);
		return $links;
	}

	public function add_admin_menu() {
		add_menu_page( 'Test Plugin', 'Test Plugin', 'manage_options', 'test_plugin', array($this, 'admin_index'), 'dashicons-admin-tools', 110 );
	}

	public function admin_index() {
		require_once plugin_dir_path( __FILE__ ) . 'templates/admin.php';
	}

	function activate() {
 		Activate::activate();
	}

	function deactivate() {
		Deactivate::deactivate();
	}

	function custom_post_type() {
		register_post_type('book',['public' => true, 'label' =>'test']);
	}

	function enqueue() {
		wp_enqueue_style('pluginstyle', plugins_url('/assets/styless.css', __FILE__));
		wp_enqueue_script('pluginscript', plugins_url('/assets/scriptss.js', __FILE__));
	}

}

if(class_exists('TestPlugin')) { 
	$testPlugin = new TestPlugin();
	$testPlugin ->register();
}

 register_activation_hook(__FILE__, array($testPlugin, 'activate'));
 register_deactivation_hook(__FILE__, array('Deactivate', 'deactivate'));
}