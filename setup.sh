export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

mkdir -p build/Debug
mkdir -p build/DebugTestable
mkdir -p build/Release
mkdir -p build/ReleaseTestable
mkdir -p build/RelWithDebInfo
mkdir -p build/MinSizeRel

cd build/Debug
cmake -DCMAKE_USER_MAKE_RULES_OVERRIDE=~/dev/ClangOverrides.txt -DCMAKE_BUILD_TYPE:STRING=Debug ../..

cd ../../build/DebugTestable
cmake -DCMAKE_USER_MAKE_RULES_OVERRIDE=~/dev/ClangOverrides.txt -DCMAKE_BUILD_TYPE:STRING=DebugTestable ../..

cd ../../build/Release
cmake -DCMAKE_USER_MAKE_RULES_OVERRIDE=~/dev/ClangOverrides.txt -DCMAKE_BUILD_TYPE:STRING=Release ../..

cd ../../build/ReleaseTestable
cmake -DCMAKE_USER_MAKE_RULES_OVERRIDE=~/dev/ClangOverrides.txt -DCMAKE_BUILD_TYPE:STRING=ReleaseTestable ../..

cd ../../build/RelWithDebInfo
cmake -DCMAKE_USER_MAKE_RULES_OVERRIDE=~/dev/ClangOverrides.txt -DCMAKE_BUILD_TYPE:STRING=RelWithDebInfo ../..

cd ../../build/MinSizeRel
cmake -DCMAKE_USER_MAKE_RULES_OVERRIDE=~/dev/ClangOverrides.txt -DCMAKE_BUILD_TYPE:STRING=MinSizeRel ../..
# make
