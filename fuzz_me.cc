#include <tvm/driver/driver_api.h>
#include <tvm/parser/parser.h>
#include <tvm/target/target.h>
#include <tvm/node/serialization.h>
#include <tvm/node/node.h>
#include "tvm/ir/module.h"
#include <tvm/runtime/object.h>

#include <cstdint>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string_view>

/*
HOWTO (by Jiawei):
git checkout libfuzz
mkdir -p libfuzz-build && cd libfuzz-build
cp ../cmake/config.cmake .
cmake .. -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Release -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=1
ninja
mkdir corpus # all files triggering new coverage will be placed here
mkdir seeds 
# @Sen, please put all tir seeds into `seeds`. 
./fuzz_me corpus seeds & # Let it run asynchrously
# Let's say it won't crash...
# Kill the program 4 hours later...
sleep 14400 && killall fuzz_me


# OK, almost done.
# then run files under corpus using tzer after checkout to coverage branch.
git checkout coverage
mkdir -p build && cd build
cmake .. -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Release -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=1
ninja
# ... Calculate coverage of files under `libfuzz-build/corpus` as what you do with TVMFuzz.
*/

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  try {
    std::string data(Size, '0');
    for (size_t i=0; i < Size; ++i)
      data[i] = Data[i];

    tvm::IRModule ir_mod(tvm::Downcast<tvm::IRModule>(tvm::LoadJSON(data)));
    auto module = tvm::build(ir_mod, tvm::Target("llvm"), tvm::Target("llvm"));
  } catch (...) {}
  return 0;  // Non-zero return values are reserved for future use.
}
