#pragma once

#include <string>
#include <vector>

namespace HttpClient {

bool download(const std::string& url, std::vector<unsigned char>& outBody);

} // namespace HttpClient

