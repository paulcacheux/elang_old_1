/* dijkstra.c57.  C57 code to run Dijkstra's algorithm on a small
   directed graph.  Uses a min-heap as the priority queue. */

/* Return the index of the parent of node i. */
func parent(i: int) -> int {
    return i / 2;
}

/* Return the index of the left child of node i. */
func left(i: int) -> int {
    return 2 * i;
}

/* Return the index of the right child of node i. */
func right(i: int) -> int {
    return 2 * i + 1;
}

/* Exchange nodes i and j, updating their keys, handles, and
   heap_index values. */
func exchange(key: *double, handle: *int, heap_index: *int, i: int, j: int) {
    let key_temp: double;
    let handle_temp: int;

    /* Exchange the keys in nodes i and j. */
    key_temp = key[i];
    key[i] = key[j];
    key[j] = key_temp;

    /* Exchange the handles in nodes i and j. */
    handle_temp = handle[i];
    handle[i] = handle[j];
    handle[j] = handle_temp;

    /* Update the heap_index values. */
    heap_index[handle[i]] = i;
    heap_index[handle[j]] = j;
}

/* Make the min-heap rooted at node i obey the min-heap property.
   Assumes that the subtrees rooted at i's left and right children
   already obey the min-heap property. */
void heapify(double key[], int handle[], int heap_index[], int i, int size) {
    int l = left(i);
    int r = right(i);
    int smallest;

    /* Is the left child smaller than node i? */
    if (l <= size && key[l] < key[i])
        smallest = l;
    else
        smallest = i;

    /* Is the right child smaller than node i and i's left child? */
    if (r <= size && key[r] < key[smallest])
        smallest = r;

    /* If the min-heap property is violated between node i and one of
       its children, fix it. */
    if (smallest != i) {
        exchange(key, handle, heap_index, i, smallest);
        heapify(key, handle, heap_index, smallest, size);
    }
}

/* Take an array that does not necessarily obey the min-heap property,
   and rearrange it so that it does. */
void build_heap(double key[], int handle[], int heap_index[], int size) {
    int i;
    for (i = size / 2; i >= 1; --i)
        heapify(key, handle, heap_index, i, size);
}

/* Extract the node with the minimum key, at index 1, from the
   min-heap. */
void extract_min(double key[], int handle[], int heap_index[], int size) {
    exchange(key, handle, heap_index, 1, size);
    heapify(key, handle, heap_index, 1, size - 1);
}

/* Bubble the key in node i up toward the root until the min-heap
   property is restored. */
void decrease_key(double key[], int handle[], int heap_index[], int i, int size,
                  double new_key) {
    key[i] = new_key;
    while (i > 1 && key[parent(i)] > key[i]) {
        exchange(key, handle, heap_index, i, parent(i));
        i = parent(i);
    }
}

/* Insert a new node into the min-heap.  Assumes that an array element
   has already been allocated. */
void insert(double key[], int handle[], int heap_index[], int vertex, int size,
            double new_key) {
    key[++size] = 1000000000.0; /* will be fixed later */
    handle[size] = vertex;
    heap_index[vertex] = size;
    decrease_key(key, handle, heap_index, size, size,
                 new_key); /* here's later */
}

/* Relax edge (u, v) with weight w. */
void relax(int u, int v, double w, double key[], int handle[], int heap_index[],
           int size, int pi[]) {
    if (key[heap_index[v]] > key[heap_index[u]] + w) {
        decrease_key(key, handle, heap_index, heap_index[v], size,
                     key[heap_index[u]] + w);
        pi[v] = u;
    }
}

/* Initialize a single-source shortest-paths computation. */
void initialize_single_source(double key[], int handle[], int heap_index[],
                              int pi[], int s, int n) {
    int i;
    for (i = 1; i <= n; ++i) {
        key[i] = 1000000000.0;
        handle[i] = i;
        heap_index[i] = i;
        pi[i] = 0;
    }

    key[s] = 0.0;
    build_heap(key, handle, heap_index, n);
}

/* Run Dijkstra's algorithm from vertex s.  Fills in arrays d and pi. */
void dijkstra(int first[], int node[], int next[], double w[], double d[],
              int pi[], int s, int n, int handle[], int heap_index[]) {
    int size = n;
    int u, v, i;

    initialize_single_source(d, handle, heap_index, pi, s, n);
    while (size > 0) {
        u = handle[1];
        extract_min(d, handle, heap_index, size);
        --size;
        i = first[u];
        while (i > 0) {
            v = node[i];
            relax(u, v, w[i], d, handle, heap_index, size, pi);
            i = next[i];
        }
    }
}

/* Set up a directed graph and run Dijkstra's algorithm on it.  Print
   out the result. */
void main(void) {
    int first[6], node[11], next[11], pi[6], handle[6], heap_index[6];
    double w[11], d[6];
    int s;
    int i;

    /* The graph contains the following directed edges with weights:
       (1, 2), weight 10
       (1, 4), weight 5
       (2, 3), weight 1
       (2, 4), weight 2
       (3, 5), weight 4
       (4, 2), weight 3
       (4, 3), weight 9
       (4, 5), weight 2
       (5, 1), weight 7
       (5, 3), weight 6
    */

    first[1] = 1;
    first[2] = 3;
    first[3] = 5;
    first[4] = 6;
    first[5] = 9;

    node[1] = 2;
    node[2] = 4;
    node[3] = 3;
    node[4] = 4;
    node[5] = 5;
    node[6] = 2;
    node[7] = 3;
    node[8] = 5;
    node[9] = 1;
    node[10] = 3;

    w[1] = 10.0;
    w[2] = 5.0;
    w[3] = 1.0;
    w[4] = 2.0;
    w[5] = 4.0;
    w[6] = 3.0;
    w[7] = 9.0;
    w[8] = 2.0;
    w[9] = 7.0;
    w[10] = 6.0;

    next[1] = 2;
    next[2] = 0;
    next[3] = 4;
    next[4] = 0;
    next[5] = 0;
    next[6] = 7;
    next[7] = 8;
    next[8] = 0;
    next[9] = 10;
    next[10] = 0;

    print "Enter source node: ";
    read s;

    dijkstra(first, node, next, w, d, pi, s, 5, handle, heap_index);

    for (i = 1; i <= 5; ++i) {
        print i;
        print ": ";
        print d[heap_index[i]];
        print "  ";
        print pi[i];
        print "\n";
    }
}

/* Correct outputs (format might not be exactly the same):

   Enter source node: 1
   1: 0.000000  0
   2: 8.000000  4
   3: 9.000000  2
   4: 5.000000  1
   5: 7.000000  4

   Enter source node: 2
   1: 11.000000  5
   2: 0.000000  0
   3: 1.000000  2
   4: 2.000000  2
   5: 4.000000  4

   Enter source node: 3
   1: 11.000000  5
   2: 19.000000  4
   3: 0.000000  0
   4: 16.000000  1
   5: 4.000000  3

   Enter source node: 4
   1: 9.000000  5
   2: 3.000000  4
   3: 4.000000  2
   4: 0.000000  0
   5: 2.000000  4

   Enter source node: 5
   1: 7.000000  5
   2: 15.000000  4
   3: 6.000000  5
   4: 12.000000  1
   5: 0.000000  0
*/
