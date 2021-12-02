BUILD_PARAMS = -std=c++17 lpe/*
GUI_BUILD_PARAMS = `pkg-config gtkmm-3.0 --cflags --libs`

build:
	g++ main.cpp ${BUILD_PARAMS} -obin/main

debug-build:
	g++ main.cpp ${BUILD_PARAMS} -DDEBUG -g -obin/debug

gui: | bin
	g++ main-app.cpp ${BUILD_PARAMS} window/* ${GUI_BUILD_PARAMS} -obin/app

bin:
	mkdir -p $@
