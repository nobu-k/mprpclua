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

#include "client.hpp"

#include <msgpack/lua/lua_objects.hpp>

namespace msgpack {
namespace rpc {
namespace lua {
namespace {
// for Client userdata
template<int (Client::*Memfun)(lua_State*, const std::string&, int)>
int clientProxy(lua_State* L) {
  Client* p =
    *static_cast<Client**>(luaL_checkudata(L, 1, Client::MetatableName));
  size_t name_len;
  const char* name = luaL_checklstring(L, 2, &name_len);
  return (p->*Memfun)(L, std::string(name, name_len), 3);
}

// for wrapper(SyncClient, AsyncClient)
int callProxy(lua_State* L) {
  // get metatable of the table and get Client* from it
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_getmetatable(L, 1);
  lua_pushstring(L, "client");
  lua_gettable(L, -2);

  Client* p =
    *static_cast<Client**>(luaL_checkudata(L, -1, Client::MetatableName));
  lua_pop(L, 2); // pop metatable and Client*

  // get name of the target function
  size_t name_len;
  const char* name = luaL_checklstring(L, lua_upvalueindex(1), &name_len);
  return p->call(L, std::string(name, name_len), 2);
}

int createCallProxy(lua_State* L) {
  // (table, key)
  luaL_checktype(L, 1, LUA_TTABLE);
  luaL_checktype(L, 2, LUA_TSTRING);

  // create upvalues
  lua_pushvalue(L, 2); // use key as an upvalue
  lua_pushcclosure(L, &callProxy, 1);

  // set closure to client table
  lua_pushvalue(L, 2); // key
  lua_pushvalue(L, -2); // closure
  lua_rawset(L, 1);
  return 1;
}
} // namespace

const char* const Client::MetatableName = "msgpackrpc.Client";

void Client::registerUserdata(lua_State* L) {
  if (luaL_newmetatable(L, Client::MetatableName) == 0) {
    lua_pop(L, 1);
    return; // already created
  }

  // metatable.__index = metatable
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  // set __gc
  lua_pushcfunction(L, &Client::finalizer);
  lua_setfield(L, -2, "__gc");

  // register methods
  const struct luaL_Reg Methods[] = {
    {"call", &clientProxy<&Client::callSync>},
    {"call_async", &clientProxy<&Client::callAsync>},
    {NULL, NULL}
  };
  luaL_register(L, NULL, Methods);
  lua_pop(L, 1);
}

int Client::createUserdata(lua_State* L, bool sync) {
  size_t host_len;
  const char* host = luaL_checklstring(L, 1, &host_len);
  int port = luaL_checkint(L, 2);

  // metatable["client"] = new Client
  Client** p = static_cast<Client**>(lua_newuserdata(L, sizeof(Client*)));
  luaL_getmetatable(L, Client::MetatableName);
  lua_setmetatable(L, -2);
  *p = new Client(sync, std::string(host, host_len), port);
  return 1;
}

int Client::create(lua_State* L) {
  return createUserdata(L, true); // sync call is default
}

int Client::create(lua_State* L, bool sync) {
  lua_newtable(L);

  // create metatable
  lua_newtable(L);
  lua_pushcfunction(L, &createCallProxy);
  lua_setfield(L, -2, "__index");

  // metatable["client"] = new Client
  createUserdata(L, sync);
  lua_setfield(L, -2, "client");

  lua_setmetatable(L, -2);
  return 1;
}

int Client::finalizer(lua_State* L) {
  Client* p =
    *static_cast<Client**>(luaL_checkudata(L, -1, Client::MetatableName));
  delete p;
  return 0;
}

Client::Client(bool sync, const std::string& host, int port)
  : client(host, port), sync_(sync) {
}

Client::~Client() {
}

int Client::call(lua_State* L, const std::string& name, int arg_base) {
  return call(L, sync_, name, arg_base);
}

int Client::call(lua_State* L, bool sync, const std::string& name, int arg_base) {
  msgpack::lua::LuaObjects args(L, arg_base, true);

  shared_zone slife;
  future f = send_request(name, args, slife);
  f.join();

  msgpack::lua::LuaObjects res(L);
  res.msgpack_unpack(f.result());
  return 1;
}

int Client::callSync(lua_State* L, const std::string& name, int arg_base) {
  return call(L, true, name, arg_base);
}

int Client::callAsync(lua_State* L, const std::string& name, int arg_base) {
  return call(L, false, name, arg_base);
}

} // namespace lua
} // namespace rpc
} // namespace msgpack
