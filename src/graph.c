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

#include <libcore/graph.h>
#include <libcore/darray.h>

/* Type definitions */

struct _graph {
    GRAPH_TYPE type;
    DArray *vertices;
    unsigned long edge_count;
};

struct _vertex {
    unsigned long idx;
    unsigned long in_degree;
    unsigned long out_degree;
    void *data;
    DArray *edges;
};

struct _edge {
    Vertex *source;
    Vertex *target;
    float weight;
};


/* Vertex operations */

Vertex* vertex_create(void *data)
{
    Vertex *new_vertex;

    new_vertex = malloc(sizeof(struct _vertex));
    if(NULL == new_vertex) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    new_vertex->idx = -1;
    new_vertex->in_degree = 0;
    new_vertex->out_degree = 0;
    new_vertex->data = data;

    new_vertex->edges = darray_create();
    if(NULL == new_vertex->edges) {
        fprintf(stderr, "Can't create edge list (%s:%d)\n",
                __FUNCTION__, __LINE__);
        free(new_vertex);
        return NULL;
    }

    return new_vertex;
}

void vertex_free(Vertex *v)
{
    assert(v != NULL);

    darray_free_all(v->edges, NULL);
    free(v);
}

void vertex_free_all(Vertex *v, FreeFn data_freefn)
{
    assert(v != NULL);

    if(NULL == data_freefn) {
        /* Default to stdlib free */
        data_freefn = free;
    }

    data_freefn(v->data);
    darray_free_all(v->edges, NULL);
    free(v);
}

unsigned long vertex_get_index(const Vertex *v)
{
    assert(v != NULL);

    return v->idx;
}

void* vertex_get_data(const Vertex *v)
{
    assert(v != NULL);

    return v->data;
}

int vertex_set_data(Vertex *v, void *data)
{
    assert(v != NULL);

    v->data = data;

    return 0;
}

DArray* vertex_get_edges(const Vertex *v)
{
    assert(v != NULL);

    return v->edges;
}

unsigned long vertex_edge_count(const Vertex *v)
{
    assert(v != NULL);

    return darray_size(v->edges);
}


/* Edge operations */

Edge* edge_create(Vertex *v, Vertex *w, float weight)
{
    Edge *new_edge;

    assert(v != NULL);
    assert(w != NULL);

    new_edge = malloc(sizeof(struct _edge));
    if(NULL == new_edge) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    new_edge->source = v;
    new_edge->target = w;
    new_edge->weight = weight;

    return new_edge;
}

void edge_free(Edge *e)
{
    assert(e != NULL);

    free(e);
}

float edge_get_weight(const Edge *e)
{
    assert(e != NULL);

    return e->weight;
}

int edge_set_weight(Edge *e, float weight)
{
    assert(e != NULL);

    e->weight = weight;

    return 0;
}

Vertex* edge_get_source(const Edge *e)
{
    assert(e != NULL);

    return e->source;
}

Vertex* edge_get_target(const Edge *e)
{
    assert(e != NULL);

    return e->target;
}


/* Graph operations */

Graph* graph_create(GRAPH_TYPE type)
{
    Graph *new_graph;

    /* Graph container */
    new_graph = malloc(sizeof(struct _graph));
    if(NULL == new_graph) {
        fprintf(stderr, "Out of memory (%s:%d)\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    if((type < 0) && (type >= GRAPH_TYPE_INVALID)) {
        fprintf(stderr, "Invalid graph type: %d\n", type);
        free(new_graph);
        return NULL;
    }

    new_graph->type         = type;
    new_graph->vertices     = darray_create();
    new_graph->edge_count   = 0;

    return new_graph;
}

void graph_free(Graph *g)
{
    assert(g != NULL);

    /* Only free graph container, NOT edges or vertices  */
    free(g);
}

void graph_free_all(Graph *g, FreeFn vertex_data_freefn)
{
    unsigned long index;
    Vertex *v;

    assert(g != NULL);

    if(NULL == vertex_data_freefn) {
        /* Default to stdlib free */
        vertex_data_freefn = free;
    }

    /* Free edge lists and vertex data */
    for(index = 0; index < darray_size(g->vertices); index++) {
        v = (Vertex *)darray_index(g->vertices, index);
        vertex_data_freefn(v->data);
        darray_free_all(v->edges, NULL);
    }

    /* Free vertex list */
    darray_free_all(g->vertices, NULL);

    /* Free graph container */
    free(g);
}

int graph_vertex_add(Graph *g, Vertex *v)
{
    assert(g != NULL);
    assert(v != NULL);

    v->idx = darray_size(g->vertices);

    return darray_append(g->vertices, v);
}

int graph_edge_add(Graph *g, Edge *e)
{
    unsigned long index;
    Vertex *v;

    assert(g != NULL);
    assert(e != NULL);

    for(index = 0; index < darray_size(g->vertices); index++) {
        v = (Vertex *)darray_index(g->vertices, index);

        if(v == e->source) {
            v->out_degree++;
            darray_append(v->edges, e);
            g->edge_count++;
        }

        if((v == e->target)) {
            /* For undirected graphs, insert an extra edge to allow
             * traversal from the target vertex back to the source
             * vertex, except in the case where an edge is a loop.
             */
            if(graph_is_undirected(g)) {
                v->out_degree++;

                if(e->target != e->source) {
                    darray_append(v->edges,
                        edge_create(e->target, e->source, e->weight));
                } else {
                    /* Loops are counted twice */
                    v->out_degree++;
                }
            } else {
                /* Keep track of the number of edges directed
                 * toward this vertex, but not which edges
                 *
                 * TODO Keep a separate in-edge list for each
                 * node?
                 */
                v->in_degree++;
            }
        }
    }

    return 0;
}

DArray* graph_get_vertices(const Graph *g)
{
    assert(g != NULL);

    return g->vertices;
}

DArray* graph_get_edges(const Graph *g)
{
    unsigned long index;
    DArray *edges;
    Vertex *v;

    assert(g != NULL);

    edges = darray_create();

    if(NULL == edges) {
        fprintf(stderr, "Cannot create edge list (%s:%d)\n",
                __FUNCTION__, __LINE__);
        return NULL;
    }

    for(index = 0; index < darray_size(g->vertices); index++) {
        v = (Vertex *)darray_index(g->vertices, index);

        if(darray_concat(edges, v->edges) < 0) {
            fprintf(stderr, "Cannot construct edge list (%s:%d)\n",
                    __FUNCTION__, __LINE__);
            darray_free(edges);
            return NULL;
        }
    }

    return edges;
}

Vertex* graph_get_vertex(const Graph *g, unsigned long index)
{
    assert(g != NULL);

    if(index >= graph_vertex_count(g)) {
        return NULL;
    }

    return (Vertex *)darray_index(g->vertices, index);
}

unsigned long graph_vertex_count(const Graph *g)
{
    assert(g != NULL);

    return darray_size(g->vertices);
}

unsigned long graph_edge_count(const Graph *g)
{
    assert(g != NULL);

    return g->edge_count;
}

GRAPH_TYPE graph_get_type(const Graph *g)
{
    assert(g != NULL);

    return g->type;
}

int graph_is_directed(const Graph *g)
{
    assert(g != NULL);

    return (g->type == GRAPH_TYPE_DIRECTED) ? 1 : 0;
}

int graph_is_undirected(const Graph *g)
{
    assert(g != NULL);

    return (g->type == GRAPH_TYPE_UNDIRECTED) ? 1 : 0;
}

int graph_is_empty(const Graph *g)
{
    assert(g != NULL);

    return darray_is_empty(g->vertices);
}


/* Graph vertex operations */

Vertex* graph_vertex_find(const Graph *g, const void *data)
{
    unsigned long index;
    Vertex *v;

    assert(g != NULL);
    assert(data != NULL);

    for(index = 0; index < darray_size(g->vertices); index++) {
        v = (Vertex *)darray_index(g->vertices, index);
        if(v->data == data) {
            return v;
        }
    }

    return NULL;
}

unsigned long graph_vertex_in_degree(const Graph *g, const Vertex *v)
{
    assert(g != NULL);
    assert(v != NULL);

    if(graph_is_undirected(g)) {
        return v->out_degree;
    }

    return v->in_degree;
}

unsigned long graph_vertex_out_degree(const Graph *g, const Vertex *v)
{
    assert(g != NULL);
    assert(v != NULL);

    return v->out_degree;
}

unsigned long graph_vertex_degree(const Graph *g, const Vertex *v)
{
    assert(g != NULL);
    assert(v != NULL);

    return v->in_degree + v->out_degree;
}

DArray* graph_vertex_get_adj_edges(const Graph *g, const Vertex *v)
{
    assert(g != NULL);
    assert(v != NULL);

    return v->edges;
}

DArray* graph_vertex_get_in_edges(const Graph *g, const Vertex *v)
{
    unsigned long index;
    DArray *in_edges;
    Edge *e;

    assert(g != NULL);
    assert(v != NULL);

    in_edges = darray_create();

    if(NULL == in_edges) {
        fprintf(stderr, "Cannot create in-edge list (%s:%d)\n",
                __FUNCTION__, __LINE__);
        return NULL;
    }

    for(index = 0; index < darray_size(v->edges); index++) {
        e = (Edge *)darray_index(v->edges, index);
        if(v == e->target) {
            if(darray_append(in_edges, e) < 0) {
                fprintf(stderr, "Cannot construct in-edge list (%s:%d)\n",
                        __FUNCTION__, __LINE__);
                darray_free(in_edges);
                return NULL;
            }
        }
    }

    return in_edges;
}

DArray* graph_vertex_get_out_edges(const Graph *g, const Vertex *v)
{
    unsigned long index;
    DArray *out_edges;
    Edge *e;

    assert(g != NULL);
    assert(v != NULL);

    out_edges = darray_create();

    if(NULL == out_edges) {
        fprintf(stderr, "Cannot create out-edge list (%s:%d)\n",
                __FUNCTION__, __LINE__);
        return NULL;
    }

    for(index = 0; index < darray_size(v->edges); index++) {
        e = (Edge *)darray_index(v->edges, index);
        if(v == e->source) {
            if(darray_append(out_edges, e) < 0) {
                fprintf(stderr, "Cannot construct out-edge list (%s:%d)\n",
                        __FUNCTION__, __LINE__);
                darray_free(out_edges);
                return NULL;
            }
        }
    }

    return out_edges;
}
