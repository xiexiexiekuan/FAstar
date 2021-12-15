#ifndef NODE_H
#define	NODE_H

#include <limits>

#include "typedefs.h"

class Node {
public:

    Node() : g(0), h(0), subnetwork(varset(0)), leaf(0), pqPos(-1) {
    }

    Node(float g, float h, varset &subnetwork, byte &leaf) :
    g(g), h(h), subnetwork(subnetwork), leaf(leaf), pqPos(0) {
    }

    float getF() const {
        return g + h;
    }

    varset getSubnetwork() const {
        return subnetwork;
    }

    int getPqPos() const {
        return pqPos;
    }

    byte getLeaf() const {
        return leaf;
    }

    float getH() const {
        return h;
    }

    float getG() const {
        return g;
    }

    /**
     * Count layer of this node.
     * @return 
     */
    byte getLayer() {
        return cardinality(subnetwork);
    }

    void copy(Node* n) {
        g = n->g;
        h = n->h;
        leaf = n->leaf;
        subnetwork = n->subnetwork;
    }

    void setSubnetwork(varset subnetwork) {
        this->subnetwork = subnetwork;
    }

    void setPqPos(int pqPos) {
        this->pqPos = pqPos;
    }

    void setLeaf(byte leaf) {
        this->leaf = leaf;
    }

    void setG(float g) {
        this->g = g;
    }

    void setH(float h) {
        this->h = h;
    }
    
protected:
    float g;
    float h;
    varset subnetwork;
    byte leaf;
    int pqPos;
};

struct CompareNodeStar {

    bool operator()(Node *a, Node * b) {

        float diff = a->getF() - b->getF();
        if (fabs(diff) < std::numeric_limits<float>::epsilon()) {
            return (b->getLayer() - a->getLayer()) > 0;
        }

        return (diff > 0);
    }
};

// TODO: make this work for boost::bitset representation
struct CompareNodeStarLexicographic {
    
    bool operator()(Node *a, Node * b) {
        int diff = a->getSubnetwork() - b->getSubnetwork();
        if (diff == 0) {
            diff = a->getF() - b->getF();
            if (fabs(diff) < std::numeric_limits<float>::epsilon()) {
                return false;
            }
        }
        return (diff > 0);
    }
};

#endif	/* NODE_H */

