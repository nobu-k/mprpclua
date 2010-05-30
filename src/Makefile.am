pkglibdir = $(LUA_CMOD_DIR)

pkglib_LTLIBRARIES = msgpack-rpc.la

msgpack_rpc_la_SOURCES = \
  msgpack-rpc.cpp
msgpack_rpc_la_CXXFLAGS = \
  $(LUA_CFLAGS)
msgpack_rpc_la_LIBADD = \
  $(LUA_LIBS) -lmsgpack

# Because mplua.la does not start with "lib", module option is necessary.
msgpack_rpc_la_LDFLAGS = \
  -module