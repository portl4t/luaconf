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

This function will retrieve the element according to the string `path`. Upon successful a `luaconf_elt` pointer is returned. Otherwise, NULL is returned.

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

### luaconf_getEltElt
**syntax:** luaconf_elt * luaconf_getEltElt(luaconf_elt *base, const char *path, size_t path_len);

This function will retrieve the element according to the string `path` from `base`.  Upon successful a `luaconf_elt` pointer is returned. Otherwise, NULL is returned.

```c
/*
   site = {
        ['domain'] = 'www.taobao.com',
        ['level'] = 5
   }
*/
luaconf_elt *base = luaconf_getElt(inst, "site", sizeof("site") - 1);
luaconf_elt *elt = luaconf_getEltElt(base, "domain", sizeof("domain") - 1);
```

### luaconf_freeElt
**syntax:** void luaconf_freeElt(luaconf_elt *elt);

This function will destroy the resources owed by `elt` pointer. After we get the real data from `elt`, we should use this function to release the resource.


### luaconf_isXXX
- int luaconf_isNumber(luaconf_elt *elt)
- int luaconf_isString(luaconf_elt *elt)
- int luaconf_isBool(luaconf_elt *elt)
- int luaconf_isTable(luaconf_elt *elt)

These functions can be used to test whether `elt` has XXX type.

### luaconf_getNumber
**syntax:** double luaconf_getNumber(luaconf_elt *elt);

Converts the value represent by `elt` to the C double type.

### luaconf_getBool
**syntax:** int luaconf_getBool(luaconf_elt *elt);

Converts the value represent by `elt` to the C boolean type(0 or 1).

### luaconf_getStr
**syntax:** char * luaconf_getStr(luaconf_elt *elt, char *buf, size_t size, size_t *len);

Converts the value represent by `elt` to the C string. If len is not NULL, it also sets *len with the string length. 

### luaconf_getStrLen
**syntax:** size_t luaconf_getStrLen(luaconf_elt *elt);

Returns the length of the string which is represented by `elt`.

### luaconf_getSubEltCnt
**syntax:** size_t luaconf_getSubEltCnt(luaconf_inst *inst, const char *path, size_t path_len);

Returns the count of the sub element under the `path`.

```c
/*
   site = {
        ['domain'] = 'www.taobao.com',
        ['level'] = 5
   }
*/
int n = luaconf_getSubEltCnt(inst, NULL, 0);                                // 1    site
int m = luaconf_getSubEltCnt(inst, "site", sizeof("site")-1);               // 2    domain, level
int p = luaconf_getSubEltCnt(inst, "site.domain", sizeof("site.domain")-1); // 0
```

### luaconf_getSubElts
**syntax:** luaconf_elt ** luaconf_getSubElts(luaconf_inst *inst, const char *path, size_t path_len, luaconf_elt **vec, size_t size, size_t *n);

Retrieves all the sub elements under the `path` and fills them in the parameter `vec`. It also sets *n with the count of the sub elments.

### luaconf_getEltSubEltCnt
**syntax:** size_t luaconf_getStrLen(luaconf_elt *elt);
### luaconf_getEltSubElts
**syntax:** size_t luaconf_getStrLen(luaconf_elt *elt);

### luaconf_getEltName
**syntax:** const char * luaconf_getEltName(luaconf_elt *elt, size_t *size);

Return the name string of `elt` and set *size to the length of the string. The string memory will be reclaimed if the elt has been freed.


