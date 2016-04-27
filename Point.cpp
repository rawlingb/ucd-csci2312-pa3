//
// Created on 2/7/2016.
//

#include <cmath>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include "Point.h"
#include "Exceptions.h"

namespace Clustering {
    const char Point::POINT_VALUE_DELIM = ',';
    unsigned int Point::__idGen = 0;

    Point::Point(unsigned int dim) {
        if(dim <= 0) throw ZeroDimensionsEx();
        __dim = dim;
        __values = new double[dim];
        for(int count = 0; count < __dim; count++)
            __values[count] = 0;
        __id = __idGen++;
    }

    Point::Point(unsigned int dim, double *values) {
        if(dim <= 0) throw ZeroDimensionsEx();
        __dim = dim;
        __values = values;
        for(int count = 0; count < __dim; count++)
            __values[count] = 0;
        __id = __idGen++;
    }

    //Copy Constructor
    Point::Point(const Point &point) {
        __id = point.__id;
        __dim = point.__dim;
        __values = new double[point.__dim];
        for (int count = 0; count < __dim; count++)
            __values[count] = point.__values[count];
    }

    //Destructor
    Point::~Point() {
        delete [] __values;
    }

    //Assignment Operator
    Point &Point::operator=(const Point &rhs) {
        if(__dim != rhs.__dim) throw DimensionalityMismatchEx(__dim, rhs.__dim);
        if(*this == rhs)
            return *this;
        delete [] __values;
        __id = rhs.__id;
        __dim = rhs.__dim;
        __values = new double[rhs.__dim];
        for (int count = 0; count < __dim; count++)
            __values[count] = rhs.__values[count];
        return *this;
    }

    void Point::rewindIdGen(){
        --__idGen;
    }

    int Point::getId() const {
        return __id;
    }

    unsigned int Point::getDims() const {
        return __dim;
    }

    void Point::setValue(unsigned int index, double value) {
            if(index >= __dim) throw OutOfBoundsEx(index, __dim);
            __values[index] = value;
    }

    double Point::getValue(unsigned int index) const {
            if(index >= __dim) throw OutOfBoundsEx(index, __dim);
            return __values[index];
    }

    double Point::distanceTo(const Point &point) const {
        double answer = 0;
            if(point.__dim != __dim)
                throw DimensionalityMismatchEx(__dim, point.__dim);
            for (int count = 0; count < __dim; count++)
                answer += pow(point.__values[count] - __values[count], 2);
            return sqrt(answer);
    }

    double &Point::operator[](unsigned int index) {
        if(index >= __dim) throw OutOfBoundsEx(index, __dim);
        return __values[index];
    }

    const double &Point::operator[](unsigned int index) const {
        if(index >= __dim) throw OutOfBoundsEx(index, __dim);
        return __values[index];
    }

    Point &Point::operator*=(double rhs) {
        for (int count = 0; count < __dim; count++)
            __values[count] *= rhs;
        return *this;
    }

    Point &Point::operator/=(double rhs) {
        for (int count = 0; count < __dim; count++)
            __values[count] /= rhs;
        return *this;
    }

    const Point Point::operator*(double rhs) const {
        Point p(*this);
        return p*= rhs;
    }

    const Point Point::operator/(double rhs) const {
        Point lhs(*this);
        return lhs /= rhs;
    }

    Point &operator+=(Point &lhs, const Point &rhs) {
            if (lhs.__dim != rhs.__dim) throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);
            for (int count = 0; count < lhs.__dim; count++)
                lhs[count] += rhs.__values[count];
            return lhs;
    }

    Point &operator-=(Point &lhs, const Point &rhs) {
            if (lhs.__dim != rhs.__dim) throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);
            for (int count = 0; count < lhs.__dim; count++)
                lhs[count] -= rhs.__values[count];
            return lhs;

    }

    const Point operator+(const Point &lhs, const Point &rhs) {
        Point p(lhs.__dim);
        p+=lhs;
        p+=rhs;
        return p;
    }

    const Point operator-(const Point &lhs, const Point &rhs) {
        Point p(lhs.__dim);
        p+=lhs;
        p-=rhs;
        return p;
    }

    bool operator==(const Point &lhs, const Point &rhs) {
        if(lhs.__dim != rhs.__dim) throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);
        if(lhs.__id != rhs.__id) return false;
        for (int count = 0; count < lhs.__dim; count++) {
            if(lhs.__values[count] != rhs.__values[count])
                return false;
        }
        return true;
    }

    bool operator!=(const Point &lhs, const Point &rhs) {
        return !(lhs == rhs);
    }

    bool operator<(const Point &lhs, const Point &rhs) {
            if (lhs.__dim != rhs.__dim)
                throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);
            for (int count = 0; count < lhs.__dim; count++) {
                if (lhs.__values[count] < rhs.__values[count]) {
                    return true;
                } else if (lhs.__values[count] > rhs.__values[count]) {
                    //If this lhs coordinate is more than the rhs coordinate, it doesn't matter what coordinates are on the right
                    //However, it still has to continue if they are equal.
                    return false;
                }
            }
            return false;
    }

    bool operator>(const Point &lhs, const Point &rhs) {
        if (lhs.__dim != rhs.__dim)
            throw DimensionalityMismatchEx(lhs.__dim, rhs.__dim);
        for (int count = 0; count < lhs.__dim; count++) {
            if(lhs.__values[count] > rhs.__values[count]) {
                return true;
			}
			else if (lhs.__values[count] < rhs.__values[count]) {
				//If this lhs coordinate is less than the rhs coordinate, it doesn't matter what coordinates are on the right
				//However, it still has to continue if they are equal.
				return false;
			}
        }
        return false;
    }

    bool operator<=(const Point &lhs, const Point &rhs) {
        return !(lhs > rhs);
    }

    bool operator>=(const Point &lhs, const Point &rhs) {
        return !(lhs < rhs);
    }

    std::ostream &operator<<(std::ostream &ostream, const Point &point) {
        for (int count = 0; count < point.__dim - 1; count++)
            ostream << point.__values[count] << ",";
        ostream << point.__values[point.__dim - 1];
        return ostream;
    }

    std::istream &operator>>(std::istream &istream, Point &point) {
        std::string temp;
        unsigned int count = 0;
        unsigned int numCommas = 0;
        Point tempPoint (point);

		std::string str;
		istream >> str;
        for (int count = 0; count < str.length(); count++) {
            if (str[count] == Point::POINT_VALUE_DELIM)
                numCommas++;
        }
		std::stringstream sstream(str);

        if((numCommas+1) != point.__dim) throw DimensionalityMismatchEx(point.__dim, numCommas+1);

		for (count = 0; count < (point.__dim - 1); count++) {
			getline(sstream, temp, Point::POINT_VALUE_DELIM);
            tempPoint.__values[count] = atof(temp.c_str());
		}
        getline(sstream, temp);
        tempPoint.__values[count] = atof(temp.c_str());
        point = tempPoint;//Note
        return istream;
    }
}



