#ifndef XUNX_INI_H
#define XUNX_INI_H

typedef struct ini_t ini_t;
ini_t* ini_load(const char* path);
int ini_get(ini_t* ini, const char* section, const char* key, void** value, void** default_value);
int ini_set(ini_t* ini, const char* section, const char* key, const char* value);
int ini_write(ini_t* ini);

#endif //XUNX_INI_H