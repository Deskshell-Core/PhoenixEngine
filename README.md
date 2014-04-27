# Phoenix Engine

This is going to be a project that allows you to take your favorite technologies to the desktop - in a way, that you did not think about.


## What makes this different to node-webkit?

This project is based off the same resources, but differently. We use `libchromiumcontent`, a library that combines the components, instead. This way, we are fully independent from the Google source tree - and further, we can swap the library out. I.e., if you want a certain feature to be disabled, you recompile the library with the disabled feature, put it into your distribution, and it will still work, as we are only touching the real surface - the `Content API`.

Node-Webkit also offers only limited support for actual GUI access and is purely javascript. Optimal for many apps. But what if you need the power of PHP, bundled resources, streamline access to a compressed file container (aka. ZIP)? These, and more, are the things that this projects wants to cover.


## What, why, when. Aka: What resources, why these, when its gonna be out?

I am going to list all the resources I use with a brief description and the reason why. Its a lot - but I am only at about 4 additional MB. More to that part later.

* FLTK: Fast Light ToolKit.
	* This is a GUI abstraction, that will allow me to work pretty independent from the host OS' GUI API (Wow, caps). Further, since this is a simple API, ti will also be accessible to the application itself, so that you can actually use its widgets and components too! Example: you can show a box with a install-like wizard, if an update for your app is available.
* NodeJS (c-ares, libuv, http-parser, openssl, zlib)
	* NodeJS will be made open! I am working on a way/solution to run nodejs' environment in a browser context. That would make the non-plus-ultra; `window` alongside `process` and `require` with native capabilities? I like this idea. :) Some integrated modules:
		- WS
		- TAGG
		- Async
* PHP: Pre-Hypertext Processor - aka. one of the worlds most famous web scripting languages
	* We will use PHP to render pages, to administer databases, to do things! But PHP won't come alone. It in fact will come with a few extra extensions. Such as:
		- pthreads
		- uv
		- v8js
		- PHP-CPP
		- yar
		- curl
* ttvfs: FS abstraction with extras
	* This library is capable of mounting/reading .zip files just like they were a folder - and further, it is also capable of treating memory chunks as local files. So if you tell it to mount `./foo.txt` from a `char buffer[1024]`, then it will do this! After that, you can operate on this chunk like it was a file. The advantage: We can use this handy library to treat a .zip file as a folder, to store assets and such. But not just that! We can also add our own types - liek the following.
* libcanister: Speed-oriented file ocntainer
	* This library oriented itself from the way files are stored on a HD, and uses this to store files within a generic container. So we can use ttvfs plus this to read files off a virtual folder within this file!
* sqlite + Trololo
	* I choose to use the Trololo C++ wrapper over sqlite because it looks simple for me to take advantage of - and, well, it trolls. :) I am hereby using the amalganated version of sqlite3.
* libucl: JSON / YAML hybrid configuration parser
	* This will be the way I lay out configuration files for configuring the engine from a file.
* cDetect: Autoconf in C
	* This is being used to make FLTK and other libraries believe that they were configured thru autoconf, although they weren't. This way, I can compile everything on each platform, as long as it sports a proper compiler.
* uvpp
	* libuv wrapper for C++11 that will help me take advantage of libuv in a more clean way (lambdas etc).
* curl
	* Needed for php-curl, nodejs-curl and in general a pretty amazing library to pull files from across resources.
* AngelScript
	* The engine will export some bindings to internal resources (mainly FLTK, a light binding to Content API, libcurl, ttvfs, libcanister, ...) so that prototyping apps can be made easier. I absolutely recommend taking a look at it. I am hereby redistributing the currently latest SDK in its original form, all licenses and readme's contained.
	
As soon as I have a working version, I will be posting screenshots and such. But first, I need to gather my components...


## Development

I am a one-man-army. All I do is completely made off my own. The fork of the build system, rewrite o the build rules, the concepts, bindings, cDetect configs, patches... that all goes to my own. Therefore, I can only really list what I did and did not do so far. To make it a bit easier to follow, I am only listing it roughly.

- PHP
	I have a 100% working configuration for TSRM, however, it requires parts of Zend. And I have not completed Zend yet. The "main module" and the extensions are fully missing, yet. BTW, I am using PHP 5.5.11 at this moment. I am also using it as a submodule.
- Phoenix Engine Core
	Not started. Unless I have my components done, I can't do anything else. I require all other components to build and work first!
- FLTK (+ image/forms/gl/png/jpeg/z), ttvfs, libcanister, c-ares, http-parser, sqlite3, Trololo, libuv, AngelScript (+ extensions), libucl
	 Configures, builds, works. :)
	 
	 
## Motivation?

I have been motivated to this project for quite a couple of reasons:

- node-webkit is complicated to build, needs a lot of resources ro be built. In general, you can use it, but for me, it doesnt give all the options I want...
- AppJS was a great start, but even I had to deprecate it. CEF1 is dead, its base dead within.
- Other projects, like Awesomium, are closed source.
- Similar projects, Bellite.io, went down due to discontinuing of software
- Deskshell looks like you tried to tape a console controller. It works, but its not complete.

The idea with PHP came, when I was thinking about designing a game. I am very good in PHP, and to me, doing something liek the following, shapes up very quickly infront of my eyes:


	Imagine you are working on a game.
	The game uses a 2D, tile based, map, where many things are done using sprites. But, its an MMORPG, so you extend it. One day, you decide to randomly apply a fix to a map. What would be the best way for a client to notice this?
	To me, its simple. The client uses WebSockets (JS) to talk to the server. At some point, it gets the following JSON message:
	
		{
			"messageType":"update",
			"data":"http://mygame.com/maps/data.map"
		}
	
	Now, you could tell the client that it has to download the stuff. But hey, we are in a fully controllable browser environment! So what we could do, is spawn a worker process - based on PHP. We add three event listeners to it: start, progress and end. So this could look like this:
	
		// JS
		worker = new PHPWorker(__dirname+"/worker.php");
		worker.on("start",function(e){ console.log("Starting update..."); });
		worker.on("progress",function(e){ console.log("Downloaded: "+e); });
		worker.on("end",function(e){ console.log("Update over! Following was updated: "+e.updateInfo); });
		worker.emit("run", updateMessage);
	
	Looks simple, no? Now, the PHP side!
	
		// PHP
		<?php
		include_once dirname(__FILE__)."/curl_wrapper.php";
		$self->on("run", function($e){
			// We are gonna pretend that CurlThread has a method called "getPercentage()".
			$download = new CurlThread($e->data);
			$download->start();
			$self->emit("start"); // e == null
			// We are gonna wait here.
			while( $p = $download->getPercentage() ) {
				if($p != 100) $self->emit("progress",$p);
				sleep(0.5); // Give it some time to work out.
			}
			// Magically extract some info from the download... now its in $dlInfo
			unset($download); // dtor
			$self->emit("end",$dlInfo);
		});
		?>
		
	What we have now is, that the game continues without a flaw. But once the progress is done, we report it back from the worker, that, without a callback hell or promises, has done its work. Due to PHP's procedural structure, we dont even need callbacks. The emit() and on() function are only there to really call into PHP scope. But we may as well export the actual stdout to the JS context.
	
	However you see it; you just saw a simple explanation of combining the various scripting languages together. JS drives your game, PHP keeps you updated. You just feed your worker with updates and it will happily go and gather the stuff, report back when its done, and your game keeps running. In fact, you could handle some other progress - like a waiting screen - while PHP did its work.
	

You see, this sounds simple. But you can easily imagine a bigger project with a similar use case - where workers are needed more often. Due to the engine giving you a fully controlled browser environment, you can also do RPC to other frames, to the GUI process to spawn windows and alike, and more. 

My goal is, to create one engine, that can help you get your app going. That simply takes your knowledge and helpes you forge it into a desktop app - a client for your website? - or your own terminal emulator! Think of what you want, but with these components combined, its not far away.


## Gimmicks

- Imagine you are creating a Text editor for coding (_nobody has ever done this, caugh_) and you prompt your client to open a file but they open a wrong file. You can now use FLTK to actually print a pretty error message.
- Storing your assets in container files (libcanister) will make it possible for you to update many things at once! Just feed a download worker with the URL, and it will replace your current assets. Reload them on the go, and you are updated.
- AngelScript helps you test things! Use this to your advantage and prototype your stuff, or entire app, before you take it to the compiler.
- Objective-J IS native. Exchange CPMenu with FLTK's Menu (FLMenu) and you suddenly have a REAL menu bar! OS X included.
- Threading within a browser context, using nodejs' TAGG (threads-a-go-go) is amazing. Keep your UI on the main thread, and schedule some nodejs based, heavier tasks, on a sub-thread. Join it back together and present the results. Example: Create an excessive chart of all your earmings and spendings from the last five quarters. That might take a while. :)
- Use good old PHP technologies, like the Yii Framework - it will think its on a server; although it's not. MVC the way you learned it, but be fully native!

Do I need to list more? ^.^


## Chromium Content?

If you are on a Mac, and have access to Github's Atom.io, navigate into the bundle and, look at it yoruself.

	$ cd Atom.app/Contents/Frameworks/Atom Framework.framework/Libraries
	$ ls
	
They are using an open source framework, for an editor they actually want to sell. Run `nm` on the Atom Framework binary to find libuv, c-ares, nodejs and more to be burried inside as well. Research further, to soon realize that they have merged nodejs into their browser context.

...See what I'm saying? I am just using the same resources as they do. :)


## I can help - how?

- If you are good in porting code from UNIX to Win32, then let me know!
- If you are keen with Google's Content API, let me know too!
- If you can contribute bindings for AngelScript or anything else, show me and I'll consider including it. :)


## Concept in other words.

I have posted a Roadmap file. Read this for a bit of weirded information that contains my ideas for the Content API, paths to look at, and more. Currently, I am developing this all on OS X.

If you want to rebuild:

	$ git clone https://github.com/IngwiePhoenix/IceTea.git
	$ cd IceTea/
	$ cd src
	$ bison build.y
	$ flex build.l
	$ cd ..
	# Long long long compiler command.
	$ clang++ -Iminibu -Isrc -Isrc/functions -Isrc/conditions -Isrc/views -Isrc/misc src/*.cpp src/functions/*.cpp src/conditions/*.cpp src/views/*.cpp minibu/src/*.cpp -o build
	$ sudo ln -s "$(pwd)"/share /usr/share/build
	$ sudo cp build /usr/local/bin
	$ cd ..
	$ git clone --recursive https://github.com/Deskshell-Core/PhoenixEngine.git
	# Grab some coffee, thats half a gig to be downloaded.
	$ build
	-- Building: ./out/libPhoenixEngine.dylib...
	.........
	
I can not guarantee this will work OOB on non-Macs.


If you wish to poke me, open an issue. :)
