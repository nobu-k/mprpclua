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

#include <lua.hpp>
#include "client.hpp"

namespace msgpack {
namespace rpc {
namespace lua {

/**
 */
int createClient(lua_State* L) {
  return Client::create(L);
}

/**
 */
int createServer(lua_State* L) {
  // TODO: impl
  return 0;
}

namespace {
const char* const MpRpcLuaPkgName = "msgpackrpc";
const struct luaL_Reg MpRpcLuaLib[] = {
  {"Client", &createClient},
  {"Server", &createServer},
  {NULL, NULL}
};
} // namespace
} // namespace lua
} // namespace rpc
} // namespace msgpack

extern "C" {
  int luaopen_msgpackrpc(lua_State* L) {
    msgpack::rpc::lua::Client::registerUserdata(L);
    luaL_register(L, msgpack::rpc::lua::MpRpcLuaPkgName,
                  msgpack::rpc::lua::MpRpcLuaLib);
    return 1;
  }
}
