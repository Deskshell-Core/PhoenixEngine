# Phoenix Engine - One engine, thousand possibilities.

[![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/Deskshell-Core/PhoenixEngine?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

Phoenix Engine is a project, that aims to do something that people always have wanted: Using their skill in web technologies, and turn that into a native application. Be it AngularJS, Cappuchino, a plain Bootstrap site, or even soemthing like a Yii based site. Everyone has at least tried this method once, but came to a sad recognition: It wasn't quite _the_ thing they were looking for...

And this is where Phoenix Engine wishes to differ - in many, many ways. Read on to learn more!


## Feature highlights
- Write your site with HTML, JavaScript and CSS - but also PHP, ObjectScript and AngelScript.
- Break the limit: Native GUI bindings to all script languages - and, they're cross-platform too!
- Use worker processes and communicate back and forth. Use the language of your choice and turn it into your most pwoerful tool.
- Pack your applications into archives to save space and simplify updating assets.
- Access archives like they were actual folders - no need to use any kind of API for that!
- Encrypt your assets into the binary and hide content form onlookers.[1]
- You decide how your app runs: preset start script or custom - customizable window frame and more.
- NodeJS-like environment in the browser context (`require()` for core modules such as FS, HTTP/S, Util and more!).
- Extend Phoenix Engine with C++ addons to up its awesomeness!


[1]: __You can never truely hide everything from someone that really wants to get in your stuff. A good example are GTA:V and Skyrim mods.__

## Idea and motivation
When AppJS was still a thing, I really enjoyed working with it - and it was that got me into learning NodeJS. Today, I am very glad it did, because NodeJS turned out to be an essential tool for my development now. But when CEF1 was deprecated, and versions moved on fast, I obtained maintainership over it, but had to realize that there was no way out of this misery, and I deprecated it at last. One alternative that came up was node-webkit, but until today, it's architecture remains horrific. I mean, it's so hard to make a proper OS X bundle out of it and your own app...

And this is where the Phoenix Engine - short: PE - wants to differ mandatorily. First, that is what the resulting app structure should look like:

```
# Windows/Linux
app/
  | engine
	- phoenix_engine.dll/.so
	- {all sorts of shared libs that are required by Chromium}
	| modules
	  | php/ -- Native PHP addons (such as php_curl.dll/.so). Also is added to include path.
	  | node/ -- A bit like node_modules, but used for more internal modules - such as if you made your own app distro.
	  | angelscript/ and objectscript/ -- Obvious.
	  | pe/ -- Native modules for Phoenix Engine itself.
  - myapp.exe -- links against engine/phoenix_engine.dll/.so
  | Resources
	| {All the files you'd put into a webroot.}

# Mac OS X
MyApp.app/Contents/
	| Frameworks/
	  | Phoenix Engine.framework
		- phoenix_engine -- dynamic lib
		| lib/ -- Contains other dynamic libraries
		| modules/ -- Same as above
		| Resources/
	      - Phoenix Engine Helper {NP/EH/...}.app -- All the helper apps are here.
	| MacOS/
	  - myapp -- Links with @executable_path/../Frameworks/Phoenix Engine.framework/phoenix_engine
	| Resources/ -- All your web stuff.
```

As you see, the structure is almost idendical. Minor adjustments are needed from one platform to another - OS X was an example I am sure I will have to cover - but the structure will almost stay the same everywhere.

Now you may wonder where the app actually starts. Other alternatives use something like a `package.json` file, but PE does not. Well - not quite. You can start your app...

- ...by placing a `start.as` into the same folder as the stock binary. This is an AngelScript file, which has full access to any API - GUI, FileSystem, ... - and can be used as a custom script to start the engine.
- ...by dismissing the above. With no script in the folder, the `Resources/` folder is assumed to be the webroot and it's containing index file will be opened. If there are multiple, it'll choose one.
- ...by placing a `package.json` into the executable's folder or the `Resources/` folder. That file contains where to find what and if there is any PE module needing pre-loading.
- ...by writing your own C++-based start program.
- ...using  a special executable to which an archive has been appended - open or encrypted - with all your data stored inside.

As you can see, there are many ways and you can decide which way to go!

## Development
Currently, I am working on the [IceTea build tool](https://github.com/IngwiePhoenix/IceTea) which should enable it for me to build the various components (PHP, AngelScript, libuv, libcurl, ...) and put them together. *Without this tool, this engine will have a massive problem building since it is very needy in many things.* So if you want to help it, please clone IceTea or get in touch with me using this repo's gitter.

Thanks!
