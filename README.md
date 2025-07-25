# Cross Platform Bullet Hell

A 2D shooting game created for my last university project.

## Installation

```bash
git clone https://github.com/Tail-R/cross-platform-bullet-hell
cd cross-platform-bullet-hell

mkdir build
cd build

# If you are using Windows
cmake .. -G "MinGW Makefiles"

cmake --build .
```


## Major Dependencies

### Windows

- **SDL2 (Simple DirectMedia Layer) 2.32.8-mingw**  
  [Official Site](https://www.libsdl.org/)  
  [GitHub Releases](https://github.com/libsdl-org/SDL/releases)

- **LuaJIT**  
  [GitHub](https://github.com/LuaJIT/LuaJIT)

- **Sol2 3.2.2**  
  [GitHub](https://github.com/ThePhD/sol2.git)

---

### Linux

- **SDL2 (Simple DirectMedia Layer) 2.32.8**  
  [Official Site](https://www.libsdl.org/)  

```bash
# Arch Linux
pacman -S sdl2

# Debian/Ubuntu
sudo apt-get install libsdl2-dev
```
