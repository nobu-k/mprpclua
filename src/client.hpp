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

#ifndef MSGPACK_RPC_LUA_CLIENT_HPP_
#define MSGPACK_RPC_LUA_CLIENT_HPP_

#include <lua.hpp>
#include <msgpack/rpc/client.h>

namespace msgpack {
namespace rpc {
namespace lua {

class Client : public ::msgpack::rpc::client {
private:
  Client(const std::string& host, int port);
  Client(const Client&);
  Client& operator =(const Client&);

public:
  static const char* const MetatableName;
  static void registerUserdata(lua_State* L);

  /**
   * @brief create a rpc client
   *
   * This function actually creates a table which has no entry.
   * Instead, the table has Client* and custom __index in its metatable.
   * Each time users access to table, __index creates a new
   * closure which calls Client::call.
   */
  static int create(lua_State* L);

private:
  static int finalizer(lua_State* L);

public:
  ~Client();

  int call(lua_State* L, const std::string& name);
};

} // namespace lua
} // namespace rpc
} // namespace msgpack

#endif
