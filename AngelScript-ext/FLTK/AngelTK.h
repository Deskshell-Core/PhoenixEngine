#ifndef HAVE_ANGELTK_H
#define HAVE_ANGELTK_H

#include <assert.h>

// FLTK includes
#include <FL/Fl_Window.H>

// AngelScript
#include "angelscript.h"

// We only export the init function
void Register_AngelTK(asIScriptEngine *engine);
void Register_AngelTK_Native(asIScriptEngine *engine);


#endif
