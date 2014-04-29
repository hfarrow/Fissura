export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
pushd $DIR/../..

mkdir build
mkdir build/Debug
mkdir build/Release
mkdir build/RelWithDebInfo
mkdir build/MinSizeRel

cd build/Debug
cmake -DCMAKE_OSX_ARCHITECTURES=x86_64 -DCMAKE_BUILD_TYPE:STRING=Debug ../..

cd ../Release
cmake -DCMAKE_OSX_ARCHITECTURES=x86_64 -DCMAKE_BUILD_TYPE:STRING=Release ../..

cd ../RelWithDebInfo
cmake -DCMAKE_OSX_ARCHITECTURES=x86_64 -DCMAKE_BUILD_TYPE:STRING=RelWithDebInfo ../..

cd ../MinSizeRel
cmake -DCMAKE_OSX_ARCHITECTURES=x86_64 -DCMAKE_BUILD_TYPE:STRING=MinSizeRel ../..

popd
