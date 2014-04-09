#include <iostream>
#include "v8/include/v8.h"
using namespace std;
int main() {
	// Does nothing but printing the v8 version, as it is unavailable from the v8.h file...
	cout << v8::V8::GetVersion();
}
