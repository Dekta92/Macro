# Macro

(Currently in Development)
This is a simple C++ terminal-only macro recorder and player.
This is my first ever programming project on Github so please be kind. Although I would request you to provide suggestions on how to improve it anyway.
This project utilizes the X11 library.
### Prerequisites

* Works only on X window managers

### Dependencies for compiling manually

For Ubuntu/Debian
```
sudo apt update
sudo apt install build-essential libx11-dev
```
For Fedora
```
sudo dnf groupinstall "Development Tools"
sudo dnf install libX11-devel
```
For Arch
```
sudo pacman -S base-devel libx11
```

### Compilation command

```
cd path/to/main.cpp
g++ main.cpp -o main -lX11 && ./main
```
