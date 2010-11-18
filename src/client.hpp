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
  Client(bool sync, const std::string& host, int port);
  Client(const Client&);
  Client& operator =(const Client&);

public:
  static const char* const MetatableName;
  static void registerUserdata(lua_State* L);

  /**
   * @brief create a rpc client
   */
  static int create(lua_State* L);

  /**
   * @brief Create a sync or async rpc client
   *
   * This function actually creates a table which has no entry.
   * Instead, the table has Client* and custom __index in its metatable.
   * Each time users access to table, __index creates a new
   * closure which calls Client::call.
   */
  static int create(lua_State* L, bool sync);

private:
  static int createUserdata(lua_State* L, bool sync);
  static int finalizer(lua_State* L);

public:
  ~Client();

  /**
   * @brief call remote method
   *
   * @param L lua_State* of this context
   * @param name name of the method to call
   * @param arg_base the index in the stack which points to the first argument
   *
   * This function switches sync/async call according to the value of sync_.
   */
  int call(lua_State* L, const std::string& name, int arg_base);

  /**
   * @brief call remote method
   *
   * @param L lua_State* of this context
   * @param sync sync/async switch
   * @param name name of the method to call
   * @param arg_base the index in the stack which points to the first argument
   */
  int call(lua_State* L, bool sync, const std::string& name, int arg_base);

  /**
   * @param L lua_State* of this context
   * @param name name of the method to call
   * @param arg_base the index in the stack which points to the first argument
   */
  int callSync(lua_State* L, const std::string& name, int arg_base);

  /**
   * @param L lua_State* of this context
   * @param name name of the method to call
   * @param arg_base the index in the stack which points to the first argument
   */
  int callAsync(lua_State* L, const std::string& name, int arg_base);

private:
  bool sync_;
};

} // namespace lua
} // namespace rpc
} // namespace msgpack

#endif
