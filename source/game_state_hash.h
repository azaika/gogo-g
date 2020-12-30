#ifndef GOGO_HEADER_GAMESTATEHASH_H
#define GOGO_HEADER_GAMESTATEHASH_H

#include "game_state.h"

#include <stdio.h>
#include <stdlib.h>
#define TABLE_NUMBER 10000

//alpha-betaの枝刈りのために二分探索木を作りました

typedef struct node
{
    game_state_hash hash;
    int depth;
    int value;
    struct node *next;
} Node;

static void chain_insert(Node **node, game_state_hash hash, int depth, int value)
{
    if ((*node) == NULL)
    {
        (*node) = (Node *)malloc(sizeof(Node));
        (*node)->hash = hash;
        (*node)->depth = depth;
        (*node)->value = value;
        (*node)->next = NULL;
    }
    else if ((*node)->hash == hash)
    {
        (*node)->depth = depth;
        (*node)->value = value;
    }
    else if ((*node)->next != NULL)
    {
        chain_insert(&((*node)->next), hash, depth, value);
    }
    else
    {
        Node *newNode;
        newNode = (Node *)malloc(sizeof(Node));
        (*node)->hash = hash;
        (*node)->depth = depth;
        (*node)->value = value;
        newNode->next = NULL;
        (*node)->next = newNode;
    }
}

/*static void hash_delete(Node **node, game_state_hash hash)
{
    if ((*node) != NULL)
    {
        if ((*node)->next != NULL && hash == (*node)->next->hash)
        {
            Node *tmp = (*node)->next;
            (*node)->next = tmp->next;
            free(tmp);
        }
        else if ((*node)->next != NULL)
        {
            Delete(&((*node)->next), hash);
        }
    }
}*/

static int chain_search_value(Node *node, game_state_hash hash)
{
    if (node != NULL)
    {
        if (node->hash == hash)
        {
            return node->value;
        }
        else
        {
            return chain_search_value(node->next, hash);
        }
    }
    else
    {
        return -1;
    }
}

static int chain_search_depth(Node *node, game_state_hash hash)
{
    if (node != NULL)
    {
        if (node->hash == hash)
        {
            return node->depth;
        }
        else
        {
            return chain_search_depth(node->next, hash);
        }
    }
    else
    {
        return -1;
    }
}

static int return_hash(game_state_hash hash)
{
    return hash % TABLE_NUMBER;
}

typedef Node*(hash_table)[TABLE_NUMBER];

static void hash_insert(hash_table table, game_state_hash hash, int depth, int value){
    int key = return_hash(hash);
    chain_insert(&table[key], hash, depth, value);
}

static int hash_search_value(hash_table table, game_state_hash hash){
    int key = return_hash(hash);
    return chain_search_value(table[key], hash);
}

static int hash_search_depth(hash_table table, game_state_hash hash){
    int key = return_hash(hash);
    return chain_search_depth(table[key], hash);
}

#endif