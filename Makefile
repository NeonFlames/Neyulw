.PHONY: linux windows

linux-x: pre_build
	CGO_ENABLED=1 GOOS=linux GOARCH=amd64 go build -tags no_metadata -trimpath -ldflags "-s -w" -o bin/Neyulw.X11.x86_64

linux-wl: pre_build
	CGO_ENABLED=1 GOOS=linux GOARCH=amd64 go build -tags no_metadata,wayland -trimpath -ldflags "-s -w" -o bin/Neyulw.wayland.x86_64

windows: pre_build
	CC=x86_64-w64-mingw32-gcc CGO_LDFLAGS="-LC:/mingw/lib -lcomdlg32 -lole32" CGO_ENABLED=1 GOOS=windows GOARCH=amd64 go build -tags no_metadata -trimpath -ldflags "-s -w -H=windowsgui" -o bin/Neyulw.exe

pre_build:
	mkdir -p bin

all: linux-x linux-wl windows
