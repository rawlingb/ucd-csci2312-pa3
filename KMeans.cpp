//
// Created by Brian on 3/15/2016.
//

#include <string>
#include <fstream>
#include "KMeans.h"
#include "Exceptions.h"

namespace Clustering {

    KMeans::KMeans(unsigned int dim, unsigned int k, std::string filename, unsigned int maxIter) :
        __dimensionality(dim), __k(k), __maxIter(maxIter), __iFileName(filename), __numIter(0), __numNonempty(0), __clusters(nullptr), __initCentroids(nullptr) {
        if(k == 0) { throw ZeroClustersEx(); }
        __clusters = new Cluster*[k];//Array of pointers to cluster
        __initCentroids = new Point*[k];//With this initialization, you can initialize an array without calling the ctor,
        //since all the array contains at this point is uninitialized pointers to points
        std::fstream temp(filename);
        if(temp.is_open()) {
            for(int count = 0; count < k; count++) {
                __clusters[count] = new Cluster(dim);
            }
            temp>>*(__clusters[0]);
        } else {
            throw DataFileOpenEx(filename);
        }
        __clusters[0]->pickCentroids(k, __initCentroids);
        for(int count = 0; count < k; count++) {
            if(count >__clusters[0]->getSize()){
                __clusters[count]->centroid.toInfinity();
            } else{
                __clusters[count]->centroid.set(*__initCentroids[count]);
            }
        }
        //File blank
            //Throw Exception
        //If has contents
            //Delete before throwing
        //temp.close;
    }

    KMeans::~KMeans() {
        for(int count = 0; count < __k; count++) {
            delete __clusters[count];
            delete __initCentroids[count];
        }
        delete []__clusters;
        delete []__initCentroids;
        __clusters = nullptr;
        __initCentroids = nullptr;
    }


    void KMeans::run() {
        unsigned int moves = 100;
        unsigned int iter = 0;
        while (moves > 0 && iter < __maxIter) {
            moves = 0;
            for(unsigned int cNum = 0; cNum < __k; cNum++){ //For each cluster in __cluster
                for(unsigned int pNum = 0; pNum < __clusters[cNum]->getSize(); pNum++){ //For each point in cluster
                    double closest = __clusters[0]->centroid.get().distanceTo((*__clusters[cNum])[pNum]);
                    unsigned int closestIndex = 0;
                    for (unsigned int count = 1; count < __k; count++) {
                        if(__clusters[count]->centroid.get().distanceTo((*__clusters[cNum])[pNum]) < closest) {
                            closest = __clusters[count]->centroid.get().distanceTo((*__clusters[cNum])[pNum]);
                            closestIndex = count;
                        }
                    }
                    if(cNum != closestIndex) {
                        Cluster::Move move((*__clusters[cNum])[pNum],*__clusters[cNum],*__clusters[closestIndex]);
                        move.perform();
                        moves++;
                        pNum--;
                    }
                }
            } //After this, recompute centroids
            for(unsigned int cNum = 0; cNum < __k; cNum++){
                __clusters[cNum]->centroid.compute();
            }
            iter++;
        }
        __numIter = iter;
        __numMovesLastIter = moves;
        unsigned int nonEmpty = 0;
        for(unsigned int cNum = 0; cNum < __k; cNum++){
            if(__clusters[cNum]->centroid.isValid())
                nonEmpty++;
        }
        __numNonempty = nonEmpty;
    }

    std::ostream &operator<<(std::ostream &os, const KMeans &kmeans) {
        for(int count = 0; count < kmeans.__k; count++) {
            if(kmeans.__clusters[count]->centroid.isValid()) {
                os << (*(kmeans.__clusters[count]));
            }
        }
        return os;
    }

}

