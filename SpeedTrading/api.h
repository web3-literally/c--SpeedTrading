#pragma once

#include <string>

int do_post_request(const std::string suburl, const std::wstring data);

int do_multi_post_request(const std::wstring suburl, const void *param);

int do_get_request(const std::wstring suburl, const void *param);