//
// Created by AndyCui on 2023/10/10.
//

#include "MaduoRunner.h"

MaduoRunner::MaduoRunner(bool isSame) : is_same(isSame) {

}

MaduoRunner::~MaduoRunner() {

}

bool MaduoRunner::isSame() const {
    return is_same;
}

void MaduoRunner::setIsSame(bool isSame) {
    is_same = isSame;
}

bool MaduoRunner::operator==(const MaduoRunner &rhs) const {
    return is_same == rhs.is_same;
}

bool MaduoRunner::operator!=(const MaduoRunner &rhs) const {
    return !(rhs == *this);
}

std::ostream &operator<<(std::ostream &os, const MaduoRunner &runner) {
    os << "is_same: " << runner.is_same;
    return os;
}

MaduoRunner &MaduoRunner::operator=(const MaduoRunner &other) {
    if(this != &other) {
        this->is_same = other.is_same;
    }
    return *this;
}
