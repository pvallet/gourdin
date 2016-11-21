#!/bin/bash -x

mkdir external
cd external

function get_glm {
  wget https://github.com/g-truc/glm/archive/0.9.8.3.zip
  unzip -q 0.9.8.3.zip
  mv glm-0.9.8.3 glm
  rm 0.9.8.3.zip
}

function get_SFML {
  wget www.sfml-dev.org/files/SFML-2.4.1-sources.zip
  unzip -q SFML-2.4.1-sources.zip
  mv SFML-2.4.1 SFML
  rm SFML-2.4.1-sources.zip

  cd SFML
  mkdir build
  cd build

  cmake ..
  make -j4
  cp -r lib ..

  cd ../..
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

function get_flann {
  wget https://github.com/mariusmuja/flann/archive/1.9.1.zip
  unzip -q 1.9.1.zip
  mv flann-1.9.1 flann
  rm 1.9.1.zip

  cd flann
  mkdir build
  cd build

  cmake ..
  make -j4
  cp -r lib ..

  cd ../..
}

if [ $# -eq 0 ]; then
  get_glm
  get_SFML
  get_glew
  get_flann
fi

while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    glm)
    get_glm
    ;;

    SFML)
    get_SFML
    ;;

    glew)
    get_glew
    ;;

    flann)
    get_flann
    ;;

    *)
    echo "Unkown dependency: " $1
    ;;
esac
shift
done
