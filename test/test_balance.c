/*
  This file is part of p4est.
  p4est is a C library to manage a parallel collection of quadtrees and/or
  octrees.

  Copyright (C) 2007 Carsten Burstedde, Lucas Wilcox.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <p4est_algorithms.h>
#include <p4est_base.h>

int
main (int argc, char **argv)
{
  int                 k;
  int8_t              l;
  p4est_t            *p4est;
  p4est_tree_t        stree, *tree = &stree;
  p4est_quadrant_t   *q;
  p4est_connectivity_t *connectivity;

  connectivity = p4est_connectivity_new_unitsquare ();
  p4est = p4est_new (MPI_COMM_NULL, stdout, connectivity, 4, NULL);

  /* build empty tree */
  tree->quadrants = p4est_array_new (sizeof (p4est_quadrant_t));
  for (l = 0; l <= P4EST_MAXLEVEL; ++l) {
    tree->quadrants_per_level[l] = 0;
  }
  tree->maxlevel = 0;

  /* insert two quadrants */
  p4est_array_resize (tree->quadrants, 4);
  q = p4est_array_index (tree->quadrants, 0);
  p4est_quadrant_set_morton (q, 1, 0);
  q = p4est_array_index (tree->quadrants, 1);
  p4est_quadrant_set_morton (q, 3, 13);
  q = p4est_array_index (tree->quadrants, 2);
  p4est_quadrant_set_morton (q, 1, 1);
  q = p4est_array_index (tree->quadrants, 3);
  p4est_quadrant_set_morton (q, 1, 2);
  for (k = 0; k < tree->quadrants->elem_count; ++k) {
    q = p4est_array_index (tree->quadrants, k);
    q->user_data = p4est_mempool_alloc (p4est->user_data_pool);
    ++tree->quadrants_per_level[q->level];
    tree->maxlevel = (int8_t) P4EST_MAX (tree->maxlevel, q->level);
  }

  /* balance the tree, print and destroy */
  p4est_balance_subtree (p4est, tree, 0, NULL);
  p4est_tree_print (tree, p4est->mpirank, p4est->nout);
  for (k = 0; k < tree->quadrants->elem_count; ++k) {
    q = p4est_array_index (tree->quadrants, k);
    p4est_mempool_free (p4est->user_data_pool, q->user_data);
  }
  p4est_array_destroy (tree->quadrants);

  /* balance the forest */
  p4est_balance (p4est, NULL);
  tree = p4est_array_index (p4est->trees, 0);
  p4est_tree_print (tree, p4est->mpirank, p4est->nout);

  /* clean up memory */
  P4EST_ASSERT (p4est->user_data_pool->elem_count ==
                p4est->local_num_quadrants);
  p4est_destroy (p4est);
  p4est_connectivity_destroy (connectivity);

  p4est_memory_check ();

  return 0;
}

/* EOF test_balance.c */