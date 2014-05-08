
#include "luaconf.h"


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


luaconf_elt *
luaconf_getElts(luaconf_inst *inst, luaconf_elt *elt, const char *path, size_t *n)
{
    *n = 0;

    if (elt) {
        if (elt->type != LUACONF_TYPE_TABLE)
            return NULL;

    } else {
    }
}

