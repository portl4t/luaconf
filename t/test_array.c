
/*

-- array.lua

site = {
    ['domain_list'] = {
            'www.google.com',
            'www.facebook.com',
            'www.apple.com',
            'www.microsoft.com',
            'www.amazon.com',
    },
 
    ['level'] = 796
}

*/

#include "luaconf.h"


int main(int argc, char *argv[])
{
    long            i, n, cnt;
    size_t          size;
    luaconf_inst    *inst;
    luaconf_elt     *base;
    char            buf[1024];
    luaconf_elt     *vec[256];

    inst = luaconf_init("array.lua");
    base = luaconf_getElt(inst, "site.domain_list", sizeof("site.domain_list") - 1);

    luaconf_getEltSubElts(base, vec, 256, &cnt);

    for (i = 0; i < cnt; i++) {
        luaconf_getStr(vec[i], buf, 1024, &size);
        printf("%.*s\n", size, buf);
    }

    for (i = 0; i < cnt; i++) {
        luaconf_freeElt(vec[i]);
    }

    luaconf_freeElt(base);
    luaconf_destroy(inst);

    return 0;
}

