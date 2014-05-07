
#include "luaconf.h"


static luaconf_elt * luaconf_get(lua_State *L, const char *start, const char *end);


luaconf_inst *
luaconf_init(const char *conf)
{
    luaconf_inst    *inst;
    lua_State       *L;

    inst = NULL;
    L = luaL_newstate();

    if (L == NULL) {
        return NULL;
    }

    if (luaL_loadfile(L, conf->script)) {
        fprintf(stderr, "luaL_loadfile %s failed: %s\n",
                            conf, lua_tostring(L, -1));
        lua_pop(L, 1);
        goto failed;
    }

    if (lua_pcall(L, 0, 0, 0)) {
        fprintf(stderr, "lua_pcall %s failed: %s\n",
                            conf->script, lua_tostring(L, -1));
        lua_pop(L, 1);
        goto failed;
    }

    inst = (luaconf_inst*)malloc(sizeof(luaconf_inst));
    inst->L = L;
    inst->conf = conf;

    return inst;

failed:

    if (L) {
        lua_close(L);
    }

    if (inst) {
        free(inst);
    }

    return NULL;
}

void
luaconf_destroy(luaconf_inst *inst)
{
    if (inst == NULL) 
        return;

    if (inst->L) {
        lua_close(inst->L);
    }

    free(inst);
}

void
luaconf_freeElt(luaconf_elt *elt)
{
    int         top;
    lua_State   *L;

    L = elt->L;
    top = lua_gettop(L);

    if (top == elt->pos) {
        lua_pop(L, -1);

        top  = lua_gettop(L);
        while (top > 0 && lua_isnil(L, -1)) {
            lua_pop(L, -1);
        }

    } else {
        lua_pushnil(L);
        lua_replace(L, elt->pos);
    }

    free(elt);
}

luaconf_Number
luaconf_getNumber(luaconf_elt *elt)
{
    luaconf_Number  num;

    LUACONF_ASSERT(elt && elt->type == LUACONF_TYPE_NUMBER);

    num = lua_tonumber(elt->L, elt->pos);

    return num;
}

size_t
luaconf_getStrLen(luaconf_elt *elt)
{
    LUACONF_ASSERT(elt && elt->type == LUACONF_TYPE_STRING);

    return lua_objlen(elt->L, elt->pos);
}

char *
luaconf_getStr(luaconf_elt *elt, char *buf, size_t size, size_t *len)
{
    const char  *var;
    size_t      var_len;

    LUACONF_ASSERT(elt && elt->type == LUACONF_TYPE_STRING);

    var = lua_tolstring(elt->L, elt->pos, &var_len);

    if (size >= var_len) {
        *len = var_len;
        memcpy(buf, var, var_len);
        return buf;
    }

    return NULL;
}

luaconf_elt *
luaconf_getElt(luaconf_inst *inst, const char *path, size_t path_len)
{
    int             n;
    lua_State       *L;
    luaconf_elt     *new_elt;
    const char      *begin, *end, *ptr;

    L = inst->L;
    n = lua_gettop(L);
    new_elt = NULL;

    lua_pushvalue(L, LUA_GLOBALSINDEX);     // push global table

    if (path && path_len > 0) {
        begin = path;
        ptr = path;
        end = ptr + path_len;

        while (begin < end && !isalpha(*begin))
            begin++;

        while (begin < end && !isalpha(*(end-1)))
            end--;

        if (begin >= end)
            goto nbrch;

        return luaconf_get(L, start, end);
    }

nbrch:

    new_elt = (luaconf_elt*)malloc(sizeof(luaconf_elt));
    if (!new_elt) {
        lua_pop(L, 1);      // pop global table
        return NULL;
    }

    new_elt->pos = n - 1;
    new_elt->type = LUACONF_TYPE_TABLE;     // global table
    new_elt->vname[0] = 0;

    return new_elt;
}

luaconf_elt *
luaconf_getEltElt(luaconf_elt *elt, const char *path, size_t path_len)
{
    int         n;
    const char  *begin, *end, *ptr;

    n = lua_gettop(L);

    lua_pushvalue(elt->L, elt->pos);

    if (path && path_len > 0) {
        begin = path;
        ptr = path;
        end = ptr + path_len;

        while (begin < end && !isalpha(*begin))
            begin++;

        while (begin < end && !isalpha(*(end-1)))
            end--;

        if (begin >= end)
            goto nbrch;

        return luaconf_get(L, start, end);
    }

nbrch:

    new_elt = (luaconf_elt*)malloc(sizeof(luaconf_elt));
    if (!new_elt) {
        lua_pop(L, 1);      // pop table
        return NULL;
    }

    new_elt->pos = n - 1;
    new_elt->type = LUACONF_TYPE_TABLE;     // global table
    new_elt->vname[0] = 0;

    return new_elt;
}

static luaconf_elt *
luaconf_get(lua_State *L, const char *begin, const char *end)
{
    int             n;
    const char      *ptr, *start;
    luaconf_elt     *new_elt;

    n = lua_gettop(L);
    new_elt = NULL;

    start = begin;
    ptr = begin;

    do {
        ptr++;

        if (*ptr == '.') {
            if (ptr > start) {
                lua_pushlstring(L, start, ptr-start);
                lua_gettable(L, -2);
                if (!lua_istable(L, -1)) {        // not table, has no member at all
                    goto failed;
                }
            }

            ptr++;
            start = ptr;
        }
    } while (ptr < end);

    if (ptr > start) {
        lua_pushlstring(L, start, ptr-start);
        lua_gettable(L, -2);
        if (lua_isnil(L, -1))
            goto failed;

        new_elt = (luaconf_elt*)malloc(sizeof(luaconf_elt));
        if (!new_elt)
            goto failed;

        lua_remove(L, -2);                  // remomve global table
        new_elt->type = lua_type(L, -1);
        new_elt->pos = lua_gettop(L);
    }

    new_elt->vname[0] = 0;
    new_elt->L = L;

    return new_elt;

failed:

    if (new_elt)
        free(new_elt);

    lua_settop(n);

    return NULL;
}

