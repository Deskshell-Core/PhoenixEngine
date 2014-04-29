class TargetHandle {
	TargetHandle(string name) {
		print("Target: "+name+"\n");
	}
	void opAssign(dictionary data) {
		string rule;
		data.get("rule", rule);
		print("Rule: "+rule+"\n");
	}
};

void target(string name) {
	TargetHandle th("beep");
	dictionary td = { {"rule","abc"} };
	th = td;
}

void main() {
	/*target("libfoo") = (
		{"rule", "static_library"}
		//{"tag", "foo"}
		//{"sources", {
		//	files("foo/*.cpp"),
		//	"lib.cpp"
		//}}
	);*/
	print("Hello world!\n");
}
