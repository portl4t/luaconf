
/*

   -- conf.lua

   site1 = {
       ['domain'] = 'www.taobao.com',
       ['level'] = 6,
   }

*/

#include "luaconf.h"


int main(int argc, char *argv[])
{
    long            n;
    size_t          size;
    luaconf_inst    *inst;
    luaconf_elt     *elt1;
    luaconf_elt     *elt2;
    char            buf[1024];

    inst = luaconf_init("conf.lua");
    elt1 = luaconf_getElt(inst, "site1.domain", sizeof("site1.domain")-1);
    elt2 = luaconf_getElt(inst, "site1.level", sizeof("site1.level")-1);

    if (elt1) {
        luaconf_getStr(elt1, buf, sizeof(buf), &size);
        printf("site1.domain = %.*s\n", size, buf);
    }

    if (elt2) {
        n = luaconf_getNumber(elt2);
        printf("site1.level = %d\n", n);
    }

    luaconf_freeElt(elt1);
    luaconf_freeElt(elt2);
    luaconf_destroy(inst);

    return 0;
}

