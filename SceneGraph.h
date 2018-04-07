#pragma once

#include <assert.h>

#include <raylib.h>

typedef struct GraphNode {
    Model const *model;
    Matrix transform;
    void (*update)(struct GraphNode *node, void *user);
    struct GraphNode *next_sibling;
    struct GraphNode *prev_sibling;
    struct GraphNode *parent;
    struct GraphNode *first_child;
    struct GraphNode *last_child;
} GraphNode;

static inline GraphNode Node_New(Model const *model, Matrix transform) {
    void Node_Update_Stub(GraphNode *node, void *user);
    GraphNode node = {0};
    node.model = model;
    node.transform = transform;
    node.update = Node_Update_Stub;
    return node;
}

static inline void Node_AddChild(GraphNode *node, GraphNode *child) {
    if (node->last_child) {
        assert(!node->last_child->next_sibling);
        node->last_child->next_sibling = child;
        child->prev_sibling = node->last_child;
    } else {
        node->last_child = node->first_child = child;
    }

    child->parent = node;
}

static inline void Node_Remove(GraphNode *node) {
    if (node->parent) {
        if (node->parent->first_child == node) {
            node->parent->first_child = node->next_sibling;
        } else if (node->parent->last_child == node) {
            node->parent->last_child = node->prev_sibling;
        }
    }

    node->prev_sibling->next_sibling = node->next_sibling;
    node->next_sibling->prev_sibling = node->prev_sibling;

    for (GraphNode *child = node->first_child; child; child = child->next_sibling) {
        child->parent = node->parent;
    }
}

void Node_Update(GraphNode *node, void *user);
void Node_Draw(GraphNode *node);
void Node_Destroy(GraphNode *root_node);