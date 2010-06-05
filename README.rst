MessagePack-RPC for Lua
=======================

Still writing...

Requirements
============

MessagePack-RPC and liblua 5.1 are required.

Install
=======

Run ./configure && make && make install.

If you cloned this repository from github, you need to run ./autogen.sh first.

Usage
=====

This project is working in progress.
Currently, only the rpc-client interface is designed and
it'll be like below::

  require "msgpackrpc"

  cli = msgpackrpc.Client(host, port)
  future = cli:name_of_method(args)
  value = future.get()

This project may be going to depend on mplua, MessagePack for Lua.
However, mprpclua can currently be compiled and installed without mplua.

I'm afraid but current version is not able to run yet.