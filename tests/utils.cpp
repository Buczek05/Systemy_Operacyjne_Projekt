#pragma once

#include <iostream>
#include <exception>

class TestException : public std::exception {
    std::string message;
public:
    explicit TestException(const std::string &msg = "TestException occurred") : message(msg) {}
    const char *what() const _NOEXCEPT override {
        return message.c_str();
    }
};