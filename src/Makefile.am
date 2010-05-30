pkglibdir = $(LUA_CMOD_DIR)

pkglib_LTLIBRARIES = msgpackrpc.la

msgpackrpc_la_SOURCES = \
  msgpackrpc.cpp
msgpackrpc_la_CXXFLAGS = \
  $(LUA_CFLAGS)
msgpackrpc_la_LIBADD = \
  $(LUA_LIBS) -lmsgpack

# Because mpluarpc.la does not start with "lib", module option is necessary.
msgpackrpc_la_LDFLAGS = \
  -module