#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

/*

#define MAP(var) MAPname ## var

typedef int MAP(key);
typedef float MAP(payload);

int MAP(cmp)(MAP(key) first, MAP(key) second) {
	if(first == second) return 0;
	if(first < second) return -1;
	return 1; }

#include "MAP.c"

#undef MAP

*/

struct MAP () {
	struct MAP () * left, *right, *parent;
	MAP(key) key;
	MAP(payload) payload;
};

struct MAP () ** MAP(find) (struct MAP() ** n, MAP(key) * key) {
	if (*n) {
		int r = MAP(cmp) (&(*n)->key, key);

		if (r < 0) {
			return MAP(find) (&((*n)->left), key);
		}
		if (r > 0) {
			return MAP(find) (&((*n)->right), key);
		}
	}

	return n;
}

struct MAP () * MAP(next) (struct MAP() * n) {
	if (n) {
		if (n->right) {
			n = n->right;
			while(n->left) {
				n = n->left;
			}
			return n;
		} else if (n->parent) {
			struct MAP() * o = n;
			if(n->parent->right == n) {
				while(n->parent && n->parent->right == n) {
					n = n->parent;
				}

				if(n->parent) {
					return n->parent;
				}
				return o;
			}
			return n->parent;
		}

	}
	return 0;
}

struct MAP () * MAP(findnext) (struct MAP() * n, MAP(key) * key) {
	if (n) {
		int r = MAP(cmp) (&n->key, key);

		if (r < 0) {
			if (n->left) {
				return MAP(findnext) (n->left, key);
			}

		}
		if (r > 0) {
			if (n->right)
				return MAP(findnext) (n->right, key);

		}
	}

	return n;
}

struct MAP () * MAP(alloc) () {
	return malloc(sizeof(struct MAP ()));
}

void MAP(free) (struct MAP() * n) {
	free(n);
}

void MAP(insert_proper) (struct MAP() * parent,
			 struct MAP() ** place,
			 MAP(key) * key, MAP(payload) * payload) {
	*place = MAP(alloc) ();
	**place = (struct MAP()) {
	0, 0, parent, *key, *payload};
}

void MAP(insert) (struct MAP() ** n, MAP(key) * key, MAP(payload) * payload) {
	if (*n) {
		int r = MAP(cmp) (&(*n)->key, key);

#define MAPINSERT(left) \
if((*n)->left == 0) \
{ \
	MAP(insert_proper)(*n, &((*n)->left), key, payload); \
} \
else return MAP(insert)(&((*n)->left), key, payload);

		if (r < 0) {
			MAPINSERT(left)
		}
		if (r > 0) {
			MAPINSERT(right)
		}
#undef MAPINSERT

	} else {
		MAP(insert_proper) (0, n, key, payload);
	}
}

typedef void (*MAP(fnctor)) (struct MAP() * n, void *);

void MAP(foreach) (struct MAP() ** n, MAP(fnctor) fn, void *data) {

	if (*n) {
		MAP(foreach) (&((*n)->left), fn, data);
		fn(*n, data);
		MAP(foreach) (&((*n)->right), fn, data);
	}
}

void MAP(delete_node) (struct MAP() ** n) {
	struct MAP () * parent = (*n)->parent;
	struct MAP () * left = (*n)->left;
	struct MAP () * right = (*n)->right;
	MAP(free) (*n);

	if (left == 0 && right == 0) {
		*n = 0;
	} else if (left == 0) {
		*n = right;
		(*n)->parent = parent;
	} else if (right == 0) {
		*n = left;
		(*n)->parent = parent;
	} else {
		struct MAP () ** tmp = &right;

		while ((*tmp)->left != 0) {
			tmp = &(*tmp)->left;
		}

		struct MAP () * t = *tmp;

		*tmp = t->right;

		if (*tmp) {
			(*tmp)->parent = t->parent;
		}

		t->right = right;
		t->left = left;
		*n = t;
		(*n)->parent = parent;
		right->parent = *n;
		left->parent = *n;
	}
}

void MAP(delete) (struct MAP() ** n, MAP(key) * key) {

	struct MAP () ** m = MAP(find) (n, key);

	if (*m)
		MAP(delete_node) (m);
}
