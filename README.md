Setup

Linux Ubuntu

clone repository

install cmake
sudo apt-get update
sudo apt-get install cmake

install sdl2, glew

sudo apt-get install libsdl2-dev libglew-dev

create a folder named build on the same level as source and external

cd build
cmake ..
make
./execute_file

Add this as submodule somewhere else

cd path/to/your/project
git submodule add https://github.com/smoksde/lumina.git external
git submodule update --init --recursive
git add external
git commit -m "Added submodule lumina"
