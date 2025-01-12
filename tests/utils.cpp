#pragma once

#include <iostream>
#include <exception>

class TestException : public std::exception {
public:
    const char *what() const _NOEXCEPT override {
        return "TestException occurred";
    }
};
