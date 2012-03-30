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
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SURBTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __LIBCORE_GRAPH_H__
#define __LIBCORE_GRAPH_H__

#if __cplusplus
extern "C" {
#endif

#include <libcore/darray.h>
#include <libcore/types.h>

/* Opaque forward declarations */
typedef struct _graph Graph;
typedef struct _vertex Vertex;
typedef struct _edge Edge;

/* Supported graph types */
typedef enum {
    GRAPH_TYPE_DIRECTED,
    GRAPH_TYPE_UNDIRECTED,

    GRAPH_TYPE_INVALID
} GRAPH_TYPE;

/* Vertex operations */
Vertex*         vertex_create       (void *data);
void            vertex_free         (Vertex *v);
void            vertex_free_all     (Vertex *v, FreeFn data_freefn);
unsigned long   vertex_get_index    (const Vertex *v);
void*           vertex_get_data     (const Vertex *v);
int             vertex_set_data     (Vertex *v, void *data);
DArray*         vertex_get_edges    (const Vertex *v);
unsigned long   vertex_edge_count   (const Vertex *v);

/* Edge operations */
Edge*           edge_create         (Vertex *v, Vertex *w, float weight);
void            edge_free           (Edge *e);
float           edge_get_weight     (const Edge *e);
int             edge_set_weight     (Edge *e, float weight);
Vertex*         edge_get_source     (const Edge *e);
Vertex*         edge_get_target     (const Edge *e);

/* Graph operations */
Graph*          graph_create        (GRAPH_TYPE type);
void            graph_free          (Graph *graph);
void            graph_free_all      (Graph *graph, FreeFn vertex_data_freefn);

int             graph_vertex_add    (Graph *g, Vertex *v);
int             graph_edge_add      (Graph *g, Edge *e);

DArray*         graph_get_vertices  (const Graph *g);
DArray*         graph_get_edges     (const Graph *g);

Vertex*         graph_get_vertex    (const Graph *g, unsigned long index);

unsigned long   graph_vertex_count  (const Graph *g);
unsigned long   graph_edge_count    (const Graph *g);

GRAPH_TYPE      graph_get_type      (const Graph *g);
int             graph_is_directed   (const Graph *g);
int             graph_is_undirected (const Graph *g);

int             graph_is_empty      (const Graph *g);

/* Graph vertex operations */
Vertex*         graph_vertex_find          (const Graph *g, const void *data);
unsigned long   graph_vertex_in_degree     (const Graph *g, const Vertex *v);
unsigned long   graph_vertex_out_degree    (const Graph *g, const Vertex *v);
unsigned long   graph_vertex_degree        (const Graph *g, const Vertex *v);
DArray*         graph_vertex_get_adj_edges (const Graph *g, const Vertex *v);
DArray*         graph_vertex_get_in_edges  (const Graph *g, const Vertex *v);
DArray*         graph_vertex_get_out_edges (const Graph *g, const Vertex *v);

#if __cplusplus
}
#endif

#endif
