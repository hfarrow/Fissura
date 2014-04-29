DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
EXTERNAL_DIR=$DIR/../../external
BUILD_DIR=$EXTERNAL_DIR/build
SDL2_SOURCE_DIR=$EXTERNAL_DIR/SDL2

pushd $EXTERNAL_DIR

# Configure and build SDL2
pushd $SDL2_SOURCE_DIR
./configure --prefix=$BUILD_DIR/
make
make install
popd

popd
