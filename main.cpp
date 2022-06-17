#include "ini.h"

int main(int, char**)
{
	ini_t* ini = ini_load("D:/test.ini");
	const char* name = "";
	const char* default_name = "";
	ini_set(ini, "tony", "name", "tony");
	ini_set(ini, "tony", "sex", "boy");
	ini_set(ini, "anne", "name", "anne");
	ini_set(ini, "anne", "sex", "girl");
	ini_get(ini, "tony", "name", (void**)(&name), (void**)(&default_name));
	ini_write(ini);
}