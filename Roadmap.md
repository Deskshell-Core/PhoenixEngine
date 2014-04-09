# Deskshell "WebDev SDK" roadmap

## Dependencies:

	- IceTea (fork of Xagasoft/build)
		- libbu
	- CEF
		- libcef_dll_wrapper
	- libd0p
		- libcanister
	- libphp5
		- TSRM
		- Zend VM
		- PHP-CPP
		* Extensions:
			- php-pthreads (--enable-maintainer-zts)
			- php-qb
			- php-sockets
			- php-curl
				- libcurl
	- node-cV8
		- libuv-cV8
			- uvpp
				- libuv
		- cares-cV8
			- c-ares
		- http_parser
		- module
		- zlib or minizip
		- libcanister-cV8
		- libd0p-cV8
		- libchannel-cV8
		- FLTK-cV8
	- FLTK
	- ttvfs
		- ttvfs-d0p
		- ttvfs-dpk
		- ttvfs-can
		- ttvfs-zip
	- libucl


## Process

1. Deskshell is launched and either a dpk (Deskshell Package) or a .desk (JSON format) is given as an argument, with the following structure:
{
	"name": "App Name",
	"version": "Version",
	"maintainer": [
		"name": "Name of maintainer 1",
		"email": "Email"
	],
	"htdocs": {
		"mac": "../Resources",
		"default": "./www"
	},
	"bootstrap": {
		"mac": "../Resources/lib/main.js",
		"default": "./www/lib/main.js"
	}
}

2. Create a browser, with a hidden frame.
3. CefApp::*() ->
	- Resolve any events, such as to create custom schemes to map access to .can/.d0p/.dpk files
	- As well as to intercept requests that attempt to open a PHP file. Execute it with embedded PHP 5.5.x and return the result.
		* PHP scripts are ment to receive a apache-mod_php like environment.
	- Mainly implement the app:// scheme. The host is the name of the app. I.e.:
		app://drag0n/index.php
	... will point to drag0n.app/Contents/Resources/index.php and cause the request to be intercepted, the PHP script evaluated and returned.
4. MyRenderProcessHandler::OnWebKitInitialized() -> 
	- Load up all javascript/v8 modules and create the extensions neccessary.
	- Inject some Deskshell API.
	- Inject the bootstrap script. The bootstrap script can unhide the window via the FLTK bindings injected for the created window.
	* It should be noted, that the bootstrapping script runs int he same context as the following javascript, so it gets all the DOM access.
	... After the modules are loaded, this script is the first to be executed before anything else. It may show a splash screen, or whatever.
	... In fact, it could operate completely without showing the browser window and could just utilize the FLTK API to show widgets of any kind, really.
	... A good example would be the following: A sound-editing program requests the user to pic a file to work with first. 
	... One the file is selected and loaded, app.show(); is called to unhide the already-prepared browser and to "launch" the app.
5. Let the application work as it wants to.
6. Make sure that once the last window of an app is closed, the app closes completely (Mainly for OS X. Might become a setting...)


## Components
libphp5:
	- Used to evaluate requests to a local PHP file with the bestest templating/scripting language that there exists. Mainly, we want to enable Yii framework apps to run. SQLite should be made available too for database reading, etc.
libcanister/libdrag0n:
	- Used to access headered archieves such as drag0n Packages and Deskshell Packages, whose configuration may be stored in the proposed header.
	- Libcanister is ultra-fast, as it uses an API and workflow similar to the usual filesystem - its therefore ultrafast and reliable.
FLTK:
	- Windowing and widget API to make handling OS based UIs easier. Features include file-choosers, windows with progress bars, threaded and live output of commands, text editors and wizards. Useful for configuring the actual applications.
	- Mainly used to unify the OS specific GUI actions as best as possible. Example includes menubars and window titles.
node-cV8:
	- Porting the NodeJS environment into the brwoser to enabe publicy known functions such as require() and modules such as child_process, http(s), fs, etc etc.
ttvfs:
	- Unify FS access for browser request. Best feature: Access archives like they were real folders! Ultra useful for asset containers:
		app://sshClient/assets/jquery.can/min.js
	- Will be used to create the "killer" feature. ttvfs can mount files from the memory as virtual files. That way, a whole app can be bundled into a binary (like the demo app will) and be ran straight from memory. Now, dont tell me this isn't cool =D! This completely encrypts the application, if needed.
build (aka. IceTea):
	- My fork of Xagasoft's Build. I just call it that due to an old concept, thats really all o.o
	- It handles the build on each and every platform and will also include some specific features, such as:
		- Building PHP extensions natively.
		- Building cV8 extensions natively.
		- Creating FLTK plugins.
		- Creating canisters, drag0n packages and deskshell packages on the fly.
		- Easily linking against libraries bundled in Deskshell.
libucl:
	- Used to parse JSON and YAML; its nice and tiny. :)
	

## Modularism
To make the application modular and implement a common use, I will be trying to imitate the nodejs native API. That means, that node-cV8 will be using the same JS files for the userland part, whilst the native code is not plain v8, but cV8 instead. That way, nodejs' environment shall be copied and impersonated as best as possible. Excluded from that is src/node.js, as it is the `int main(argc, argv)` of nodejs' itself.

NodeJS implements the following *native* modules:
	- tty_wrap
	- timer_wrap
	- cares_wrap
	- signal_wrap
	- evals
	- natives
	- buffer
	- fs
	- constants

NodeJS implements the following *userland* modules:
	- _debugger
	- _http_agent
	- _http_client
	- _http_common
	- _http_incoming
	- _http_outgoing
	- _http_server
	- _linklist
	- _stream_duplex
	- _stream_passthrough
	- _stream_readable
	- _stream_transform
	- _stream_writable
	- _tls_legacy
	- _tls_wrap
	- assert
	- buffer
	- child_process
	- cluster
	- console
	- constants
	- crypto
	- dgram
	- dns
	- domain
	- events
	- freelist
	- fs
	- http
	- https
	- module
	- net
	- os
	- path
	- punycode
	- querystring
	- readline
	- repl
	- smalloc
	- stream
	- string_decoder
	- sys
	- timers
	- tls
	- tracing
	- tty
	- url
	- util
	- vm
	- zlib
	
NodeJS is a big complex system. But properly impersonating the native API should do the trick just nicely. However, that might take a while...and that means that not all modules may be released at the same time, but one-by-one. But once they are all implemented, we will have a nodejs in a browser. :)


