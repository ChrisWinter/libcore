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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libcore/darray.h>
#include <libcore/dlist.h>
#include <libcore/queue.h>
#include <libcore/stack.h>
#include <libcore/graph-algorithms.h>

#define DEBUG   0
#define DBG(...) \
    do { if(DEBUG) fprintf(stderr, __VA_ARGS__); } while(0)

/* These algorithms are loosely based on the ideas and descriptions
 * given in "The Algorithm Design Manual", Second Edition, by Steven
 * S. Skiena (Springer, 2008). The code in this book is unreliable since
 * it is riddled with many bugs.
 */

enum _search_type {
    BFS,
    DFS
};

static GraphSearchCtx* _graph_search_ctx_create(const Graph *g,
        enum _search_type search_type)
{
    GraphSearchCtx *ctx;
    unsigned long index;

    ctx = malloc(sizeof(GraphSearchCtx));
    if(NULL == ctx) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    ctx->g = (Graph *)g;

    ctx->stop_search = 0;

    ctx->discovered = malloc(sizeof(int) * graph_vertex_count(g));
    if(NULL == ctx->discovered) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        free(ctx);
        return NULL;
    }

    ctx->processed = malloc(sizeof(int) * graph_vertex_count(g));
    if(NULL == ctx->processed) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        free(ctx);
        return NULL;
    }

    ctx->entry_time = NULL;
    ctx->exit_time = NULL;

    if(DFS == search_type) {
        ctx->entry_time = malloc(sizeof(unsigned long) * graph_vertex_count(g));
        if(NULL == ctx->entry_time) {
            fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
            free(ctx);
            return NULL;
        }

        ctx->exit_time = malloc(sizeof(unsigned long) * graph_vertex_count(g));
        if(NULL == ctx->exit_time) {
            fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
            free(ctx);
            return NULL;
        }
    }

    ctx->parent = darray_create_size(graph_vertex_count(g));
    if(NULL == ctx->parent) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        free(ctx);
        return NULL;
    }

    for(index = 0; index < graph_vertex_count(g); index++) {
        ctx->discovered[index] = 0;
        ctx->processed[index]  = 0;

        if(DFS == search_type) {
            ctx->entry_time[index] = 0;
            ctx->exit_time[index]  = 0;
        }

        darray_replace(ctx->parent, index, NULL);
    }

    return ctx;
}

void graph_search_ctx_free(GraphSearchCtx *ctx)
{
    assert(ctx != NULL);

    free(ctx->discovered);
    free(ctx->processed);
    free(ctx->entry_time);
    free(ctx->exit_time);
    darray_free(ctx->parent);
}

GraphSearchCtx* graph_breadth_first_search(const Graph *g,
        const Vertex                *start,
        GraphProcessVertexEarlyFn   vertex_early_fn,
        GraphProcessEdgeFn          edge_fn,
        GraphProcessVertexLateFn    vertex_late_fn,
        void                        *userdata)
{
    unsigned long index;
    GraphSearchCtx *ctx;
    Vertex *v, *succ;
    Queue *q;
    Edge *e;

    assert(g != NULL);

    ctx = _graph_search_ctx_create(g, BFS);
    if(NULL == ctx) {
        fprintf(stderr, "Failed to create graph search context (%s:%d)\n",
                __FUNCTION__, __LINE__);
        return NULL;
    }

    q = queue_create();
    queue_enqueue(q, (Vertex *)start);

    /* Mark start vertex as discovered */
    ctx->discovered[vertex_get_index(start)] = 1;

    while(!queue_is_empty(q)) {
        v = (Vertex *)queue_dequeue(q);

        if(vertex_early_fn != NULL) {
            vertex_early_fn(v, ctx, userdata);

            if(ctx->stop_search) {
                goto done;
            }
        }

        for(index = 0; index < vertex_edge_count(v); index++) {
            e = (Edge *)darray_index(vertex_get_edges(v), index);
            succ = edge_get_target(e);

            if((!ctx->processed[vertex_get_index(succ)] &&
                        darray_index(ctx->parent, vertex_get_index(v)) != succ) ||
                    graph_is_directed(g)) {
                if(edge_fn != NULL) {
                    edge_fn(e, ctx, userdata);

                    if(ctx->stop_search) {
                        goto done;
                    }
                }
            }

            if(!ctx->discovered[vertex_get_index(succ)]) {
                ctx->discovered[vertex_get_index(succ)] = 1;
                /* Set parent of succ as v */
                darray_replace(ctx->parent, vertex_get_index(succ), v);
                queue_enqueue(q, succ);
            }
        }

        if(vertex_late_fn != NULL) {
            vertex_late_fn(v, ctx, userdata);

            if(ctx->stop_search) {
                goto done;
            }
        }

        /* Mark vertex v as processed */
        ctx->processed[vertex_get_index(v)] = 1;
    }

done:
    queue_free(q);

    return ctx;
}

GraphSearchCtx* graph_depth_first_search(const Graph *g,
        const Vertex                *start,
        GraphProcessVertexEarlyFn   vertex_early_fn,
        GraphProcessEdgeFn          edge_fn,
        GraphProcessVertexLateFn    vertex_late_fn,
        void                        *userdata)
{
    unsigned long index, time;
    GraphSearchCtx *ctx;
    Vertex *v, *succ;
    Stack *s;
    Edge *e;

    assert(g != NULL);

    ctx = _graph_search_ctx_create(g, DFS);
    if(NULL == ctx) {
        fprintf(stderr, "Failed to create graph search context (%s:%d)\n",
                __FUNCTION__, __LINE__);
        return NULL;
    }

    s = stack_create();
    stack_push(s, (Vertex *)start);

    time = 1;

    while(!stack_is_empty(s)) {
        v = (Vertex *)stack_top(s);

        if(!ctx->discovered[vertex_get_index(v)]) {
            /* Mark v as discovered */
            ctx->discovered[vertex_get_index(v)] = 1;
            ctx->entry_time[vertex_get_index(v)] = time++;

            if(vertex_early_fn != NULL) {
                vertex_early_fn(v, ctx, userdata);

                if(ctx->stop_search) {
                    goto done;
                }
            }

            for(index = 0; index < vertex_edge_count(v); index++) {
                e = (Edge *)darray_index(vertex_get_edges(v), index);
                succ = edge_get_target(e);

                if(!ctx->discovered[vertex_get_index(succ)]) {
                    /* Set parent of succ as v */
                    darray_replace(ctx->parent, vertex_get_index(succ), v);
                    stack_push(s, succ);
                }

                /* Process the edge */
                if((!ctx->processed[vertex_get_index(succ)] &&
                            darray_index(ctx->parent, vertex_get_index(v)) != succ) ||
                        graph_is_directed(g)) {
                    if(edge_fn != NULL) {
                        edge_fn(e, ctx, userdata);

                        if(ctx->stop_search) {
                            goto done;
                        }
                    }
                }
            }
        }

        /* v should still be at the top of the stack
         * if there are no more descendants to visit
         */
        if(v == (Vertex *)stack_top(s)) {
            stack_pop(s);

            if(vertex_late_fn != NULL) {
                vertex_late_fn(v, ctx, userdata);

                if(ctx->stop_search) {
                    goto done;
                }
            }

            /* Mark vertex v as processed */
            ctx->processed[vertex_get_index(v)] = 1;
            ctx->exit_time[vertex_get_index(v)] = time++;
        }
    }

done:
    stack_free(s);

    return ctx;
}

static void _graph_find_path(const Graph *g, GraphSearchCtx *ctx,
        Vertex *start, Vertex *end, DList *path)
{
    if((start == end) || (NULL == end)) {
        dlist_append(path, start);
    } else {
        _graph_find_path(g,
                ctx,
                start,
                darray_index(ctx->parent, vertex_get_index(end)),
                path);
        dlist_append(path, end);
    }
}

DList* graph_find_path(const Graph *g, GraphSearchCtx *ctx,
        Vertex *start, Vertex* end)
{
    DList *path;

    assert(g != NULL);
    assert(ctx != NULL);
    assert(start != NULL);
    assert(end != NULL);

    path = dlist_create();
    if(NULL == path) {
        fprintf(stderr, "Failed to create path list (%s:%d)\n",
                __FUNCTION__, __LINE__);
        return NULL;
    }

    _graph_find_path(g, ctx, start, end, path);

    return path;
}

unsigned long* graph_connected_components(const Graph *g)
{
    GraphSearchCtx *ctx;
    unsigned long *comp;
    unsigned long c, i;
    Vertex *v;

    assert(g != NULL);

    comp = malloc(sizeof(unsigned long) * graph_vertex_count(g));
    if(NULL == comp) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    /* v_0 is always undiscovered and in the first
     * connected component
     */
    c = 1;
    comp[0] = c;
    v = graph_get_vertex(g, 0);
    ctx = graph_breadth_first_search(g, v, NULL, NULL, NULL, NULL);

    for(i = 1; i < graph_vertex_count(g); i++) {
        if(!ctx->discovered[i]) {
            c++;
            graph_search_ctx_free(ctx);
            v = graph_get_vertex(g, i);
            ctx = graph_breadth_first_search(g, v, NULL, NULL, NULL, NULL);
        }
        comp[i] = c;
    }

    graph_search_ctx_free(ctx);

    return comp;
}


typedef enum {
    VERTEX_COLOR_UNCOLORED,
    VERTEX_COLOR_WHITE,
    VERTEX_COLOR_BLACK
} _VertexColor;

typedef struct {
    _VertexColor *color;
    int bipartite;
} _GraphBipartiteCtx;

static int _vertex_color_complement(_VertexColor color)
{
    if(VERTEX_COLOR_WHITE == color) return VERTEX_COLOR_BLACK;
    if(VERTEX_COLOR_BLACK == color) return VERTEX_COLOR_WHITE;

    return VERTEX_COLOR_UNCOLORED;
}

static void _graph_is_bipartite_process_edge(Edge *e,
        const GraphSearchCtx *ctx, void *userdata)
{
    _GraphBipartiteCtx *b_ctx;
    Vertex *v, *w;

    b_ctx = (_GraphBipartiteCtx *)userdata;
    v = edge_get_source(e);
    w = edge_get_target(e);

    if(b_ctx->color[vertex_get_index(v)] == b_ctx->color[vertex_get_index(w)]) {
        b_ctx->bipartite = 0;
    }

    b_ctx->color[vertex_get_index(w)] =
            _vertex_color_complement(b_ctx->color[vertex_get_index(v)]);
}

int graph_is_bipartite(const Graph *g)
{
    _GraphBipartiteCtx b_ctx;
    GraphSearchCtx *ctx;
    unsigned long i;

    assert(g != NULL);

    b_ctx.color = malloc(sizeof(_VertexColor) * graph_vertex_count(g));
    if(NULL == b_ctx.color) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return 0;
    }

    /* Initialize colors to 0 (uncolored) */
    for(i = 0; i < graph_vertex_count(g); i++) {
        b_ctx.color[i] = VERTEX_COLOR_UNCOLORED;
    }

    b_ctx.bipartite = 1;

    b_ctx.color[0] = VERTEX_COLOR_WHITE;
    ctx = graph_breadth_first_search(g, graph_get_vertex(g, 0),
            NULL, _graph_is_bipartite_process_edge, NULL, &b_ctx);

    for(i = 1; i < graph_vertex_count(g); i++) {
        if(!ctx->discovered[i]) {
            b_ctx.color[i] = VERTEX_COLOR_WHITE;
            graph_search_ctx_free(ctx);
            ctx = graph_breadth_first_search(g, graph_get_vertex(g, i),
                    NULL, _graph_is_bipartite_process_edge, NULL, &b_ctx);
        }
    }

    graph_search_ctx_free(ctx);

    free(b_ctx.color);

    return b_ctx.bipartite;
}


EDGE_TYPE graph_classify_edge(const Edge *e, const GraphSearchCtx *ctx)
{
    Vertex *s, *t, *parent;

    assert(e != NULL);
    assert(ctx != NULL);

    s = edge_get_source(e);
    t = edge_get_target(e);
    parent = darray_index(ctx->parent, vertex_get_index(t));

    DBG("\nEdge from %lu to %lu\n", *(unsigned long *)vertex_get_data(s),
            *(unsigned long *)vertex_get_data(t));
    DBG("\tVertex %lu discovered: %d\n", *(unsigned long *)vertex_get_data(s),
            ctx->discovered[vertex_get_index(s)]);
    DBG("\tVertex %lu processed: %d\n", *(unsigned long *)vertex_get_data(s),
            ctx->processed[vertex_get_index(s)]);
    DBG("\tVertex %lu discovered: %d\n", *(unsigned long *)vertex_get_data(t),
            ctx->discovered[vertex_get_index(t)]);
    DBG("\tVertex %lu processed: %d\n", *(unsigned long *)vertex_get_data(t),
            ctx->processed[vertex_get_index(t)]);

    if(darray_index(ctx->parent, vertex_get_index(t)) == s) {
        DBG("\tEdge type: TREE\n");
        return EDGE_TYPE_TREE;
    }
    if(ctx->discovered[vertex_get_index(t)] &&
            !ctx->processed[vertex_get_index(t)]) {
        DBG("\tEdge type: BACK\n");
        return EDGE_TYPE_BACK;
    }
    if(ctx->processed[vertex_get_index(t)] &&
            (ctx->entry_time[vertex_get_index(t)] >
             ctx->entry_time[vertex_get_index(s)])) {
        DBG("\tEdge type: FORWARD\n");
        return EDGE_TYPE_FORWARD;
    }
    if(ctx->processed[vertex_get_index(t)] &&
            (ctx->entry_time[vertex_get_index(t)] <
             ctx->entry_time[vertex_get_index(s)])) {
        DBG("\tEdge type: CROSS\n");
        return EDGE_TYPE_CROSS;
    }

    DBG("\tEdge type: UNKNOWN\n");
    return EDGE_TYPE_UNKNOWN;
}

static void _graph_find_back_edges_process_edge(Edge *e,
        const GraphSearchCtx *ctx, void *userdata)
{
    DList *back_edges = (DList *)userdata;

    if(EDGE_TYPE_BACK == graph_classify_edge(e, ctx)) {
        dlist_append(back_edges, e);
    }
}

DList* graph_find_back_edges(const Graph *g)
{
    GraphSearchCtx *ctx;
    DList *back_edges;

    assert(g != NULL);

    back_edges = dlist_create();
    if(NULL == back_edges) {
        fprintf(stderr, "Failed to create path list (%s:%d)\n",
                __FUNCTION__, __LINE__);
        return NULL;
    }

    ctx = graph_depth_first_search(g, graph_get_vertex(g, 0),
            NULL, _graph_find_back_edges_process_edge, NULL, back_edges);
    graph_search_ctx_free(ctx);

    return back_edges;
}


static void _graph_topo_sort_process_vertex_late(Vertex *v,
        const GraphSearchCtx *ctx, void *userdata)
{
    DList *sorted = (DList *)userdata;

    dlist_prepend(sorted, v);
}

static void _graph_topo_sort_process_edge(Edge *e,
        const GraphSearchCtx *ctx, void *userdata)
{
    if(EDGE_TYPE_BACK == graph_classify_edge(e, ctx)) {
        fprintf(stderr, "Warning: found directed cycle. Graph is not a DAGn");
    }
}

DList* graph_topological_sort(const Graph *g)
{
    GraphSearchCtx *ctx;
    unsigned long i;
    DList *sorted;

    assert(g != NULL);

    if(graph_is_undirected(g)) {
        fprintf(stderr, "Cannot topologically sort an undirected graph\n");
        return NULL;
    }

    sorted = dlist_create();
    if(NULL == sorted) {
        fprintf(stderr, "Failed to create topological sort list (%s:%d)\n",
                __FUNCTION__, __LINE__);
        return NULL;
    }

    ctx = graph_depth_first_search(g, graph_get_vertex(g, 0),
            NULL, _graph_topo_sort_process_edge,
            _graph_topo_sort_process_vertex_late, sorted);

    for(i = 1; i < graph_vertex_count(g); i++) {
        printf("%lu\n", i);
        if(!ctx->discovered[i]) {
            graph_search_ctx_free(ctx);
            ctx = graph_depth_first_search(g, graph_get_vertex(g, i),
                    NULL, _graph_topo_sort_process_edge,
                    _graph_topo_sort_process_vertex_late, sorted);
        }
    }

    graph_search_ctx_free(ctx);

    return sorted;
}
