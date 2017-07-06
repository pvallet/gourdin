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

if [ $# -eq 0 ]; then
  get_glm
  get_SDL2pp
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

    *)
    echo "Unkown dependency: " $1
    ;;
esac
shift
done
