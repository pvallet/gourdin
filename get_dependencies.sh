#!/bin/bash -x

mkdir external
cd external

function get_glm {
  wget https://github.com/g-truc/glm/archive/0.9.8.3.zip
  unzip -q 0.9.8.3.zip
  mv glm-0.9.8.3 glm
  rm 0.9.8.3.zip
}

function get_SDL2pp {
  wget https://github.com/libSDL2pp/libSDL2pp/archive/0.14.1.zip
  unzip -q 0.14.1.zip
  mv libSDL2pp-0.14.1 libSDL2pp
  rm 0.14.1.zip
}

function get_glew {
  wget https://sourceforge.net/projects/glew/files/glew/2.0.0/glew-2.0.0.zip
  unzip -q glew-2.0.0.zip
  mv glew-2.0.0 glew
  rm glew-2.0.0.zip

  cd glew

  cmake build/cmake
  make -j4

  cd ..
}

if [ $# -eq 0 ]; then
  get_glm
  get_SDL2pp
  get_glew
fi

while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    glm)
    get_glm
    ;;

    SDL2pp)
    get_SDL2pp
    ;;

    glew)
    get_glew
    ;;

    *)
    echo "Unkown dependency: " $1
    ;;
esac
shift
done
