//
// Created by AndyCui on 2023/10/20.
//

#ifndef CMAKE_TESTAPP_POINT_H
#define CMAKE_TESTAPP_POINT_H

#include <ostream>
#include <vector>
#include <array>
#include <cassert>

using namespace std;

namespace a::b::c
{


    class matrix3d
    {
    private:

        //直接初始化这个矩阵的初始数据
        double a[4][4] {
                        {1.0, 0.0, 0.0, 0.0},
                        {0.0, 1.0, 0.0, 0.0},
                        {0.0, 0.0, 1.0, 0.0},
                        {0.0, 0.0, 0.0, 1.0}
                        };

        //用这个也是没有问题的
        vector<vector<double>> b{
                {1.0, 0.0, 0.0, 0.0},
                {0.0, 1.0, 0.0, 0.0},
                {0.0, 0.0, 1.0, 0.0},
                {0.0, 0.0, 0.0, 1.0}
        };

        //为什么array需要{{}}
        array<array<double, 4>, 4> c {{
                                       {1.0, 0.0, 0.0, 0.0},
                                       {0.0, 1.0, 0.0, 0.0},
                                       {0.0, 0.0, 1.0, 0.0},
                                       {0.0, 0.0, 0.0, 1.0}
        }};


    };

    class vector3d
    {
    public:
        vector3d(double dx, double dy, double dz, double dw) : dx(dx), dy(dy), dz(dz), dw(dw) {}

    private:
        double dx;
        double dy;
        double dz;
        double dw;
    };

    /**
     * 平常都是用笛卡尔坐标系中的xyz来表示3d空间中的一个点的，简单直观，但是有问题，几何变换中涉及到：平移、旋转、缩放、仿射变换等操作，用笛卡尔坐标值表示法
     * 统一到矩阵乘法中，意思就是，我们一律用矩阵乘法来表示几何变换中的：平移、旋转、缩放、仿射变换等操作，就不能用(x,y,z)这种笛卡尔坐标系下的坐标值表示方法了。
     *
     * 新的方法是齐次坐标，抛弃传统的三维笛卡尔坐标表示法，所以变成   笛卡尔坐标表示法  vs   齐次坐标表示法， 齐次坐标表示法大胜出！
     * 齐次坐标可以用来表示点、向量、矩阵，而笛卡尔坐标不能表示。
     *
     * 简单来说：齐次坐标就是用N+1维的坐标来表示N维坐标
     */
    class point3d
    {
    public:
        point3d(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {

        }

        enum class Category : int {
            isSmall = 0,
            isMedium = 1,
            isLarge = 2
        };
        //为什么会有friend，因为在使用方写代码的时候，会 cout << p1; 此时就会调用operator<<这个重载函数，而这个重载函数需要访问point3d的private变量
        friend std::ostream &operator<<(std::ostream &os, const point3d &d) {
            //z不需要输出
            os << "point3d x: " << d.x << " y: " << d.y << " z: " << d.z;
            return os;
        }

        point3d(const point3d& other) noexcept
        {
            assert(&other != this); //这行代码是惯例代码，拷贝构造函数不允许自己给自己拷贝，那没有意义
            switch (static_cast<int>(other.x)) {
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    break;
                default:
                    cout<<"default branch"<<endl;
            }
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;
        }

        point3d& operator=(const point3d& other) {
            this->x = other.x;
            this->y = other.y;
            this->z = other.z;
            this->w = other.w;
            return *this;
        }

        bool operator==(const point3d &rhs) const {
            return x == rhs.x &&
                   y == rhs.y &&
                   z == rhs.z &&
                   w == rhs.w;
        }

        bool operator!=(const point3d &rhs) const {
            return !(rhs == *this);
        }

        vector3d pointTo(const point3d& p2) const noexcept {
            return vector3d(p2.x -x , p2.y-y, p2.z-z, 0.0);
        }

    private:
        double x{0.0};
        double y{0.0};
        double z{0.0};
        double w{1.0};   //使用齐次坐标表示一个空间的点，w一般取值为1
    };

}


#endif //CMAKE_TESTAPP_POINT_H
