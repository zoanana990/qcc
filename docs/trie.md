# trie

trie is a structure in compiler, and the data structure of trie is designed as follow,
the following code can also solve [leetcode 208](https://leetcode.com/problems/implement-trie-prefix-tree/description/)

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define TRIE_LEN            (26)

typedef struct trie{
    struct trie *c[TRIE_LEN];
    bool e;    
} Trie;


Trie* trieCreate() {
    Trie *node = malloc(sizeof(Trie));
    
    memset(node, 0, sizeof(Trie));
    return node;
}

void trieInsert(Trie* obj, char* word) {
    Trie *node = obj;

    while(*word) {
        int i = *word - 'a';
        if(!node->c[i])
            node->c[i] = trieCreate();
        node = node->c[i];
        word++;
    }

    node->e = true;
}

bool trieSearch(Trie* obj, char* word) {
    Trie *node = obj;

    while(*word) {
        int i = *word - 'a';
        if(!node->c[i])
            return false;
        node = node->c[i];
        word++;
    }

    return node->e;
}

bool trieStartsWith(Trie* obj, char* prefix) {
    Trie *node = obj;

    while(*prefix) {
        int i = *prefix - 'a';
        if(!node->c[i])
            return false;
        node = node->c[i];
        prefix++;
    }

    return true;
}

void trieFree(Trie* obj) {
    if(obj == NULL)
        return;
    
    if(obj->e){
        for(int i = 0; (i < TRIE_LEN); i++)
            trieFree(obj->c[i]);
    }

    free(obj);
}

int main() {
    struct trie* root = trieCreate();

    trieInsert(root, "apple");
    trieInsert(root, "app");
    trieInsert(root, "banana");
    trieInsert(root, "bat");

    printf("Search 'apple': %s\n",  trieSearch(root, "apple") ? "Found" : "Not Found");
    printf("Search 'app': %s\n",    trieSearch(root, "app") ? "Found" : "Not Found");
    printf("Search 'banana': %s\n", trieSearch(root, "banana") ? "Found" : "Not Found");
    printf("Search 'batman': %s\n", trieSearch(root, "batman") ? "Found" : "Not Found");

    trieFree(root);

    return 0;
}
```