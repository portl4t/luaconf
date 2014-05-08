
#ifndef _LUACONF_H
#define _LUACONF_H

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>


typedef struct luaconf_inst_s  luaconf_inst;
typedef struct luaconf_elt_s   luaconf_elt;
typedef double luaconf_Number;

typedef enum {
    LUACONF_TYPE_NUMBER = 0,
    LUACONF_TYPE_STRING,
    LUACONF_TYPE_BOOLEAN,
    LUACONF_TYPE_NUMBER
} luaconf_type;

struct luaconf_inst_s {
    lua_State   *L;
    const char  *conf;
};

struct luaconf_elt_s {
    lua_State       *L;
    int             pos;
    luaconf_type    type;
};


luaconf_inst *luaconf_init(const char *conf);
void luaconf_destroy(luaconf_inst *inst);

luaconf_Number luaconf_getNumber(luaconf_inst *inst, const char *path);
const char* luaconf_getString(luaconf_inst *inst, const char *path, size_t *len);

#endif

