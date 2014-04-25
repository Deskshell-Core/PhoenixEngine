#include "cDetect.c"
#include "autoconf.c"

// Cool. TSRM is uber-easy. Lets take advantage of this ~100 configure.in.
// Generate: php/TSRM/tsrm_config.h
int main(int argc, char** argv){
	config_begin();
	ac_register();
	
   	config_option_register_group("cDetect extra");
	config_option_register("cache",0,"cache.txt",0,"cDetect cache file to use.");
	config_option_register("out",0,"php/TSRM/tsrm_config.h",0,"Output config.h.");
	
	// TSRM options
	config_option_register_group("TSRM: ThreadSafe Resource Management");
	config_option_register("with-tsrm-pth",0,"no",0,"Use GNU Pth (supply PATH or \"yes\")");
	config_option_register("with-tsrm-st",0,"no",0,"Use SGI's State Threads (supply PATH or \"yes\")");
	config_option_register("with-tsrm-pthreads",0,"no","yes","Use POSIX pthreads (Default)");
	config_option_register("with-tsrm-zts",0,"no","yes","Use POSIX pthreads (Default)");
	
	if(config_options(argc, argv)) {
		// And encore!
		config_cache_register( config_option_get("cache") );
		config_header_register( config_option_get("out") );
		
		// options.
		if( // If neither of these are set, use the default.
			config_equal( config_option_get("with-tsrm-pth"), "no" )
			|| config_equal( config_option_get("with-tsrm-st"), "no" )
		) {
			config_option_set("with-tsrm-pthreads","yes");
			config_macro_define("PTHREADS","1");
		}
		if(config_equal( config_option_get("with-tsrm-zts"), "yes" )) {
			config_macro_define("ZTS","1");
		}
		
		// Header
		ac_header_stdc();
		ac_header_dirent();
		config_header_check("utime.h");
		config_header_check("dirent.h");
		config_header_check("stdarg.h");
		config_header_check("alloca.h");
		config_header_check("unistd.h");
		config_header_check("limits.h");
		config_header_check("stdlib.h");
		config_header_check("stdarg.h");
		config_header_check("string.h");
		config_header_check("float.h");
		config_header_check("st.h");
		
		// Function
		config_function_check("sigprocmask");
		
		// Customs
		config_function_check_library("pthread_create","pthread")
			|| config_macro_define("PTHREADS","1");
		
	}
	config_end();
}
