BUILD_PARAMS = -std=c++17 main.cpp lpe/*

build:
	g++ ${BUILD_PARAMS} -obin/main

debug-build:
	g++ ${BUILD_PARAMS} -DDEBUG -g -obin/debug

gui:
	g++ -std=c++17 gtkmm.cpp lpe/process.cpp -obin/app `pkg-config gtkmm-3.0 --cflags --libs`