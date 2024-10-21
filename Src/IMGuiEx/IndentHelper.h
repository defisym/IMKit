#pragma once

struct IndentHelper {  // NOLINT(cppcoreguidelines-special-member-functions)
    float indent = {};

    IndentHelper(const float i = 0.0f);
    ~IndentHelper();
};