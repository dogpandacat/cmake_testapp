//
// Created by AndyCui on 2023/10/10.
//

#ifndef CMAKE_TESTAPP_MADUORUNNER_H
#define CMAKE_TESTAPP_MADUORUNNER_H


#include <ostream>

class MaduoRunner {
public:
    explicit MaduoRunner(bool isSame);



    virtual ~MaduoRunner();

    bool isSame() const;

    void setIsSame(bool isSame);

    MaduoRunner& operator=(const MaduoRunner& other);

    bool operator==(const MaduoRunner &rhs) const;

    bool operator!=(const MaduoRunner &rhs) const;

    //添加friend的目的是因为要访问private成员
    friend std::ostream &operator<<(std::ostream &os, const MaduoRunner &runner);

public:


private:
    bool is_same;
    static bool double_lines;
};


#endif //CMAKE_TESTAPP_MADUORUNNER_H
