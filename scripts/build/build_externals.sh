export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
EXTERNAL_DIR=$DIR/../../external
BUILD_DIR=$EXTERNAL_DIR/build

SDL2_DIR=$EXTERNAL_DIR/SDL2
TINYXML_DIR=$EXTERNAL_DIR/tinyxml

pushd $EXTERNAL_DIR

# Configure and build SDL2
pushd $SDL2_DIR
./configure --prefix=$BUILD_DIR/
make
make install
popd

pushd $TINYXML_DIR
rm -rf build
mkdir build
cd build
cmake ../
make
make install
cd ..
popd

popd
