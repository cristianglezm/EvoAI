// libFuzzer harness for JsonBox::Value::loadFromString() itself, with no
// EvoAI type on top.
//
// Every other harness in this directory assumes JsonBox either parses
// successfully or throws a catchable exception on malformed input - none
// of them would catch a bug that lives entirely inside JsonBox's own
// parser (a crash before EvoAI ever sees a constructed Value). This
// harness exists specifically to test that assumption, independent of
// anything EvoAI-specific.
#include <JsonBox.h>
#include <cstddef>
#include <cstdint>
#include <string>

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size){
    std::string input(reinterpret_cast<const char*>(data), size);
    JsonBox::Value v;
    try{
        v.loadFromString(input);
    }catch(const std::exception&){
        return 0;
    }
    return 0;
}
