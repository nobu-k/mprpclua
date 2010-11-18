lib_LTLIBRARIES = libmsgpack-rpc-lua.la

libmsgpack_rpc_lua_la_SOURCES = \
  msgpackrpc.cpp \
  client.hpp \
  client.cpp
libmsgpack_rpc_lua_la_CXXFLAGS = \
  $(LUA_CFLAGS)
libmsgpack_rpc_lua_la_LIBADD = \
  -L$(pkglibdir) $(LUA_LIBS) -lmsgpack -lmsgpack-rpc -lmsgpack-lua

install-exec-hook:
	ln -f -s $(libdir)/libmsgpack-rpc-lua.so $(LUA_CMOD_DIR)/msgpackrpc.so
