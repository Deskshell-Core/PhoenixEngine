# Files for libchromiumcontent

- content/public/renderer/v8_value_converter.h
    Provides v8 bridging! yay!

- content/public/browser/web_ui.h
    Contains ui access w/ JS stuff.

- base/command_line.h
    Obvious.

- base/bind.h
- base/callback.h
    callbacks and the like - amzing callbacks, on a note.

- base/message_loop/message_loop.h
- base/run_loop.h"
    obvious.

- base/task_runner.h
- base/threading/thread.h
    Threading, biatch :3

- base/json
    JSON support. Can drop libjson?

- base/native_lib.h
    Can extend browser/renderer...hurhurhur.

- base/process
    Process control



# Resource links
- http://www.chromium.org/developers/design-documents/multi-process-architecture
- http://www.chromium.org/developers/design-documents/inter-process-communication
- http://www.chromium.org/developers/content-module
- http://www.chromium.org/developers/design-documents/browser-components
- http://www.chromium.org/developers/design-documents/threading
-

# Structure
- RenderProcess is the RENDER PROCESS.
- RenderProcessHost is the BROWSERs access to RENDERERs.
- RenderView
- RenderViewHost Is an access to all the views in a renderer process.
- Each view has an ID
- Browser can talk to a view via its ID, thru the RenderviewHost
-

# What needs to be done
    PHP/PHP-CPP:
        - Must be implemented within the Render process.
        - Possibly obtain UI module.
        - IPCing to browser host, to handle events.
        - Possibly interfaces the backgrounded nodejs script?
        - Possibly use a php script in background?
    NodeJS:
        - Must be implemented within render process.
        - Must librarize in order to export internal v8 code.
        - Upgrade v8 API to match...
        - For the background worker:
            - Must be implemented as a new thread.
            - Threadsafe v8 coding
            - IPC handling (to be poked by views)
            - Must be able to recieve events and messages. Should very probably use something like socket.io...
            - POSSIBLE worker-api:
                worker.on(event, function(e)) -- Handle event, e being a simple, unserialized, JSON object.
                worker.get(name) -- get a variable fro the worker.
                worker.eval(code) -- eval code in worker, dangerous.
                worker.call(function, {params}) -- call a function with given functions
            - Possible in-worker api:
                me.emitAll(event, object) -- emit an event to all available windows
                me.emit(event, Rid, Vid, object) -- emit event to RenderProcessHost[Rid]->RederViewHOST[Vid].
                me.functions -- An object ot export functions
                me.variables -- an object to export variables.
    FLTK:
        - Must be implemented in Browser thread.
        - Must be using GL(UT) interface to the brwoser, for painting.
        - Will only run from UI thread, maybe renderer too, depending on thread structure.
    libcan/d0p/dpk
        - Implemented within renderer, as I/O operation
        - Content must be IPC'd to browser, methinks.
    SQLite3
        - Nodejs module must become available to chromium's v8.



# Workflow!
- Create a start routine to launch soemthing like MainContents
- Instantiate a *Content Delegate* and pass that to *content::contentMain(argc,argv,_DELEGATE_)*
- The derived content::ContentMainDelegate must export functions to initialize toolkit, resources, etc...
--- SPLIT.
Browser:
- browser client has methods in order to create browserMainParts and rendererHosts
- RenderProcessHostCreated may register message filters?
- Browser client also seems to intercept requests and resource loading handles.
- Also seems to make AccessTokens...
- BrowserMainParts takes care of *WINDOWING*.
Renderer:
- ContentRendererClient has events before and after thread spawning.
- Renderer can access webkit and modify stuff...need to do this.



# Workflow in content_shell
- Prepare logging and ensure the platform-specific thigns are correct.
- Enter the browser process part and begin to run the browser loop.
- Within this loop, create a window and put the browser inside it.
- Spawn render processes and receive their output and...well, render it.
- Upon specific action, clsoe all brwosers
- Return exit code of browser loop
- Exit app

## What classes are neded...
- Pre-Browser (aka. initializing)
    * ContentMainDelegate (Must call to browser or renderer to start it)
    - ContentClient (Common stuff such as user agent, name, version, etc...)
- Browser
    * BrowserMainParts (Construction and destruction; initializes toolkit, etc, and releases everything)
    * ContentBrowserClient (Runs on the UI thread and maintains startup of browser as well as overrides, as well
        ... as setting up processes, Render and Browser hosts. Also takes care for cookies, cache, desktop
        ... notifications, )
    * BrowserContext (maybe? probably not.)
    * NotificationObserver (Maybe?)
    * JavaScriptDialogManager
    * net::NetworkDelegate
    * PluginServiceFilter (Needed for deskshell extensions!)
    - BrowserURLHandler (Handling)
- Render
    * ContentRendererClient (Handles started render thread, render view, GetNavigationErrorStrings to handle
        ... 404 pages and the like, JavaSCript bindings!, )
    * RenderProcessObserver
        - Base class for objects that want to filter control IPC messages and get notified of events.
        - Does iPC handling, inportant.
    * RenderViewObserver
        * Base class for objects that want to filter incoming IPCs, and also get notified of changes to the frame.
        * Can do iPC
        * Handles a bunch of JS like events. Not good for PHP implementation tho.
        * Can extend its livetime...for what, nobody knows.
- Utility
    - ContentUtilityClient (Takes care of utility stuff)
    - UtilityThread
