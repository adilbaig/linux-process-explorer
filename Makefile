BUILD_PARAMS = -std=c++17 src/lpe/* src/ext/*.hpp src/ext/nlohmann/*
GUI_BUILD_PARAMS = src/window/* `pkg-config gtkmm-3.0 --cflags --libs`

build:
	g++ src/main.cpp ${BUILD_PARAMS} ${GUI_BUILD_PARAMS} -obin/lpe

app: build

debug-build:
	g++ src/main.cpp ${BUILD_PARAMS} ${GUI_BUILD_PARAMS} -DDEBUG -g -obin/lpe-debug

# gui:
# 	g++ src/main-app.cpp ${BUILD_PARAMS} ${GUI_BUILD_PARAMS} -obin/lpe-gui

start-webserver:
	python3 -m http.server --bind=localhost --directory=web
