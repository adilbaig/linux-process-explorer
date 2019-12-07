BUILD_PARAMS = -std=c++17 main.cpp lpe/*
GUI_BUILD_PARAMS = -std=c++17 `pkg-config gtkmm-3.0 --cflags --libs`

build:
	g++ ${BUILD_PARAMS} -obin/main

debug-build:
	g++ ${BUILD_PARAMS} -DDEBUG -g -obin/debug

gui:
	g++ main-app.cpp lpe/* window/* -obin/app ${GUI_BUILD_PARAMS}