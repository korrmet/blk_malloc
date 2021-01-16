#include "blk_malloc.h"
#ifdef BLK_MALLOC_OUTPUT_ENABLE
  #include <stdio.h>
#endif
#include <stddef.h>

#if (BLK_MALLOC_BLOCK_ALIGN%BLK_MALLOC_BLOCK_SIZE == 0)
#define BLK_MALLOC_ACTUAL_BLOCK_SIZE BLK_MALLOC_BLOCK_SIZE
#else //#if (BLK_MALLOC_BLOCK_ALIGN%BLK_MALLOC_BLOCK_SIZE == 0)
#define BLK_MALLOC_ACTUAL_BLOCK_SIZE \
  (BLK_MALLOC_BLOCK_SIZE + (BLK_MALLOC_BLOCK_ALIGN%BLK_MALLOC_BLOCK_SIZE))
#warning size of memory pool will be explicitly large
#endif //#if (BLK_MALLOC_BLOCK_ALIGN%BLK_MALLOC_BLOCK_SIZE == 0)

#ifdef BLK_MALLOC_OUTPUT_ENABLE
#define pwrn(...) printf("[blk_mem](%s)[wrn] ", __func__); printf(__VA_ARGS__)
#define perr(...) printf("[blk_mem](%s)[err] ", __func__); printf(__VA_ARGS__)
#define pmss(...) printf("[blk_mem] ");                    printf(__VA_ARGS__)
#else //#ifdef BLK_MALLOC_OUTPUT_ENABLE
#define pwrn()
#define perr()
#define pmss()
#endif //#ifdef BLK_MALLOC_OUTPUT_ENABLE

unsigned char blk_pool[BLK_MALLOC_POOL_SIZE][BLK_MALLOC_ACTUAL_BLOCK_SIZE] 
__attribute__((aligned(BLK_MALLOC_BLOCK_ALIGN))) = { 0 };
unsigned char blk_busy_flags[BLK_MALLOC_POOL_SIZE / 8] = { 0 };

/** \brief   search free space in pool
 *  \details scans blk_busy_flags array for zeros and returns position
 *  \return  position of free block in pool or error indicator
 *  \retval  >=0 valid position
 *  \retval  -1 error indicator */
int blk_find_free(void) 
{ int res = -1;
  for (int i = 0; i < sizeof(blk_busy_flags); i++)
  { if (res != -1) { break; }
    if (blk_busy_flags[i] != 0xff) 
    { unsigned char tmp = blk_busy_flags[i];
      for (int j = 0; j < 8; j++)
      { if ((tmp & (unsigned char)(1 << j)) == 0x00) 
        { res = (i * 8) + j; break; } } } }
  if (res < 0) { perr("can't find free space\n"); }
  return res; }

/** \brief   mark memory as busy
 *  \details set in blk_busy_flags array 1 in requested position
 *  \arg     pos requested position
 *  \return  error indicator
 *  \retval  0  all ok
 *  \retval  -1 error */
int blk_mark_busy(int pos) 
{ if (pos >= BLK_MALLOC_POOL_SIZE || pos < 0) 
  { perr("position %d outs of bounds of allocation table\n", pos); return -1; }

  blk_busy_flags[pos/8] |= 1 << (pos % 8);

  return 0; }

/** \brief   mark memory as free
 *  \details set in blk_busy_flags array zero in requested position
 *  \arg     pos requested position
 *  \return  error indicator
 *  \retval  0  all ok
 *  \retval  -1 error */
int blk_mark_free(int pos) 
{ if (pos >= BLK_MALLOC_POOL_SIZE || pos < 0) 
  { perr("position %d outs of bounds of allocation table\n", pos); return -1; }

  blk_busy_flags[pos/8] &= ~(1 << (pos % 8));

  return 0; }

/** \brief  converts pointer to position
 *  \arg    ptr pointer to allocated block
 *  \return position or error indicator
 *  \retval >=0 valid position
 *  \retval -1  error */
int blk_ptr_to_pos(void* ptr) 
{ if ((ptr < (void*)blk_pool) || (ptr > (void*)(blk_pool + sizeof(blk_pool))))
  { perr("pointer %p outs of bounds of allocation memory\n", ptr); 
    return -1; }
  
  int shift = (char*)ptr - (char*)blk_pool;
  if ((shift%BLK_MALLOC_ACTUAL_BLOCK_SIZE) != 0) 
  { perr("pointer %p does not fits allocation table fields\n", ptr);
    return -1; }

  return shift/BLK_MALLOC_POOL_SIZE; }

void* _blk_alloc(void) 
{ int first_free = blk_find_free(); if (first_free < 0) { return NULL; }

  blk_mark_busy(first_free); return &blk_pool[first_free][0]; }

int _blk_free(void* ptr) 
{ int pos = blk_ptr_to_pos(ptr);
  if (blk_mark_free(pos) == -1) { return -1; }
  return 0; }

void* blk_alloc(void) 
{ BLK_MALLOC_LOCK(); 
  void* res = _blk_alloc(); 
  BLK_MALLOC_UNLOCK(); 
  return res; }

int blk_free(void* ptr) 
{ BLK_MALLOC_LOCK();
  int res = _blk_free(ptr);
  BLK_MALLOC_UNLOCK();
  return res; }
