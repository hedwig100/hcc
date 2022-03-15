
int printf(const char *__format, ...);
void *calloc(int __nmemb, int __size);
void free(void *__ptr);
void exit(int __status);

#define NULL 0
#define bool int
#define true 1
#define false 0

typedef struct HashData HashData;
typedef struct Hash Hash;

struct HashData {
    int value;
    HashData *next;
    HashData *prev;
};

struct Hash {
    int size;
    int a;
    int b;
    HashData **table;
};

int calc_hash(int value, int hash_size, int a, int b) {
    return (a * value + b) % hash_size;
}

HashData *make_hash_data(int value) {
    HashData *hd = calloc(1, sizeof(HashData));
    if (hd == NULL) exit(1);
    hd->value = value;
    hd->next  = NULL;
    hd->prev  = NULL;
    return hd;
}

Hash *make_hash(int n, int a, int b) {
    Hash *h = calloc(1, sizeof(Hash));
    if (h == NULL) exit(1);
    h->size = n;
    h->a    = a;
    h->b    = b;

    HashData **table = calloc(n, 8);
    if (table == NULL) exit(1);
    for (int i = 0; i < n; i++) {
        table[i] = NULL;
    }
    h->table = table;
    return h;
}

void hash_insert(Hash *h, HashData *hd) {
    int key = calc_hash(hd->value, h->size, h->a, h->b);
    if (h->table[key]) {
        hd->next            = h->table[key];
        h->table[key]->prev = hd;
        h->table[key]       = hd;
    } else {
        h->table[key] = hd;
    }
}

bool hash_search(Hash *h, int val) {
    int key = calc_hash(val, h->size, h->a, h->b);
    if (h->table[key]) {
        for (HashData *p = h->table[key]; p != NULL; p = p->next) {
            if (p->value == val) {
                return true;
            }
        }
    } else {
        return false;
    }
    return false;
}

void hash_delete(Hash *h, int val) {
    int key = calc_hash(val, h->size, h->a, h->b);
    for (HashData *p = h->table[key]; p != NULL; p = p->next) {
        if (p->value != val) continue;
        if (p->next == NULL && p->prev == NULL) {
            h->table[key] = NULL;
            free(p);
            return;
        } else if (p->next == NULL) {
            (p->prev)->next = NULL;
            free(p);
            return;
        } else if (p->prev == NULL) {
            h->table[key]->next = p->next;
            free(p);
            return;
        } else {
            (p->prev)->next = p->next;
            (p->next)->prev = p->prev;
            free(p);
            return;
        }
    }
}

int hash_count_data(Hash *h) {
    int count = 0;
    for (int key = 0; key < h->size; key++) {
        for (HashData *p = h->table[key]; p != NULL; p = p->next) {
            ++count;
        }
    }
    return count;
}

int main() {
    Hash *hash = make_hash(1009, 7, 5);
    printf("start! number of data: %d\n", hash_count_data(hash));
    for (int i = 0; i < 10; i++) {
        hash_insert(hash, make_hash_data(i));
        printf("       INSERT %d!\n", i);
        printf("       number of data: %d\n", hash_count_data(hash));
    }
    for (int i = 0; i < 10; i += 2) {
        if (hash_search(hash, i)) {
            hash_delete(hash, i);
            printf("       DELETE %d!\n", i);
        }
        printf("       number of data: %d\n", hash_count_data(hash));
    }
    printf("finished!\n");
    return 0;
}