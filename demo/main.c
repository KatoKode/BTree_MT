/*------------------------------------------------------------------------------
    BTree(MT) Implementation in x86_64 Assembly Language with C Interface
    Copyright (C) 2025  J. McIntosh

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
------------------------------------------------------------------------------*/
#include "main.h"

int main (int argc, char *argv[]) {
  if (argc < 2) {
    printf ("usage: ./btest [random number]\n");
    return -1;
  }
  // myrand will hold the random number paramenter
  size_t myrand = strtol(argv[1], NULL, 10);
  srand48(myrand);    // initialize the random number generator
  // some constants
  size_t const mindeg = MINIMUM_DEGREE;
  size_t const o_size = sizeof(data_t);
  // allocate and initialize our b-tree
  b_tree_t *tree = b_tree_alloc();
  if (b_tree_init(tree, mindeg, o_size, o_cmp_cb, k_cmp_cb,
        o_del_cb, k_get_cb) < 0) return -1;
  // initialize insert meta 1 structure and launch thread 1
  meta_t meta_1 =
  { tree, 1000000, 10000000, calloc(DATA_COUNT * 0.25, sizeof(long)) };
  pthread_t thrd_1;
  (void) pthread_create(&thrd_1, NULL, insert_data, &meta_1);
  // initialize insert meta 2 structure and launch thread 2
  meta_t meta_2 =
  { tree, 10000001, 20000000, calloc(DATA_COUNT * 0.25, sizeof(long)) };
  pthread_t thrd_2;
  (void) pthread_create(&thrd_2, NULL, insert_data, &meta_2);
  int retval;
  void *retptr = &retval;
  // wait on threads to complete
  (void) pthread_join(thrd_2, &retptr);
  if (*((int *)retptr) < 0) return -1;
  (void) pthread_join(thrd_1, &retptr);
  if (*((int *)retptr) < 0) return -1;
  // walk the tree and output data_t structure
  walk_tree(tree);
  // launch thread 3 with insert meta 1 structure
  pthread_t thrd_3;
  (void) pthread_create(&thrd_3, NULL, remove_data, &meta_1);
  // initialize insert meta 3 structure and launch thread 4
  meta_t meta_3 =
  { tree, 40000001, 60000000, calloc(DATA_COUNT * 0.25, sizeof(long)) };
  pthread_t thrd_4;
  (void) pthread_create(&thrd_4, NULL, insert_data, &meta_3);
  // launch thread 5 with insert meta 2 structure
  pthread_t thrd_5;
  (void) pthread_create(&thrd_5, NULL, remove_data, &meta_2);
  // initialize insert meta 3 structure and launch thread 6
  meta_t meta_4 =
  { tree, 20000001, 40000000, calloc(DATA_COUNT * 0.25, sizeof(long)) };
  pthread_t thrd_6;
  (void) pthread_create(&thrd_6, NULL, insert_data, &meta_4);
  // wait on threads to complete
  (void) pthread_join(thrd_6, &retptr);
  if (*((int *)retptr) < 0) return -1;
  (void) pthread_join(thrd_5, &retptr);
  if (*((int *)retptr) < 0) return -1;
  (void) pthread_join(thrd_4, &retptr);
  if (*((int *)retptr) < 0) return -1;
  (void) pthread_join(thrd_3, &retptr);
  if (*((int *)retptr) < 0) return -1;
  // walk the tree and output data_t structure
  walk_tree(tree);
  // release memory held by meta structures
  free(meta_4.lng);
  free(meta_3.lng);
  free(meta_2.lng);
  free(meta_1.lng);
  // release memory held by all the data_t structures (if any), as well as, all
  // the memory held by the tree
  term_tree(tree);

  return 0;
}
//
// callback that compares two data structures
int o_cmp_cb (void const *vp1, void const *vp2) {
  data_t const *d1 = vp1;
  data_t const *d2 = vp2;
  // do comparsions
  if (d1->lng > d2->lng) return 1;
  else if (d1->lng < d2->lng) return -1;
  return 0;
}
//
// callback that compares a long int with the long int in a data structure
// and returns -1 for less-than, 0 for equal-to, 1 for greater-than
//
int k_cmp_cb (void const * vp1, void const * vp2) {
  long const lng = *(long const *)vp1;
  data_t const *d2 = vp2;
  // do comparsions
  if (lng > d2->lng) return 1;
  else if (lng < d2->lng) return -1;
  return 0;
}
//
// callback that returns the key of a data structure
void const * k_get_cb (void const *vp) {
  data_t const *dp = vp;

  printf("%s: lng:\t%ld\n", __func__, dp->lng);
  // return structure key
  return &dp->lng;
}
//
// callback that processes a data structure before deletion from tree
//
void o_del_cb (void const *vp) {
  data_t const *d = vp;
  print_data(__func__, d);
}
//
// output a data structure
//
void print_data (char const *s, data_t const *d) {
  printf("%s:  lng: % 8ld\tstr: %8s\n", s, d->lng, d->str);
}
//
// terminate a tree
//
void term_tree (b_tree_t *tree) {
  puts("\n---| term tree |---\n");
  b_tree_term(tree);
  b_tree_free(tree);
}

//
// callback that outputs a data structure while walking a tree
//
void walk_cb (void const *vp) {
  data_t const *d = vp;

  printf("%8lu:  lng: % 8ld\tstr: %8s\n", ndx++, d->lng, d->str);

  fflush(stdout);

  if ((ndx % 8) == 0) sched_yield();
}
//
// walk a tree
//
void walk_tree (b_tree_t *tree) {
  puts("\n---| walk tree |---\n");

  // initialize index used by tree walking callback
  ndx = 0L;

  b_walk(tree, walk_cb);
}
//
// insert data structures into the tree
//
void * insert_data (void * vp)
{
  // sleep a bit
  struct timespec req = { 0, 250000000 };
  nanosleep(&req, NULL);
  // insert data structures loop
  for (size_t i = 0; i < DATA_COUNT * 0.25; ++i) {
    data_t d;
    size_t x = 0;
    size_t y = 0;
    // attempt insert loop
    do {
      // get a random long int in the range (minlng <= lng <= maxlng)
      do {
        d.lng = lrand48();
        // yield the CPU when dealing with duplicates
        if ((x++ % RND_MOD_BY) == 0) sched_yield();
      } while (d.lng < ((meta_t*)vp)->minlng
          || d.lng > ((meta_t*)vp)->maxlng);
      // assign the random long int to our data array
      ((meta_t *)vp)->lng[i] = d.lng;
      // convert the random long int to a string and store in our data structure
      (void) snprintf(d.str, STR_LEN + 1, "%ld", d.lng);
      // yield the CPU when dealing with duplicates
      if ((y++ % INS_MOD_BY) == 0) sched_yield();
    // attempt to insert a data_t structure into tree
    } while (b_insert(((meta_t*)vp)->tree, &d) < 0);
  }
  // exit thread
  pthread_exit(&RET_SUCCESS);
}
//
// remove data structures from the tree
//
void * remove_data (void * vp)
{
  // sleep a bit
  struct timespec req = { 0, 250000000 };
  nanosleep(&req, NULL);

  size_t x = 0;
  // remove data structures loop
  for (size_t i = 0; i < DATA_COUNT * 0.25; ++i) {
    puts("\n---| begin delete |---\n");
    // yield the CPU when dealing with duplicates
    if ((x++ % REM_MOD_BY) == 0) sched_yield();
    // remove target data structure from tree
    b_remove(((meta_t *)vp)->tree, &((meta_t *)vp)->lng[i]);
  }
  // exit thread
  pthread_exit(&RET_SUCCESS);
}

