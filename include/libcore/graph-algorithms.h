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

#ifndef __LIBCORE_GRAPH_ALGORITHMS_H__
#define __LIBCORE_GRAPH_ALGORITHMS_H__

#if __cplusplus
extern "C" {
#endif

#include <libcore/darray.h>
#include <libcore/dlist.h>
#include <libcore/graph.h>

typedef struct {
    Graph *g;

    int stop_search;

    int *discovered;
    int *processed;

    unsigned long *entry_time;
    unsigned long *exit_time;

    DArray *parent;
} GraphSearchCtx;


typedef void (*GraphProcessVertexEarlyFn)(Vertex *v,
        const GraphSearchCtx *ctx, void *userdata);

typedef void (*GraphProcessEdgeFn)(Edge *e,
        const GraphSearchCtx *ctx, void *userdata);

typedef void (*GraphProcessVertexLateFn)(Vertex *v,
        const GraphSearchCtx *ctx, void *userdata);


void graph_search_ctx_free(GraphSearchCtx *ctx);

GraphSearchCtx* graph_breadth_first_search(const Graph *g,
        const Vertex                *start,
        GraphProcessVertexEarlyFn   vertex_early_fn,
        GraphProcessEdgeFn          edge_fn,
        GraphProcessVertexLateFn    vertex_late_fn,
        void                        *userdata);

GraphSearchCtx* graph_depth_first_search(const Graph *g,
        const Vertex                *start,
        GraphProcessVertexEarlyFn   vertex_early_fn,
        GraphProcessEdgeFn          edge_fn,
        GraphProcessVertexLateFn    vertex_late_fn,
        void                        *userdata);

DList* graph_find_path(const Graph *g, GraphSearchCtx *ctx,
        Vertex *start, Vertex* end);

unsigned long* graph_connected_components(const Graph *g);

int graph_is_bipartite(const Graph *g);

DList* graph_find_back_edges(const Graph *g);

#if __cplusplus
}
#endif

#endif
