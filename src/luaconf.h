
#ifndef _LUACONF_H
#define _LUACONF_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define LUACONF_ASSERT          assert
#define LUACONF_MAX_NAME_LEN    256

#define luaconf_isNumber(elt)   (elt->type == LUACONF_TYPE_NUMBER)
#define luaconf_isString(elt)   (elt->type == LUACONF_TYPE_STRING)
#define luaconf_isBool(elt)     (elt->type == LUACONF_TYPE_BOOL)
#define luaconf_isTable(elt)    (elt->type == LUACONF_TYPE_TABLE)

typedef struct luaconf_inst_s  luaconf_inst;
typedef struct luaconf_elt_s   luaconf_elt;
typedef double luaconf_Number;


typedef enum {
    LUACONF_TYPE_NUMBER = LUA_TNUMBER,
    LUACONF_TYPE_STRING = LUA_TSTRING,
    LUACONF_TYPE_BOOL = LUA_TBOOLEAN,
    LUACONF_TYPE_TABLE = LUA_TTABLE
} luaconf_type;

struct luaconf_inst_s {
    lua_State   *L;
    const char  *conf;
};

struct luaconf_elt_s {
    lua_State       *L;
    char            vname[LUACONF_MAX_NAME_LEN];
    int             vsize;
    int             pos;
    luaconf_type    type;
};


luaconf_inst *luaconf_init(const char *conf);
void luaconf_destroy(luaconf_inst *inst);

size_t luaconf_getSubEltCnt(luaconf_elt *elt);
luaconf_elt ** luaconf_getSubElts(luaconf_elt *elt, luaconf_elt **vec, size_t size, size_t *n);

luaconf_elt * luaconf_getElt(luaconf_inst *inst, const char *path, size_t path_len);
luaconf_elt * luaconf_getEltElt(luaconf_elt *elt, const char *path, size_t path_len);

void luaconf_freeElt(luaconf_elt *elt);

const char * luaconf_getEltName(luaconf_elt *elt, size_t *size);
luaconf_Number luaconf_getNumber(luaconf_elt *elt);
int luaconf_getBool(luaconf_elt *elt);

char * luaconf_getStr(luaconf_elt *elt, char *buf, size_t size, size_t *len);
size_t luaconf_getStrLen(luaconf_elt *elt);

#endif

