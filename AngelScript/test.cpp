#include <iostream>
#include <stdio.h>
#include <assert.h>

#include <angelscript.h>
#include "scriptdictionary/scriptdictionary.h"
#include "scriptstdstring/scriptstdstring.h"
#include "scriptbuilder/scriptbuilder.h"

using namespace std;

void print(string &msg) {
  printf("%s", msg.c_str());
}
void MessageCallback(const asSMessageInfo *msg, void *param) {
	cerr << "ERR(" << msg->row << "): " << msg->message << endl;
}

int main(int argc, char** argv) {
	if(argc<=0) cerr << "Too few args!" << endl;
	cout << "Starting..." << endl;
	
	// Create the script engine
	asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	int r;

	// Set the message callback to receive information on errors in human readable form.
	r = engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL); assert( r >= 0 );

	// AngelScript doesn't have a built-in string type, as there is no definite standard 
	// string type for C++ applications. Every developer is free to register it's own string type.
	// The SDK do however provide a standard add-on for registering a string type, so it's not
	// necessary to implement the registration yourself if you don't want to.
	RegisterStdString(engine);
	
	// Also register the dictionary.
	RegisterScriptDictionary_Generic(engine);

	// Register the function that we want the scripts to call 
	r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL); assert( r >= 0 );

	// The CScriptBuilder helper is an add-on that loads the file,
	// performs a pre-processing pass if necessary, and then tells
	// the engine to build a script module.
	CScriptBuilder builder;
	r = builder.StartNewModule(engine, "MyModule"); 
	if( r < 0 )  {
  		// If the code fails here it is usually because there
  		// is no more memory to allocate the module
  		printf("Unrecoverable error while starting a new module.\n");
  		return 1;
	}

	r = builder.AddSectionFromFile(argv[1]);
	if( r < 0 ) {
  		// The builder wasn't able to load the file. Maybe the file
  		// has been removed, or the wrong name was given, or some
  		// preprocessing commands are incorrectly written.
  		printf("Please correct the errors in the script and try again.\n");
  		return 1;
	}

	r = builder.BuildModule();
	if( r < 0 ){
  		// An error occurred. Instruct the script writer to fix the 
  		// compilation errors that were listed in the output stream.
  		printf("Please correct the errors in the script and try again.\n");
  		return 1;
	}
	
	// Find the function that is to be called. 
	asIScriptModule *mod = engine->GetModule("MyModule");
	asIScriptFunction *func = mod->GetFunctionByDecl("void main()");
	if( func == 0 ){
		// The function couldn't be found. Instruct the script writer
  		// to include the expected function in the script.
  		printf("The script must have the function 'void main()'. Please add it and try again.\n");
  		return 1;
	}

	// Create our context, prepare it, and then execute
	asIScriptContext *ctx = engine->CreateContext();
	ctx->Prepare(func);
	r = ctx->Execute();
	if( r != asEXECUTION_FINISHED ){
  		// The execution didn't complete as expected. Determine what happened.
  		if( r == asEXECUTION_EXCEPTION ){
    		// An exception occurred, let the script writer know what happened so it can be corrected.
    		printf("An exception '%s' occurred. Please correct the code and try again.\n", ctx->GetExceptionString());
  		}
	}
	
	// Clean up
	ctx->Release();
	engine->Release();
}
