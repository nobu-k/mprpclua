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

#ifndef MSGPACK_RPC_LUA_FUTURE_HPP_
#define MSGPACK_RPC_LUA_FUTURE_HPP_

#include <lua.hpp>
#include <msgpack/rpc/future.h>

namespace msgpack {
namespace rpc {
namespace lua {

class LuaFuture {
public:
  static const char* const MetatableName;
  static void registerUserdata(lua_State* L);
  static int create(lua_State* L, const future& f);

private:
  static int finalizer(lua_State* L);

public:
  LuaFuture(const future& f);

  int get(lua_State* L);

private:
  future result_;
};

} // namespace lua
} // namespace rpc
} // namespace msgpack

#endif
