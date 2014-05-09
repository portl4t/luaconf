## Name
luaconf - C library for parsing config file which is written in lua.

## Status
This module has not been released yet.

## Synopsis
```c
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

    luaconf_getStr(elt1, buf, sizeof(buf), &size);
    printf("site1.domain = %.*s\n", size, buf);          // www.taobao.com
    
    n = luaconf_getNumber(elt2);
    printf("site1.level = %d\n", n);                     // 6

    luaconf_freeElt(elt1);
    luaconf_freeElt(elt2);
    luaconf_destroy(inst);

    return 0;
}
```

## System Requirements
* linux/freebsd 32/64bits
* lua-5.1

## Build
**step1**: get luaconf

    git clone https://github.com/portl4t/luaconf.git
    cd luaconf/src

**step2**: build

    make
    make install

## C API

### luaconf_init
**syntax:**  luaconf_inst *luaconf_init(const char *conf);

This function opens the `conf` file which is written in Lua and parse it. Upon successful luaconf_init() returns a `luaconf_inst` pointer. Otherwise, NULL is returned.

```c
luaconf_inst *inst = luaconf_init("/X/conf.lua");
```

### luaconf_destroy
**syntax:**  void luaconf_destroy(luaconf_inst *inst);

This function will destroy the resources owned by `inst` pointer. After we parse the config file, we should use this function to release the resource.


### luaconf_getElt
**syntax:**  luaconf_elt * luaconf_getElt(luaconf_inst *inst, const char *path, size_t path_len);

This function will retrieve the element which is represent by `path`. Upon successful a `luaconf_elt` pointer is returned. Otherwise, NULL is returned.

```c
/*
   site = {
        ['domain'] = 'www.taobao.com',
        ['level'] = 5
   }
*/
luaconf_elt *elt = luaconf_getElt(inst, "site.domain", sizeof("site.domain")-1);
```
`luaconf_elt` has four possible types: number, string, bool, table.

Parameter `path` can be NULL here and the element will be a Table.


### luaconf_freeElt
**syntax:** void luaconf_freeElt(luaconf_elt *elt);

This function will destroy the resources owed by `elt` pointer. After we get the real data from `elt`, we should use this function to release the resouce.


### luaconf_isXXX
- int luaconf_isNumber(luaconf_elt *elt)
- int luaconf_isString(luaconf_elt *elt)
- int luaconf_isBool(luaconf_elt *elt)
- int luaconf_isTable(luaconf_elt *elt)

These functions can be used to test whether `elt` has XXX type.

