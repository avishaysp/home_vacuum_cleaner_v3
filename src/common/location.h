#pragma once
#include <string>
#include <iostream>

class Location {
protected:
    int row;
    int col;

public:
    Location() : row(0), col(0) {}
    Location(int row, int col) : row(row), col(col) {}
    Location(const Location& other) : Location(other.row, other.col) {}

    virtual ~Location() {}

    int getRow() const { return row; }
    int getCol() const { return col; }

    void setRow(int row) { this->row = row; }
    void setCol(int col) { this->col = col; }
    void setBoth(int row, int col) {
        this->row = row;
        this->col = col;
    }

    bool operator==(const Location& other) const {
        return row == other.row && col == other.col;
    }

    bool operator!=(const Location& other) const {
        return !(*this == other);
    }

    std::string toString() const {
        return "(" + std::to_string(row) + ", " + std::to_string(col) + ")";
    }

    void print() const {
        std::cout << toString() << std::endl;
    }

    Location& operator=(const Location& other) {
        if (this == &other) {
            return *this; // handle self assignment
        }
        row = other.row;
        col = other.col;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Location& loc) {
        os << loc.toString();
        return os;
    }
};