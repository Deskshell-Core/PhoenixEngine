#include "AngelTK.h"

void Register_AngelTK(asIScriptEngine *engine) {
	//if( strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY") )
	//	Register_AngelTK_Generic(engine);
	//else
		Register_AngelTK_Native(engine);
}

void Register_AngelTK_Native(asIScriptEngine *engine) {
	int r;
	
	// We try our luck with Fl_Window.
	r = engine->RegisterObjectType("Fl_Window", sizeof(Fl_Window), asOBJ_GC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("Fl_Window", "void label(const string &in)", asMETHOD(Fl_Window,label), asCALL_THISCALL); assert( r >= 0 );

}

// void Register_AngelTK_Generic(asIScriptEngine *engine)
