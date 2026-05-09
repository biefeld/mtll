#ifndef MTLL_H
#define MTLL_H

#include "node.h"
#include "input_parser.h"
#include "structures.h"

/* =========================================================================
 * mtll.h - Multi-Typed Linked List (MTLL) Interface
 *
 * An MTLL is a singly-linked list whose nodes each hold a value of one of
 * five types (int, float, char, string, or a reference to another MTLL).
 * Lists are identified by a unique size_t index assigned at creation.
 *
 * Memory ownership: all mtll_create/mtll_append allocations are managed
 * internally. Use mtll_free_all to release everything at program exit.
 * For runtime removal of individual lists, use mtll_remove instead of
 * mtll_free, as it handles reference counting for nested lists correctly.
 * ========================================================================= */


/* -------------------------------------------------------------------------
 * Allocation and teardown
 * ------------------------------------------------------------------------- */

/* Allocate and populate a new MTLL with the given values and types.
 * num_nodes: number of nodes to create.
 * next_index: the index to assign to the new list.
 * values: array of pointers to node values (length num_nodes).
 * types: array of pointers to node types (length num_nodes).
 * Returns a pointer to the created MTLL, or NULL on allocation failure. */
extern struct mtll *mtll_create(size_t *num_nodes, size_t *next_index,
                                void **values, enum TYPE **types);

/* Free all memory associated with mtll m, including all nodes and their values.
 * WARNING: does not update reference counts. Use mtll_remove for runtime removal. */
extern void mtll_free(struct mtll *m);

/* Free all MTLLs in the list headed at head, including all nodes and values.
 * WARNING: does not update reference counts. Use mtll_remove for runtime removal. */
extern void mtll_free_all(struct mtll *head);


/* -------------------------------------------------------------------------
 * Commands — equivalent to their CLI counterparts
 * ------------------------------------------------------------------------- */

/* Print the values of the MTLL at list_idx.
 * nested=1 suppresses the trailing newline (used when printing inside braces).
 * Returns 1 on success, 0 if list_idx is invalid. */
extern int mtll_view(char *list_idx, struct mtll **head_ptr, int nested);

/* Print the index and nesting status of all MTLLs. */
extern void mtll_view_all(struct mtll **head_ptr);

/* Print the values of the MTLL at list_idx with nested list contents expanded.
 * Returns 1 on success, 0 if list_idx is invalid. */
extern int mtll_view_nested(char *list_idx, struct mtll **head_ptr);

/* Print the type of each node in the MTLL at list_idx.
 * Returns 1 on success, 0 if list_idx is invalid. */
extern int mtll_type(char *list_idx, struct mtll **head_ptr);

/* Remove the MTLL at list_idx, decrementing reference counts for any lists
 * it references. Fails if the list is currently referenced by another list.
 * Returns 1 on success, 0 on failure. */
extern int mtll_remove(char *list_idx, struct mtll **head_ptr);

/* Insert val at node index idx in the MTLL at list_idx.
 * Negative indices are supported (tail-relative).
 * Returns 1 on success, 0 on failure. */
extern int mtll_insert(char *list_idx, char *idx, char *val,
                       struct mtll **head_ptr);

/* Delete the node at index idx in the MTLL at list_idx.
 * Negative indices are supported (tail-relative).
 * Returns 1 on success, 0 on failure. */
extern int mtll_delete(char *list_idx, char *idx, struct mtll **head_ptr);


/* -------------------------------------------------------------------------
 * Internal utilities — not intended for external use
 * ------------------------------------------------------------------------- */

/* Print the label and contents of the MTLL at list_idx. Called after
 * mutating commands to display the updated state of the list. */
extern void mtll_post_view(char *list_idx, struct mtll **head_ptr);

/* Append new to the end of the MTLL list headed at head_ptr. */
extern void mtll_append(struct mtll **head_ptr, struct mtll *new);

/* Return the MTLL with the given idx, or NULL if not found or idx is invalid. */
extern struct mtll *mtll_valid_idx(char *idx, struct mtll *head);

/* Return 1 if idx is a valid node index string (integer, optionally signed), 0 otherwise. */
extern int mtll_valid_node_idx(char *idx);

/* Set *m_len to the number of nodes in m, or -1 if the list is empty (NaT sentinel). */
extern void mtll_length(struct mtll *m, int *m_len);

#endif
