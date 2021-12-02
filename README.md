# linux-process-explorer
Linux Process Explorer is a GUI and CLI utility to display runtime information about a process.

## Setup Development Libraries
You need to install [GTKMM 3](http://www.gtkmm.org) for Linux. Run the following on Ubuntu:

```sh
sudo apt install libgtkmm-3.0-dev
```

## Build Instructions
```sh
make gui
./bin/app $$
```
