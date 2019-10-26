build:
	g++ -std=c++17 main.cpp lpe/* -obin/main

gui:
	g++ -std=c++17 gtkmm.cpp lpe/process.cpp -obin/app `pkg-config gtkmm-3.0 --cflags --libs`