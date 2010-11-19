/*
 * MessagePack-RPC for Lua
 *
 * Copyright (C) 2010 Nobuyuki Kubota
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "future.hpp"

#include <msgpack/lua/lua_objects.hpp>

namespace msgpack {
namespace rpc {
namespace lua {
namespace {
template<int (LuaFuture::*Memfun)(lua_State*)>
int futureProxy(lua_State* L) {
  LuaFuture* p =
    *static_cast<LuaFuture**>(luaL_checkudata(L, 1, LuaFuture::MetatableName));
  return (p->*Memfun)(L);
}
} // namespace

const char* const LuaFuture::MetatableName = "msgpackrpc.Future";

void LuaFuture::registerUserdata(lua_State* L) {
  if (luaL_newmetatable(L, LuaFuture::MetatableName) == 0) {
    lua_pop(L, 1);
    return; // already created
  }

  // TODO: merge this setup codes
  // metatable.__index = metatable
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  // set __gc
  lua_pushcfunction(L, &LuaFuture::finalizer);
  lua_setfield(L, -2, "__gc");

  // register methods
  const struct luaL_Reg Methods[] = {
    {"get", &futureProxy<&LuaFuture::get>},
    {NULL, NULL}
  };
  luaL_register(L, NULL, Methods);
  lua_pop(L, 1);
}

int LuaFuture::create(lua_State* L, const future& f) {
  LuaFuture** p = static_cast<LuaFuture**>(lua_newuserdata(L, sizeof(LuaFuture*)));
  luaL_getmetatable(L, LuaFuture::MetatableName);
  lua_setmetatable(L, -2);
  *p = new LuaFuture(f);
  return 1;
}

int LuaFuture::finalizer(lua_State* L) {
  LuaFuture* p =
    *static_cast<LuaFuture**>(luaL_checkudata(L, -1, LuaFuture::MetatableName));
  delete p;
  return 0;
}

LuaFuture::LuaFuture(const future& f) : result_(f) {
}

int LuaFuture::get(lua_State* L) {
  result_.join();
  if (!result_.error().is_nil()) {
    return luaL_error(L, "RPC failed"); // TODO: this code must be replaced with throw std::exception.
  }

  msgpack::lua::LuaObjects res(L);
  res.msgpack_unpack(result_.result());
  return 1;
}

} // namespace lua
} // namespace rpc
} // namespace msgpack
