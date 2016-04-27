//
// Created by Brian on 2/16/2016.
//
//TODO !__points and assert(size == 0) or (!size)
#include <string>
#include <sstream>
#include <cassert>
#include <limits>
#include "Point.h"
#include "Cluster.h"
#include "Exceptions.h"

namespace Clustering {
    const char Cluster::POINT_CLUSTER_ID_DELIM = ':';
    unsigned int Cluster::__idGenerator = 0;
    LNode::LNode(const Point &p, LNodePtr n) :
            point(p), next(n) { }

    Cluster::Centroid::Centroid(unsigned int d, const Cluster &c) :
            __dimensions(d), __c(c),
            __p(d) { } //For __p, sets point to a point with all 0's with d dimensions (no default constructor)

    void Cluster::Centroid::set(const Point &p) {
        __p = p;
        __valid = true;
    }

    void Cluster::Centroid::setValid(bool valid) {
        __valid = valid;
    }

    bool Cluster::Centroid::isValid() const { return __valid; }

    void Cluster::Centroid::compute() {
        if(__c.__points == nullptr) {
            assert(__c.__size == 0);
            toInfinity();
            return;
        }
        unsigned int count = 0;
        LNodePtr iterate = __c.__points;
        Point p(__c.__dimensionality);
        for (count = 0; iterate != nullptr; count++, iterate = iterate->next) {
            p += iterate->point / __c.__size; //Prevent overflow.  Will still get you the same answer
        }
        assert(count == __c.__size);
        set(p);
    }

    bool Cluster::Centroid::equal(const Point &point) const {
        if (__dimensions != point.getDims()) { } //throw DimensionalityMismatchEx(__dimensions, point.getDims());
        for (int count = 0; count < __dimensions; count++) {
            if (__p[count] != point[count])
                return false;
        }
        return true;
    }

    void Cluster::Centroid::toInfinity() {
        for (int count = 0; count < __dimensions; count++) {
            __p[count] = std::numeric_limits<double>::max();
        }
        __valid = false;
    }

    const Point Cluster::Centroid::get() const { return __p; }

    Cluster::Cluster(unsigned int d) : __points(nullptr), __size(0), __dimensionality(d),
        centroid(d, *this) {
        __id = __idGenerator++;
    }

    Cluster::Cluster(const Cluster &cluster) : centroid(cluster.__dimensionality, *this), __dimensionality(cluster.__dimensionality) {
        __id = cluster.__id;
        __cpy(cluster.__points);
        centroid.compute();
    }

    Cluster &Cluster::operator=(const Cluster &cluster) {
        if (*this == cluster && __id == cluster.__id){ return *this;}
        __del();
        __id = cluster.__id;
        __cpy(cluster.__points);
        centroid.compute();
        return *this;
    }

    void Cluster::__del() {
        while (__size > 0) {
            remove(__points->point);
        }
        __points = nullptr;
    }

    void Cluster::__cpy(LNodePtr pts) {
        __size = 0;
        __points = nullptr;
        LNode *iterate = pts;
        for (int count = 0; iterate != nullptr; count++) {
            this->add(iterate->point);
            iterate = iterate->next;
        }
    }

    Cluster::~Cluster() {
        __del();
    }


    unsigned int Cluster::getSize() const {
        return __size;
    }

    void Cluster::add(const Point &point) {
        if (point.getDims() != __dimensionality) { throw DimensionalityMismatchEx(__dimensionality, point.getDims()); }
        if (__size == 0) { //Copy point and point the beginning to a node containing it
            Point p(point);
            __points = new LNode(p, nullptr);
        }
        else if (__size == 1) {
            if (point >= __points->point) {//If the new point is more than or equal to the current
                Point p(point);//Copy it and make the 1st Node point to a node containing it
                __points->next = new LNode(p, nullptr);
            }
            else { //Otherwise if the new point is smaller
                Point p(point);//Copy the point, make new point to first, then make beginning point to new
                LNode *newNode = new LNode(p, __points);
                __points = newNode;
            }
        }
        else if (__size > 1) {
            int count = 0;
            bool placed = false;
            LNode *iterateNext = __points;
            LNode *iteratePrev = __points;
            while (count < __size && !placed) {
                if (point < iterateNext->point) {
                    if (iterateNext == __points) {
                        Point p(point);
                        LNode *newNode;
                        newNode = new LNode(p, __points);
                        __points = newNode;
                        placed = true;
                    } else {
                        Point p(point);
                        LNode *newNode;
                        newNode = new LNode(p, iteratePrev->next);
                        iteratePrev->next = newNode;
                        placed = true;
                    }
                }
                if (count > 0)
                    iteratePrev = iteratePrev->next;
                iterateNext = iterateNext->next;
                ++count;
            }
            if (!placed) {
                Point p(point);
                LNode *newNode;
                newNode = new LNode(p, iteratePrev->next);
                iteratePrev->next = newNode;
                placed = true;
            }

        }
        __size++;
        centroid.setValid(false);
    }

    const Point &Cluster::remove(const Point &point) {
        if (point.getDims() != __dimensionality) { throw DimensionalityMismatchEx(__dimensionality, point.getDims()); }
        if (__size == 1) {
            if (contains(point)) {
                Point *p = new Point(point);//Remove the point
                delete __points;
                __points = nullptr;
                __size--;
                centroid.setValid(false);
                return *p;
            }
        }
        else if (__size > 1) {
            int count = 0;
            bool placed = false;
            LNode *iterateNext = __points;
            LNode *iteratePrev = __points;
            while (count < __size && !placed) {
                if (contains(point)) {
                    if (point == iterateNext->point) {
                        if (point == __points->point) {
                            Point *p = new Point(point);
                            LNodePtr tempNode = __points;
                            __points = __points->next;
                            delete tempNode;
                            __size--;
                            centroid.setValid(false);
                            return *p;
                        } else {
                            Point *p = new Point(point);
                            LNodePtr tempNode = iteratePrev->next;
                            iteratePrev->next = iterateNext->next;
                            delete tempNode;
                            __size--;
                            centroid.setValid(false);
                            return *p;
                        }
                    }
                    //If these points are equal, make the previous point point to whatever
                    //this point was pointing to and delete the current one
                }
                iterateNext = iterateNext->next;
                if (count > 0)
                    iteratePrev = iteratePrev->next;
                count++;
            }

        }
        centroid.setValid(false);
        return point;
    }

    bool Cluster::contains(const Point &point) const {
        if (__size == 0) { //Nothing to check for
            return false;
        }
        else if (__size == 1) {
            if (point == __points->point)
                return true;
            return false;
        }
        else if (__size > 1) {
            LNode *iterate = __points;
            for (int count = 0; count < __size; count++) {
                if (point == iterate->point)
                    return true;
                iterate = iterate->next;
            }
            return false;
        }

    }

    const Point &Cluster::operator[](unsigned int index) const {
        if(__points == nullptr) { throw EmptyClusterEx(); }
        if( index >= __size) { throw OutOfBoundsEx(__size, index); }
        int count = 0;
        LNode *iterate = __points;
        for (count = 0; count < __size && count != index; iterate = iterate->next)
            ++count;
        return iterate->point;
    }

    Cluster &Cluster::operator+=(const Point &point) {
        add(point);
        return *this;
    }

    Cluster &Cluster::operator-=(const Point &point) {
        remove(point);
        return *this;
    }

    const Cluster operator+(const Cluster &cluster, const Point &point) {
        Cluster c(cluster);
        c += point;
        return c;
    }

    const Cluster operator-(const Cluster &cluster, const Point &point) {
        Cluster c(cluster);
        c -= point;
        return c;
    }

    Cluster &Cluster::operator+=(const Cluster &cluster) {
        for (int count = 0; count < cluster.__size; count++) {
            if (!contains(cluster[count]))
                add(cluster[count]);
        }
        return *this;

    }

    Cluster &Cluster::operator-=(const Cluster &cluster) {
        for (int count = 0; count < cluster.__size; count++) {
            remove(cluster[count]);
        }
        return *this;
    }

    const Cluster operator+(const Cluster &cluster, const Cluster &cluster1) {
        Cluster c(cluster);
        c += cluster1;
        return c;
    }

    const Cluster operator-(const Cluster &cluster, const Cluster &cluster1) {
        Cluster c(cluster);
        c -= cluster1;
        return c;
    }

    bool operator==(const Cluster &cluster, const Cluster &cluster1) {
        if (cluster.__dimensionality != cluster1.__dimensionality) {
            throw DimensionalityMismatchEx( cluster.__dimensionality, cluster1.__dimensionality );
        }
        if (cluster.__size != cluster1.__size) {
            return false;
        }
        if (cluster.__size == 0 && cluster1.__size == 0) { //Nothing to check for
            return true;
        }
        else if (cluster.__size >= 1 && cluster1.__size >= 1) {
            LNode *iterate = cluster.__points;
            LNode *iterate1 = cluster1.__points;
            for (int count = 0; count < cluster.__size; count++) {
                if (iterate->point != iterate1->point)
                    return false;

            }
        }
        return true;
    }

    bool operator!=(const Cluster &cluster, const Cluster &cluster1) {
        return !(cluster == cluster1);
    }

    std::ostream &operator<<(std::ostream &ostream, const Cluster &cluster) {
        LNode *iterate = cluster.__points;
        for (int count = 0; count < cluster.__size - 1; count++) {
            ostream << iterate->point << " " << Cluster::POINT_CLUSTER_ID_DELIM << " " << cluster.__id << std::endl;
            iterate = iterate->next;
        }
        ostream << iterate->point << " " << Cluster::POINT_CLUSTER_ID_DELIM << " " << cluster.__id << std::endl;
        return ostream;
    }

    std::istream &operator>>(std::istream &istream, Cluster &cluster) {
        std::string str;
            while (getline(istream, str)) {
                try {
                    int dim = 1;
                    for (int count = 0; count < str.length(); count++) {
                        if (isspace(str[count]))
                            str.erase(count--, 1);
                    }
                    for (int count = 0; count < str.length(); count++) {
                        if (str[count] == ',')
                            dim++;
                    }
                    std::stringstream sstream(str);
                    Point p(dim);
                    sstream >> p;
                    cluster.add(p);
                } catch (DimensionalityMismatchEx e) {
                    std::cout << e << std::endl;
                }
            }
        return istream;
    }

    Cluster::Move::Move(const Point &p, Cluster &from, Cluster &to) : __p(p), __from(from), __to(to){ }

    void Cluster::Move::perform() {
        __to.add(__p);
        __from.remove(__p);
        __to.centroid.setValid(false);
        __from.centroid.setValid(false);
    }

    void Cluster::pickCentroids(unsigned int k, Point **pointArray) {
        //PointArray is an array of pointers to Points
        LNodePtr iterate = __points;
        if (k >= __size) {
            for (int count = 0; count < k; count++) {
                if (iterate != nullptr) {
                    pointArray[count] = new Point(iterate->point);
                    iterate = iterate->next;
                } else {
                    pointArray[count] = new Point(__dimensionality);
                    for (int count1 = 0; count1 < __dimensionality; count1++) {
                        pointArray[count]->setValue(count1, std::numeric_limits<double>::max());
                    }
                }
            }
        } else if (k < __size) {
            unsigned int elimSize = 0;
            int* eliminated = new int[k];
            for (int index = 0; index < k; index++) {
                Point p(__dimensionality);
                double *distances = nullptr;
                int tempSize;
                if (index == 0) {
                    tempSize = __size;//Size of the array of distances is as big as the # of points in the Cluster
                    assert(__size >= 0);
                    int count = 0;//Used to take the average of the points
                    LNodePtr iterate = __points;
                    //Add up all of the points and take their average
                    for (count = 0; iterate != nullptr; count++, iterate = iterate->next)
                        p += (iterate->point)/tempSize;
                    //Then make a new array that holds the distance between the average of all points and each point in the Cluster
                    distances = new double[tempSize];
                    iterate = __points;
                    for (count = 0; iterate != nullptr; count++, iterate = iterate->next) {
                        distances[count] = p.distanceTo(iterate->point);
                    }
                    //For the zero case, make the first element equal to whatever point was furthest away from the average, then
                    //eliminate that point from possibly being picked again.
                } else if (index > 0) {
                    tempSize = __size - index;//Size of the array is smaller by index
                    int count;//Used to take the average of the points
                    for (count = 0; count < index; count++)
                        p += (*pointArray[count])/index;
                    //make a new array that holds the distance between the average of all points current on the point array and each point in the Cluster
                    distances = new double[tempSize];
                    LNodePtr iterate = __points;
                    bool isEliminated = false;
                    int nodeIndex = 0;
                    for (count = 0; count < tempSize; iterate = iterate->next) {
                        isEliminated = false;
                        for (int count1 = 0; count1 < index; count1++) {
                            if (eliminated[count1] == nodeIndex) //If count is equal to the index of a number that has already been picked
                                isEliminated = true;
                        }
                        if (!isEliminated) {
                            distances[count] = p.distanceTo(iterate->point);
                            count++;
                        }
                        nodeIndex++;
                    }
                }
                double furthest = distances[0];
                int furthestIndex = 0;
                for (int count = 1; count < tempSize; count++) {
                    if (distances[count] > furthest) {
                        furthest = distances[count];
                        furthestIndex = count;
                    }
                }
                pointArray[index] = new Point(this->operator[](furthestIndex));
                eliminated[index] = furthestIndex; //This index will not be picked again.
                elimSize++;
            }
        }
    }

    bool Cluster::__in(const Point &p) const {
        return false;
    }

    unsigned int Cluster::getId() const { return __id; }

    unsigned int Cluster::getDimensionality() const { return __dimensionality; }
}

