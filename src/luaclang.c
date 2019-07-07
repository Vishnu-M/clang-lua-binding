#include <clang-c/Index.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#define INDEX  "ClangIndex"
#define TU     "ClangTU"
#define CURSOR "ClangCursor"
#define TYPE "ClangType"

static CXIndex* new_CXIndex(lua_State *L) 
{
        CXIndex *idx = (CXIndex*) lua_newuserdata(L, sizeof(CXIndex));
        luaL_getmetatable(L, INDEX);
        lua_setmetatable(L, -2);
        return idx;
}

static CXIndex to_CXIndex(lua_State *L, int n)
{
        CXIndex* idx = (CXIndex*) luaL_checkudata(L, n, INDEX);
        return *idx;
}

static CXTranslationUnit* new_CXTU(lua_State *L)
{
        CXTranslationUnit* tu = (CXTranslationUnit*) lua_newuserdata(L, sizeof(CXTranslationUnit));
        luaL_getmetatable(L, TU);
        lua_setmetatable(L, -2);
        return tu;
}

static CXTranslationUnit to_CXTU(lua_State *L, int n) 
{
        CXTranslationUnit* tu = (CXTranslationUnit*) luaL_checkudata(L, n, TU);
        return *tu;
}

static CXCursor* new_CXCursor(lua_State *L) 
{
        CXCursor* cur = (CXCursor*) lua_newuserdata(L, sizeof(CXCursor));
        luaL_getmetatable(L, CURSOR);
        lua_setmetatable(L, -2);
        return cur;
}

static CXCursor to_CXCursor(lua_State *L, int n) 
{
        CXCursor* c = (CXCursor*) luaL_checkudata(L, n, CURSOR);
        return *c;
}

static CXType* new_CXType(lua_State *L)
{
        CXType* t = (CXType*) lua_newuserdata(L, sizeof(CXType));
        luaL_getmetatable(L, TYPE);
        lua_setmetatable(L, -2);
        return t;
}


/* Clang function */

static int create_CXIndex(lua_State *L) 
{
        int exclude_pch = lua_toboolean(L, 1);
        int diagnostics = lua_toboolean(L, 2);
        CXIndex* idx = new_CXIndex(L);
        *idx = clang_createIndex(exclude_pch, diagnostics);
        return 1;
}


/* Index functions */

static int dispose_CXIndex(lua_State *L) 
{
        CXIndex idx = to_CXIndex(L, 1);
        clang_disposeIndex(idx);
        return 0;
}

static int parse_TU(lua_State *L)
{
        CXIndex idx = to_CXIndex(L, 1);
        const char* file_name = lua_tostring(L, 2);
        const char* args[] = {file_name};
        CXTranslationUnit *tu = new_CXTU(L);
        *tu = clang_parseTranslationUnit(idx, 0, args, 1, 0, 0, CXTranslationUnit_None);
        return 1;
}

static luaL_Reg index_functions[] = {
        {"disposeIndex", dispose_CXIndex},
        {"parseTU", parse_TU},
        {NULL, NULL}
};


/* Translation unit functions */

static int dispose_CXTU(lua_State *L) 
{
        CXTranslationUnit tu = to_CXTU(L, 1);
        clang_disposeTranslationUnit(tu);
        return 0;
}

static int get_TU_cursor(lua_State *L) 
{
        CXTranslationUnit tu = to_CXTU(L, 1);
        CXCursor* cur = new_CXCursor(L);
        *cur = clang_getTranslationUnitCursor(tu);
        if (clang_Cursor_isNull(*cur)) {
                lua_pushnil(L);
        }
        return 1;
}

static luaL_Reg tu_functions[] = {
        {"disposeTU", dispose_CXTU},
        {"getTUCursor", get_TU_cursor},
        {NULL, NULL}
};

void new_metatable(lua_State *L, const char * name, luaL_Reg *reg) {
        luaL_newmetatable(L, name);
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
}

static const struct luaL_Reg luaclang [] = {
        {"createIndex", create_CXIndex},
        {"parseTU", parse_TU}, 
        {"getTUCursor", get_TU_cursor},
        {"disposeTU", dispose_CXTU},
        {"disposeIndex", dispose_CXIndex},
        {NULL, NULL}  
};

int luaopen_luaclang(lua_State *L) 
{
        new_metatable(L, INDEX, index_functions);
        new_metatable(L, TU, tu_functions);

        luaL_newlib(L, luaclang);
        return 1;
}
