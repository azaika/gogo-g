#ifndef GOGO_HEADER_GAMESTATEHASH_H
#define GOGO_HEADER_GAMESTATEHASH_H

#include "game_state.h"

#include <stdio.h>
#include <stdlib.h>
#define TABLE_NUMBER 10000

//alpha-betaの枝刈りのためにhash tableを作りました

typedef struct node
{
    game_state_hash hash;
    int depth;
    int value;
    move_type move;
    struct node *next;
} Node;

static void chain_insert(Node **node, game_state_hash hash, int depth, int value, move_type move)
{
    if ((*node) == NULL)
    {
        (*node) = (Node *)malloc(sizeof(Node));
        (*node)->hash = hash;
        (*node)->depth = depth;
        (*node)->value = value;
        (*node)->move = move;
        (*node)->next = NULL;
    }
    else if ((*node)->hash == hash)
    {
        (*node)->depth = depth;
        (*node)->value = value;
        (*node)->move = move;
    }
    else if ((*node)->next != NULL)
    {
        chain_insert(&((*node)->next), hash, depth, value, move);
    }
    else
    {
        Node *newNode;
        newNode = (Node *)malloc(sizeof(Node));
        (*node)->hash = hash;
        (*node)->depth = depth;
        (*node)->value = value;
        (*node)->move = move;
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

static move_type chain_search_move(Node *node, game_state_hash hash)
{
    assert(node != NULL);
    if (node->hash == hash)
    {
        return node->move;
    }
    else
    {
        return chain_search_move(node->next, hash);
    }
}

static int return_hash(game_state_hash hash)
{
    return hash % TABLE_NUMBER;
}

typedef Node *(hash_table)[TABLE_NUMBER];

static void hash_insert(hash_table table, game_state_hash hash, int depth, int value, move_type move)
{
    int key = return_hash(hash);
    chain_insert(&table[key], hash, depth, value, move);
}

static int hash_search_value(hash_table table, game_state_hash hash)
{
    int key = return_hash(hash);
    return chain_search_value(table[key], hash);
}

static int hash_search_depth(hash_table table, game_state_hash hash)
{
    int key = return_hash(hash);
    return chain_search_depth(table[key], hash);
}

static move_type hash_search_move(hash_table table, game_state_hash hash)
{
    int key = return_hash(hash);
    return chain_search_move(table[key], hash);
}

#endif