
/*

   -- deep.lua

   site1 = {
       ['domain'] = 'www.taobao.com',
       ['level'] = 6,
       ['sub'] = {
                    ['prefix'] = 'http',
                    ['port'] = 7799
                 }
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

    inst = luaconf_init("deep.lua");

    elt1 = luaconf_getElt(inst, "site1", sizeof("site1"));

    elt2 = luaconf_getEltElt(elt1, "sub.prefix", sizeof("sub.prefix")-1);

    luaconf_getStr(elt2, buf, sizeof(buf), &size);

    luaconf_freeElt(elt1);
    luaconf_freeElt(elt2);
    luaconf_destroy(inst);

    return 0;
}

