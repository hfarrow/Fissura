DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. $DIR/build_externals.sh
. $DIR/fs_setup_build.sh
. $DIR/fs_build_all_and_test.sh Debug
. $DIR/fs_build_all_and_test.sh Release
. $DIR/fs_build_all_and_test.sh MinSizeRel
