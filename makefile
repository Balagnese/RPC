all: build run


build:
	cd com && midl chat_rpc.idl
	cd server && $(MAKE) all
	cd client && $(MAKE) all

run:
	start cmd /k server.exe
	start cmd /k client.exe

clean:
	del client.exe
	del server.exe