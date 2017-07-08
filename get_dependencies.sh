#!/bin/bash -x

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

function get_SDL2 {
  cd SDL2
  wget https://www.libsdl.org/release/SDL2-2.0.5.zip
  unzip -q SDL2-2.0.5.zip
  mv SDL2-2.0.5 SDL2
  rm SDL2-2.0.5.zip
  cd ..
}

function get_SDL2_image {
  cd SDL2
  wget https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.1.zip
  unzip -q SDL2_image-2.0.1.zip
  mv SDL2_image-2.0.1 SDL2_image
  rm SDL2_image-2.0.1.zip
  cd ..
}

if [ $# -eq 0 ]; then
  get_glm
  get_SDL2pp
  get_SDL2
  get_SDL2_image
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

    SDL2)
    get_SDL2
    ;;

    SDL2_image)
    get_SDL2_image
    ;;

    clean)
    rm -rf glm libSDL2pp SDL2/SDL2 SDL2/SDL2_image
    ;;

    *)
    echo "Unkown dependency: " $1
    ;;
esac
shift
done
