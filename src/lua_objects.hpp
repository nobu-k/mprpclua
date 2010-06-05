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

#ifndef MSGPACK_RPC_LUA_LUA_OBJECT_HPP_
#define MSGPACK_RPC_LUA_LUA_OBJECT_HPP_

#include <lua.hpp>
#include <msgpack.hpp>

namespace msgpack {
namespace rpc {
namespace lua {

/**
 * @brief Lua Object class for serialization.
 *
 * @note Using this class instead of serializing Lua objects
 * directly may be too slow because of redundant copies
 * of Lua values.
 *
 * @todo remove boost
 * @todo support arrays and tables
 */
class LuaObjects {
public:
  /**
   * @param arg_base This is necessary only when LuaObjects will be serialized
   */
  LuaObjects(lua_State* L, int arg_base = 0);

  template<typename Packer>
  void msgpack_pack(Packer& pk) const {
    int n = lua_gettop(L);
    if (arg_base > n) return;

    // what should I do when there's no arguments
    pk.pack_array(n - arg_base + 1);
    for (int i = arg_base; i <= n; i++) {
      int t = lua_type(L, i);
      switch (t) {
      case LUA_TNUMBER: packNumber(pk, i); break;
      case LUA_TBOOLEAN: packBoolean(pk, i); break;
      case LUA_TSTRING: packString(pk, i); break;
      case LUA_TTABLE: packTable(pk, i); break;
      default:
        luaL_error(L, "invalid type for pack: %s",
                   lua_typename(L, t));
        break;
      }
    }
  }

  void msgpack_unpack(const msgpack::object& msg);

  int unpackedResults() const { return 1; } // always returns 1 currently

private:
  // TODO: merge these with mplua's implementation
  template<typename Packer>
  void pack(Packer& pk, int index) const {
    int t = lua_type(L, index);
    switch (t) {
    case LUA_TNUMBER: packNumber(pk, index); break;
    case LUA_TBOOLEAN: packBoolean(pk, index); break;
    case LUA_TSTRING:  packString(pk, index); break;
    case LUA_TTABLE: packTable(pk, index); break;
    default:
      luaL_error(L, "invalid type for pack: %s",
                 lua_typename(L, t));
      break;
    }
  }

  template<typename Packer>
  void packNumber(Packer& pk, int index) const {
    double n = lua_tonumber(L, index);
    int64_t i = static_cast<int64_t>(n);
    if (i == n) pk.pack(i);
    else pk.pack(n);
  }

  template<typename Packer>
  void packBoolean(Packer& pk, int index) const {
    int b = lua_toboolean(L, index);
    pk.pack(b != 0);
  }

  template<typename Packer>
  void packString(Packer& pk, int index) const {
    size_t len;
    const char* str = lua_tolstring(L, index, &len);
    pk.pack_raw(len);
    pk.pack_raw_body(str, len);
  }

  template<typename Packer>
  void packTable(Packer& pk, int index) const {
    bool isArray = false;
    size_t len = lua_objlen(L, index);
    if (len > 0) {
      lua_pushnumber(L, len);
      if (lua_next(L, index) == 0) isArray = true;
      else lua_pop(L, 2);
    }

    if (isArray) packTableAsArray(pk, index);
    else packTableAsTable(pk, index);
  }

  template<typename Packer>
  void packTableAsTable(Packer& pk, int index) const {
    size_t len = 0;
    lua_pushnil(L);
    while (lua_next(L, index) != 0) {
      len++; lua_pop(L, 1);
    }
    pk.pack_map(len);

    int n = lua_gettop(L); // used as a positive index
    lua_pushnil(L);
    while (lua_next(L, index) != 0) {  
      pack(pk, n + 1); // -2:key
      pack(pk, n + 2); // -1:value
      lua_pop(L, 1); // removes value, keeps key for next iteration
    }
  }

  template<typename Packer>
  void packTableAsArray(Packer& pk, int index) const {
    int n = lua_gettop(L);
    size_t len = lua_objlen(L, index);

    pk.pack_array(len);
    for (size_t i = 1; i <= len; i++) {
      lua_rawgeti(L, index, i);
      pack(pk, n + 1);
      lua_pop(L, 1);
    }
  }

private:
  void unpackArray(const msgpack::object_array& a);
  void unpackTable(const msgpack::object_map& m);

private:
  lua_State* L;
  int arg_base;
};

} // namespace lua
} // namespace rpc
} // namespace msgpack

#endif
