/* Copyright (c) 2012, Chris Winter <wintercni@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>

#include <libcore/graph.h>
#include <libcore/graph-algorithms.h>

unsigned long* make_ulong_ptr(unsigned long value)
{
    unsigned long *val = NULL;

    val = malloc(sizeof(unsigned long));
    if(val != NULL) {
        *val = value;
    }

    return val;
}

/* a is greater than b if a is numerically greater than b */
int ulong_compare(const unsigned long *a, const unsigned long *b)
{
    if(*a > *b) {
        return 1;
    } else if(*a == *b) {
        return 0;
    } else {
        return -1;
    }
}

void process_vertex(Vertex *v, const GraphSearchCtx *ctx, void *userdata)
{
    printf("Discovered Vertex: %lu\n", *(unsigned long *)vertex_get_data(v));
}

void test_graph_undirected(void)
{
    Vertex *v1, *v2, *v3, *v4, *v5, *v6, *v7, *v8;
    unsigned long *components;
    DList *path, *back_edges;
    GraphSearchCtx *ctx;
    DListIterator *it;
    unsigned long i;
    Graph *g;

    printf("\nUndirected graph\n");
    printf("================\n");

    g = graph_create(GRAPH_TYPE_UNDIRECTED);

    v1 = vertex_create(make_ulong_ptr(1));
    v2 = vertex_create(make_ulong_ptr(2));
    v3 = vertex_create(make_ulong_ptr(3));
    v4 = vertex_create(make_ulong_ptr(4));
    v5 = vertex_create(make_ulong_ptr(5));
    v6 = vertex_create(make_ulong_ptr(6));

    /* For testing connected components */
    v7 = vertex_create(make_ulong_ptr(7));
    v8 = vertex_create(make_ulong_ptr(8));

    graph_vertex_add(g, v1);
    graph_vertex_add(g, v2);
    graph_vertex_add(g, v3);
    graph_vertex_add(g, v4);
    graph_vertex_add(g, v5);
    graph_vertex_add(g, v6);

    graph_vertex_add(g, v7);
    graph_vertex_add(g, v8);

    /*
     *   v6                  +---+
     *   |                   |   |
     *   v1---v2---v3---v4   v5--+
     *   |                   |
     *   +-------------------+
     *
     *   v7---v8
     */

    graph_edge_add(g, edge_create(v1, v2, 1.0));
    graph_edge_add(g, edge_create(v5, v1, 1.0));
    graph_edge_add(g, edge_create(v1, v6, 1.0));

    graph_edge_add(g, edge_create(v2, v3, 1.0));
    graph_edge_add(g, edge_create(v3, v4, 1.0));
    graph_edge_add(g, edge_create(v5, v5, 1.0));

    graph_edge_add(g, edge_create(v7, v8, 1.0));

    printf("Is bipartite: %d\n", graph_is_bipartite(g));

    printf("Breadth first search:\n");
    ctx = graph_breadth_first_search(g, v1, process_vertex, NULL, NULL, NULL);

    printf("\nPath from v1 to v4\n");
    path = graph_find_path(g, ctx->parent, v1, v4);
    printf("path size: %lu\n", dlist_size(path));
    for(it = dlist_begin(path); it != NULL; it = dlist_next(it)) {
        printf("%lu ", *(unsigned long*)vertex_get_data(dlist_get_data(it)));
    }
    printf("\n");
    dlist_free(path);

    graph_search_ctx_free(ctx);

    printf("\nConnected Components\n");
    components = graph_connected_components(g);
    for(i = 0; i < graph_vertex_count(g); i++) {
        printf("V: %lu component: %lu\n", i+1, components[i]);
    }
    free(components);


    printf("\nDepth first search:\n");
    ctx = graph_depth_first_search(g, v1, process_vertex, NULL, NULL, NULL);
    for(i = 0; i < graph_vertex_count(g); i++) {
        printf("V: %lu entry_time: %lu\n",
                *(unsigned long*)vertex_get_data(graph_get_vertex(g, i)),
                ctx->entry_time[i]);
        printf("V: %lu exit_time: %lu\n",
                *(unsigned long*)vertex_get_data(graph_get_vertex(g, i)),
                ctx->exit_time[i]);
    }

    printf("\nPath from v1 to v4\n");
    path = graph_find_path(g, ctx->parent, v1, v4);
    printf("path size: %lu\n", dlist_size(path));
    for(it = dlist_begin(path); it != NULL; it = dlist_next(it)) {
        printf("%lu ", *(unsigned long*)vertex_get_data(dlist_get_data(it)));
    }
    printf("\n");
    dlist_free(path);

    graph_search_ctx_free(ctx);

    back_edges = graph_find_back_edges(g);
    printf("Found %lu back edges\n", dlist_size(back_edges));
    dlist_free(back_edges);

    graph_free_all(g, NULL);
}

void test_graph_directed(void)
{
    Vertex *v1, *v2, *v3, *v4, *v5, *v6, *v7;
    GraphSearchCtx *ctx;
    DList *back_edges;
    Graph *g;
    unsigned long i;

    printf("\nDirected graph\n");
    printf("==============\n");

    g = graph_create(GRAPH_TYPE_DIRECTED);

    v1 = vertex_create(make_ulong_ptr(1));
    v2 = vertex_create(make_ulong_ptr(2));
    v3 = vertex_create(make_ulong_ptr(3));
    v4 = vertex_create(make_ulong_ptr(4));
    v5 = vertex_create(make_ulong_ptr(5));
    v6 = vertex_create(make_ulong_ptr(6));
    v7 = vertex_create(make_ulong_ptr(7));

    graph_vertex_add(g, v1);
    graph_vertex_add(g, v2);
    graph_vertex_add(g, v3);
    graph_vertex_add(g, v4);
    graph_vertex_add(g, v5);
    graph_vertex_add(g, v6);
    graph_vertex_add(g, v7);

    /* +-----------------------+
     * |     +-----+           |
     * v     v     |           |
     * v1--->v2--->v3--->v4--->v5<-+
     * |           |           |   |
     * |           |           +---+
     * v     v-----+
     * v6--->v7
     * ^     |
     * +-----+
     */
    graph_edge_add(g, edge_create(v1, v2, 1.0));
    graph_edge_add(g, edge_create(v2, v3, 1.0));
    graph_edge_add(g, edge_create(v3, v4, 1.0));
    graph_edge_add(g, edge_create(v4, v5, 1.0));

    /* Back edge #1 */
    graph_edge_add(g, edge_create(v3, v2, 1.0));

    /* Cross edge #1 */
    graph_edge_add(g, edge_create(v3, v7, 1.0));

    /* Back edge #2 */
    graph_edge_add(g, edge_create(v5, v1, 1.0));

    /* Back edge #2.5 */
    graph_edge_add(g, edge_create(v5, v5, 1.0));

    graph_edge_add(g, edge_create(v1, v6, 1.0));
    graph_edge_add(g, edge_create(v6, v7, 1.0));

    /* Back edge #3 */
    graph_edge_add(g, edge_create(v7, v6, 1.0));

    printf("Breadth first search\n");
    ctx = graph_breadth_first_search(g, graph_get_vertex(g, 0),
            process_vertex, NULL, NULL, NULL);
    graph_search_ctx_free(ctx);

    printf("\nDepth first search\n");
    ctx = graph_depth_first_search(g, graph_get_vertex(g, 0),
            process_vertex, NULL, NULL, NULL);
    for(i = 0; i < graph_vertex_count(g); i++) {
        printf("V: %lu entry_time: %lu\n",
                *(unsigned long*)vertex_get_data(graph_get_vertex(g, i)),
                ctx->entry_time[i]);
        printf("V: %lu exit_time: %lu\n",
                *(unsigned long*)vertex_get_data(graph_get_vertex(g, i)),
                ctx->exit_time[i]);
    }
    graph_search_ctx_free(ctx);

    back_edges = graph_find_back_edges(g);
    printf("Found %lu back edges\n", dlist_size(back_edges));
    dlist_free(back_edges);

    graph_free_all(g, NULL);
}

void test_graph_directed_acyclic(void)
{
    Vertex *v1, *v2, *v3, *v4, *v5, *v6, *v7, *v8;
    DListIterator *it;
    DList *sorted;
    Graph *g;

    printf("\nDirected Acylic Graph\n");
    printf("=====================\n");

    g = graph_create(GRAPH_TYPE_DIRECTED);

    v1 = vertex_create(make_ulong_ptr(1));
    v2 = vertex_create(make_ulong_ptr(2));
    v3 = vertex_create(make_ulong_ptr(3));
    v4 = vertex_create(make_ulong_ptr(4));
    v5 = vertex_create(make_ulong_ptr(5));
    v6 = vertex_create(make_ulong_ptr(6));
    v7 = vertex_create(make_ulong_ptr(7));
    v8 = vertex_create(make_ulong_ptr(8));

    graph_vertex_add(g, v1);
    graph_vertex_add(g, v2);
    graph_vertex_add(g, v3);
    graph_vertex_add(g, v4);
    graph_vertex_add(g, v5);
    graph_vertex_add(g, v6);
    graph_vertex_add(g, v7);
    graph_vertex_add(g, v8);

    /*
     * v8
     *
     * v1--->v2--->v3
     *       |     |
     *       |     |
     *       v     v
     *       v6--->v7--->v4--->v5
     *
     */
    graph_edge_add(g, edge_create(v1, v2, 1.0));
    graph_edge_add(g, edge_create(v2, v3, 1.0));
    graph_edge_add(g, edge_create(v2, v6, 1.0));
    graph_edge_add(g, edge_create(v3, v7, 1.0));
    graph_edge_add(g, edge_create(v6, v7, 1.0));
    graph_edge_add(g, edge_create(v7, v4, 1.0));
    graph_edge_add(g, edge_create(v4, v5, 1.0));

    printf("Topological sort\n");
    sorted = graph_topological_sort(g);

    for(it = dlist_begin(sorted); it != NULL; it = dlist_next(it)) {
        printf("%lu ", *(unsigned long *)vertex_get_data((Vertex *)dlist_get_data(it)));
    }
    printf("\n");

    graph_free_all(g, NULL);
}

void test_graph_weighted_prim(void)
{
    Vertex *v1, *v2, *v3, *v4, *v5, *v6, *v7;
    DListIterator *it;
    DList *mst_edges;
    Graph *g;

    printf("\nMinimum Spanning Tree - Prim\n");
    printf("============================\n");

    g = graph_create(GRAPH_TYPE_UNDIRECTED);

    v1 = vertex_create(make_ulong_ptr(1));
    v2 = vertex_create(make_ulong_ptr(2));
    v3 = vertex_create(make_ulong_ptr(3));
    v4 = vertex_create(make_ulong_ptr(4));
    v5 = vertex_create(make_ulong_ptr(5));
    v6 = vertex_create(make_ulong_ptr(6));
    v7 = vertex_create(make_ulong_ptr(7));

    graph_vertex_add(g, v1);
    graph_vertex_add(g, v2);
    graph_vertex_add(g, v3);
    graph_vertex_add(g, v4);
    graph_vertex_add(g, v5);
    graph_vertex_add(g, v6);
    graph_vertex_add(g, v7);

    /*
     * Graph, g:
     * =========
     *
     *  +--------v6----5---v7
     *  |        | \       |
     *  7        4  |_2__  2
     *  |        |       \ |
     *  v3---9---v4---3----v5
     *  |       / \        |
     *  5  __7_|   |__4__  7
     *  | /              \ |
     *  v1------12---------v2
     *
     *
     * Results for MST Prim(g, v1)
     * ===========================
     *
     * (v1, v3), (v1, v4), (v4, v2),
     * (v4, v5), (v5, v6), (v5, v7)
     *
     */
    graph_edge_add(g, edge_create(v1, v3, 5.0));
    graph_edge_add(g, edge_create(v1, v4, 7.0));
    graph_edge_add(g, edge_create(v1, v2, 12.0));
    graph_edge_add(g, edge_create(v2, v4, 4.0));
    graph_edge_add(g, edge_create(v2, v5, 7.0));
    graph_edge_add(g, edge_create(v3, v4, 9.0));
    graph_edge_add(g, edge_create(v3, v6, 7.0));
    graph_edge_add(g, edge_create(v4, v6, 7.0));
    graph_edge_add(g, edge_create(v4, v5, 3.0));
    graph_edge_add(g, edge_create(v5, v6, 2.0));
    graph_edge_add(g, edge_create(v5, v7, 2.0));
    graph_edge_add(g, edge_create(v6, v7, 5.0));

    mst_edges = graph_mst_prim(g, v1);

    for(it = dlist_begin(mst_edges); it != NULL; it = dlist_next(it)) {
        printf("(%lu, %lu)\n",
            *(unsigned long *)vertex_get_data(edge_get_source(dlist_get_data(it))),
            *(unsigned long *)vertex_get_data(edge_get_target(dlist_get_data(it))));
    }

    dlist_free(mst_edges);
    graph_free_all(g, NULL);
}

void test_graph_weighted_dijkstra(void)
{
    Vertex *v1, *v2, *v3, *v4, *v5, *v6, *v7;
    DListIterator *it;
    DArray *parent;
    DList *path;
    Graph *g;

    printf("\nShortest Path - Dijkstra\n");
    printf("========================\n");

    g = graph_create(GRAPH_TYPE_UNDIRECTED);

    v1 = vertex_create(make_ulong_ptr(1));
    v2 = vertex_create(make_ulong_ptr(2));
    v3 = vertex_create(make_ulong_ptr(3));
    v4 = vertex_create(make_ulong_ptr(4));
    v5 = vertex_create(make_ulong_ptr(5));
    v6 = vertex_create(make_ulong_ptr(6));
    v7 = vertex_create(make_ulong_ptr(7));

    graph_vertex_add(g, v1);
    graph_vertex_add(g, v2);
    graph_vertex_add(g, v3);
    graph_vertex_add(g, v4);
    graph_vertex_add(g, v5);
    graph_vertex_add(g, v6);
    graph_vertex_add(g, v7);

    /*
     * Graph, g:
     * =========
     *
     *  +--------v6----5---v7
     *  |        | \       |
     *  7        4  |_2__  2
     *  |        |       \ |
     *  v3---9---v4---3----v5
     *  |       / \        |
     *  5  __7_|   |__4__  7
     *  | /              \ |
     *  v1------12---------v2
     *
     * Results for Shortest Path Dijkstra(g, v1)
     * =========================================
     *
     * Path from v1 to v7:  v1 -> v4 -> v5 -> v7
     */
    graph_edge_add(g, edge_create(v1, v3, 5.0));
    graph_edge_add(g, edge_create(v1, v4, 7.0));
    graph_edge_add(g, edge_create(v1, v2, 12.0));
    graph_edge_add(g, edge_create(v2, v4, 4.0));
    graph_edge_add(g, edge_create(v2, v5, 7.0));
    graph_edge_add(g, edge_create(v3, v4, 9.0));
    graph_edge_add(g, edge_create(v3, v6, 7.0));
    graph_edge_add(g, edge_create(v4, v6, 7.0));
    graph_edge_add(g, edge_create(v4, v5, 3.0));
    graph_edge_add(g, edge_create(v5, v6, 2.0));
    graph_edge_add(g, edge_create(v5, v7, 2.0));
    graph_edge_add(g, edge_create(v6, v7, 5.0));

    graph_dijkstra(g, v1, &parent);

    path = graph_find_path(g, parent, v1, v7);

    for(it = dlist_begin(path); it != NULL; it = dlist_next(it)) {
        printf("%lu ", *(unsigned long *)vertex_get_data(dlist_get_data(it)));
    }
    printf("\n");

    dlist_free(path);
    darray_free(parent);

    graph_free_all(g, NULL);
}

int main(int argc, char *argv[])
{
    test_graph_undirected();
    test_graph_directed();
    test_graph_directed_acyclic();
    test_graph_weighted_prim();
    test_graph_weighted_dijkstra();

    return 0;
}
