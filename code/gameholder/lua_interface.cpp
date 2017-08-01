/*
** Lua binding: interface
** Generated automatically by tolua++-1.0.92 on 04/07/16 22:28:11.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_interface_open (lua_State* tolua_S);

#include "lua_function.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
}

/* function: GetFormulaParam */
#ifndef TOLUA_DISABLE_tolua_interface_GetFormulaParam00
static int tolua_interface_GetFormulaParam00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* funcName = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   float tolua_ret = (float)  GetFormulaParam(funcName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFormulaParam'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_interface_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_function(tolua_S,"GetFormulaParam",tolua_interface_GetFormulaParam00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_interface (lua_State* tolua_S) {
 return tolua_interface_open(tolua_S);
};
#endif

