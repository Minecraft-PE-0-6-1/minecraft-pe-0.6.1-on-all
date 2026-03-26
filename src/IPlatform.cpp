#include <IPlatform.h>
#include <App.h>
#include <fstream>

void IPlatform::runMainLoop(App& app) {
    while(!app.wantToQuit()) app.update();
}

ByteVector IPlatform::readAssetFile(const std::string& path) {
    std::ifstream instream(path, std::ios::in | std::ios::binary);
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
    
    return data;
}