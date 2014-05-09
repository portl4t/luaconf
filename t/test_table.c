
/*

-- table.lua

site = {
    ['domain'] = 'www.foo.com',
    ['level'] = 49
}

*/

#include "luaconf.h"


int main(int argc, char *argv[])
{
    long            i, n, cnt;
    size_t          size;
    luaconf_inst    *inst;
    luaconf_elt     *base;
    const char      *name;
    size_t          len;
    char            buf[1024];
    luaconf_elt     *vec[256];

    inst = luaconf_init("table.lua");
    base = luaconf_getElt(inst, "site", sizeof("site")-1);

    luaconf_getEltSubElts(base, vec, 256, &cnt);        // luaconf_getSubElts(inst, "site", sizeof("base")-1, vec, 256, &cnt)

    for (i = 0; i < cnt; i++) {

        name = luaconf_getEltName(vec[i], &len);
        printf("key = %.*s, val = ", len, name);

        if (luaconf_isNumber(vec[i])) {
            n = luaconf_getNumber(vec[i]);
            printf("%d\n", n);

        } else if (luaconf_isString(vec[i])){
            luaconf_getStr(vec[i], buf, 1024, &size);
            printf("%.*s\n", size, buf);
        }
    }

    for (i = 0; i < cnt; i++) {
        luaconf_freeElt(vec[i]);
    }

    luaconf_freeElt(base);
    luaconf_destroy(inst);

    return 0;
}

/*

key = domain, val = www.foo.com
key = level, val = 49

*/

