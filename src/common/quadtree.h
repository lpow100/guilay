#ifndef QUADTREE_H
#define QUADTREE_H

#include "elements.h"
#include "types.h"

typedef enum {
    QT_EMPTY,
    QT_PARTIALLY_OCCUPIED,
    QT_FULLY_OCCUPIED,
    QT_CONTAINS_ITEM
} NodeStatus;

typedef struct QuadtreeNode {
    Rect bounds;
    NodeStatus status;
    struct QuadtreeNode *children[4];
    Element *inhabitant; 
} QuadtreeNode;

#endif