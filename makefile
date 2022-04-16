all: build run


build:
	cd com && midl chat_rpc.idl
	cd server && $(MAKE) all
	cd client && $(MAKE) all

run:
	start cmd /k server.exe
	start cmd /k client.exe

clean:
	del *.exe
	del *.obj
	cd com && del *.obj
	cd server && $(MAKE) clean
	cd client && $(MAKE) clean

	