//
// Created by Brian on 3/15/2016.
//

#include "Exceptions.h"

namespace Clustering {

    OutOfBoundsEx::OutOfBoundsEx(unsigned int c, int r) : __current(c), __rhs(r), __name("OutOfBoundsEx") { }

    std::ostream &operator<<(std::ostream &os, const OutOfBoundsEx &ex) {
        os << ex.__name << "(Max Dimension: " << ex.__current
           << ", Received Dimension: " << ex.__rhs << std::endl;
        return os;
    }

    DimensionalityMismatchEx::DimensionalityMismatchEx(unsigned int c, unsigned int r) : __current(c), __rhs(r), __name("DimensionalityMismatchEx") { }

    std::ostream &operator<<(std::ostream &os, const DimensionalityMismatchEx &ex) {
        os << ex.__name << "(Current Dimension: " << ex.__current
        << ", Received Dimension: " << ex.__rhs << std::endl;
        return os;
    }

    ZeroClustersEx::ZeroClustersEx() : __name("ZeroClustersEx") { }

    std::ostream &operator<<(std::ostream &os, const ZeroClustersEx &ex) {
        os << ex.__name << " Found no Clusters" << std::endl;
        return os;
    }

    DataFileOpenEx::DataFileOpenEx(std::string filename) : __name("DataFileOpenEx"), __filename(filename){ }

    std::ostream &operator<<(std::ostream &os, const DataFileOpenEx &ex) {
        os << ex.__name << " Error opening file " << ex.__filename << std::endl;
        return os;
    }

    ZeroDimensionsEx::ZeroDimensionsEx() : __name("ZeroDimensionsEx") { }

    std::ostream &operator<<(std::ostream &os, const ZeroDimensionsEx &ex) {
        os << ex.__name << " Tried to initialize a Cluster with no dimensions " << std::endl;
        return os;
    }

    EmptyClusterEx::EmptyClusterEx() : __name("EmptyClusterEx") { }

    std::ostream &operator<<(std::ostream &os, const EmptyClusterEx &ex) {
        os << ex.__name << " Tried to initialize a K-Means object with no Clusters " << std::endl;
        return os;
    }
}