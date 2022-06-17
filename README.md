# ini
A cross-platform library that provides a simple API to read and write configuration files.

### SIMPLE USAGE

```c++

  // load
	ini_t* ini = ini_load("D:/test.ini");

  // set
	ini_set(ini, "tony", "name", "tony");
	ini_set(ini, "tony", "sex", "boy");
  
  // get 
  const char* name = "";
	const char* default_name = "";
	ini_get(ini, "tony", "name", (void**)(&name), (void**)(&default_name));
  
  // save to file
	ini_write(ini);
  
```
