
#include "libluaconf.h"


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

    if (luaL_loadfile(L, conf)) {
        fprintf(stderr, "luaL_loadfile %s failed: %s\n",
                            conf, lua_tostring(L, -1));
        lua_pop(L, 1);
        goto failed;
    }

    if (lua_pcall(L, 0, 0, 0)) {
        fprintf(stderr, "lua_pcall %s failed: %s\n",
                            conf, lua_tostring(L, -1));
        lua_pop(L, 1);
        goto failed;
    }

    inst = (luaconf_inst*)malloc(sizeof(luaconf_inst));
    if (inst == NULL)
        goto failed;

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

    if (top == elt->pos) {          // at top
        lua_pop(L, 1);

        while (lua_gettop(L) > 0 && lua_isnil(L, -1))
            lua_pop(L, 1);

    } else {
        lua_pushnil(L);
        lua_replace(L, elt->pos);
    }

    free(elt);
}

const char *
luaconf_getEltName(luaconf_elt *elt, size_t *size)
{
    LUACONF_ASSERT(elt);

    *size = elt->vsize;
    return elt->vname;
}

luaconf_Number
luaconf_getNumber(luaconf_elt *elt)
{
    luaconf_Number  num;

    LUACONF_ASSERT(elt && elt->type == LUACONF_TYPE_NUMBER);

    num = lua_tonumber(elt->L, elt->pos);

    return num;
}

int
luaconf_getBool(luaconf_elt *elt)
{
    int     res;

    LUACONF_ASSERT(elt && elt->type == LUACONF_TYPE_BOOL);

    res = lua_toboolean(elt->L, elt->pos);

    return res;
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
    size_t      var_len, need;

    LUACONF_ASSERT(elt && elt->type == LUACONF_TYPE_STRING);

    var = lua_tolstring(elt->L, elt->pos, &var_len);

    need = var_len > size ? size : var_len;

    memcpy(buf, var, need);
    *len = need;

    return buf;
}

luaconf_elt *
luaconf_getElt(luaconf_inst *inst, const char *path, size_t path_len)
{
    lua_State       *L;
    luaconf_elt     *new_elt;
    const char      *begin, *end, *ptr;

    L = inst->L;
    new_elt = NULL;

    lua_pushvalue(L, LUA_GLOBALSINDEX);     // push global table

    if (path && path_len > 0) {
        begin = path;
        ptr = path;
        end = ptr + path_len;

        while (begin < end && !(isalnum(*begin) || *begin == '_'))
            begin++;

        while (begin < end && !(isalnum(*(end-1)) || *(end-1) == '_'))
            end--;

        if (begin >= end)
            goto nbrch;

        return luaconf_get(L, begin, end);
    }

nbrch:

    new_elt = (luaconf_elt*)malloc(sizeof(luaconf_elt));
    if (!new_elt) {
        lua_pop(L, 1);      // pop global table
        return NULL;
    }

    new_elt->pos = lua_gettop(L);
    new_elt->type = LUACONF_TYPE_TABLE;     // global table
    new_elt->vname[0] = 0;
    new_elt->vsize = 0;
    new_elt->L = L;

    return new_elt;
}

luaconf_elt *
luaconf_getEltElt(luaconf_elt *elt, const char *path, size_t path_len)
{
    lua_State   *L;
    const char  *begin, *end, *ptr;
    luaconf_elt *new_elt;

    L = elt->L;

    lua_pushvalue(L, elt->pos);

    if (path && path_len > 0) {
        begin = path;
        ptr = path;
        end = ptr + path_len;

        while (begin < end && !(isalnum(*begin) || *begin == '_'))
            begin++;

        while (begin < end && !(isalnum(*(end-1)) || *(end-1) == '_'))
            end--;

        if (begin >= end)
            goto nbrch;

        return luaconf_get(L, begin, end);
    }

nbrch:

    new_elt = (luaconf_elt*)malloc(sizeof(luaconf_elt));
    if (!new_elt) {
        lua_pop(L, 1);      // pop table
        return NULL;
    }

    new_elt->pos = lua_gettop(L);
    new_elt->type = LUACONF_TYPE_TABLE;     // table
    new_elt->vname[0] = 0;
    new_elt->vsize = 0;

    return new_elt;
}

static luaconf_elt *
luaconf_get(lua_State *L, const char *begin, const char *end)
{
    int             n;
    const char      *ptr, *start;
    luaconf_elt     *new_elt;

    /* there was already a table on the stack */

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
                if (!lua_istable(L, -1)) {          // not table, has no member at all
                    goto failed;
                }

                lua_remove(L, -2);                  // remomve previous table
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

        lua_remove(L, -2);                          // remomve previous table

        new_elt->type = lua_type(L, -1);
        new_elt->pos = lua_gettop(L);
        new_elt->L = L;

        new_elt->vsize = snprintf(new_elt->vname, sizeof(new_elt->vname), "%.*s", (int)(ptr-start), start);

        return new_elt;
    }

failed:

    if (new_elt)
        free(new_elt);

    lua_settop(L, n-1);     // pop the base table out

    return NULL;
}

size_t
luaconf_getEltSubEltCnt(luaconf_elt *elt)
{
    LUACONF_ASSERT(elt && elt->type == LUACONF_TYPE_TABLE);
    return lua_objlen(elt->L, elt->pos);
}

luaconf_elt **
luaconf_getEltSubElts(luaconf_elt *elt, luaconf_elt **vec, size_t size, size_t *n)
{
    int                 i, tb, cnt;
    const char          *key;
    size_t              key_len;
    lua_State           *L;
    luaconf_elt         *new_elt;

    LUACONF_ASSERT(elt && elt->type == LUACONF_TYPE_TABLE);

    L = elt->L;
    tb = elt->pos;

    i = 0;
    cnt = lua_objlen(L, tb);

    lua_rawgeti (L, tb, 1);

    if (!lua_isnil(L, -1)) {

        /* array */
        lua_pop(L, 1);      // pop the 1st out.

        i = 1;
        cnt = cnt <= size ? cnt : size;

        for (; i <= cnt; i++) {
            lua_rawgeti (L, tb, i);

            if (lua_isnil(L, -1)) {
                vec[i-1] = NULL;
                lua_pop(L, 1);      // pop the nil out.

            } else {
                new_elt = (luaconf_elt*)malloc(sizeof(luaconf_elt));
                if (new_elt == NULL) {
                    lua_pop(L, 1);          // pop the value out
                    break;
                }

                new_elt->L = L;
                new_elt->pos = lua_gettop(L);
                new_elt->type = lua_type(L, new_elt->pos);

                *(unsigned int*)(new_elt->vname) = i - 1;
                new_elt->vsize = sizeof(unsigned int);
                vec[i - 1] = new_elt;
            }
        }

        *n = i - 1;

    } else {

        /* key - value table */

        while (lua_next(L, tb)) {

            if (i >= size) {
                lua_pop(L, 2);      // pop the value and key.
                break;
            }

            lua_insert(L, -2);      // exchange key and value

            key = lua_tolstring(L, -1, &key_len);

            new_elt = (luaconf_elt*)malloc(sizeof(luaconf_elt));
            if (new_elt == NULL) {
                lua_pop(L, 2);
                break;
            }

            new_elt->L = L;
            new_elt->pos = lua_gettop(L) - 1;
            new_elt->type = lua_type(L, new_elt->pos);

            new_elt->vsize = snprintf(new_elt->vname, sizeof(new_elt->vname), "%.*s", (int)key_len, key);
            vec[i++] = new_elt;
        }

        *n = i;
    }


    return vec;
}

size_t
luaconf_getSubEltCnt(luaconf_inst *inst, const char *path, size_t path_len)
{
    size_t          n;
    luaconf_elt     *elt;

    elt = luaconf_getElt(inst, path, path_len);

    if (!elt) {
        return 0;
    }

    LUACONF_ASSERT(elt && elt->type == LUACONF_TYPE_TABLE);

    n = luaconf_getEltSubEltCnt(elt);

    luaconf_freeElt(elt);

    return n;
}

luaconf_elt **
luaconf_getSubElts(luaconf_inst *inst, const char *path, size_t path_len, 
                    luaconf_elt **vec, size_t size, size_t *n)
{
    luaconf_elt     *elt;
    luaconf_elt     **addr;

    elt = luaconf_getElt(inst, path, path_len);

    if (!elt) {
        *n = 0;
        return vec;
    }

    LUACONF_ASSERT(elt && elt->type == LUACONF_TYPE_TABLE);

    addr = luaconf_getEltSubElts(elt, vec, size, n);

    luaconf_freeElt(elt);

    return addr;
}

