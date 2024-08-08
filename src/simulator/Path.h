// Path.h
#pragma once

#include <vector>
#include "house.h"
#include <string>
#include "../common/enums.h"

class Path {
public:
    Path();

    void addEntry(Step dir);

    size_t getLength() const;

    void print() const;
    std::string toString() const;
    friend std::ostream& operator<<(std::ostream& os, const Path& path);

private:

    std::vector<Step> vec;

};
