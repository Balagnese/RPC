
all:
	midl hello.idl
	cl helloc.c hello_c.c /link /out:client.exe rpcrt4.lib
	cl hellos.c hello_s.c hellop.c /link /out:server.exe rpcrt4.lib