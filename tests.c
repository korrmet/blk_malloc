#include "blk_malloc.h"
#include <stdio.h>
#include <string.h>

#if (BLK_MALLOC_BLOCK_ALIGN%BLK_MALLOC_BLOCK_SIZE == 0)
#define BLK_MALLOC_ACTUAL_BLOCK_SIZE BLK_MALLOC_BLOCK_SIZE
#else //#if (BLK_MALLOC_BLOCK_ALIGN%BLK_MALLOC_BLOCK_SIZE == 0)
#define BLK_MALLOC_ACTUAL_BLOCK_SIZE \
  (BLK_MALLOC_BLOCK_SIZE + (BLK_MALLOC_BLOCK_ALIGN%BLK_MALLOC_BLOCK_SIZE))
#warning size of memory pool will be explicitly large
#endif //#if (BLK_MALLOC_BLOCK_ALIGN%BLK_MALLOC_BLOCK_SIZE == 0)

extern unsigned char 
blk_pool[BLK_MALLOC_POOL_SIZE][BLK_MALLOC_ACTUAL_BLOCK_SIZE]
__attribute__((aligned(BLK_MALLOC_BLOCK_ALIGN)));
extern unsigned char blk_busy_flags[BLK_MALLOC_POOL_SIZE / 8];
extern int blk_find_free(void);
extern int blk_mark_busy(int pos); 

#define TST(...) printf("--> "); printf(__VA_ARGS__); tests_num++

int main(void) 
{ int tests_num = 0; int passed_num = 0;

  printf("block size:        %d\n", BLK_MALLOC_BLOCK_SIZE);
  printf("actual block size: %d\n", BLK_MALLOC_ACTUAL_BLOCK_SIZE);

  TST("blk_find_free in case where all memory is free\n");
  { memset(blk_busy_flags, 0, sizeof(blk_busy_flags));
    if (blk_find_free() == 0) { passed_num++; } else { printf("fail\n"); } }

  TST("blk_find_free in case where all memory is busy\n");
  { memset(blk_busy_flags, 0xFF, sizeof(blk_busy_flags));
    if (blk_find_free() == -1) { passed_num++; } else { printf("fail\n"); } }

  TST("blk_find_free in case where half memory is busy\n");
  { memset(blk_busy_flags, 0xAA, sizeof(blk_busy_flags));
    int res = blk_find_free();
    if (res == 0) { passed_num++; }
    else { printf("fail, as is: %d, must be %d, blk_busy_flags: ", res, 0);  
           for (int i = 0; i < sizeof(blk_busy_flags); i++) 
           { printf("%02x ", blk_busy_flags[i]); } 
           printf("\n"); } }
  
  TST("blk_mark_busy(-1)\n");
  { if (blk_mark_busy(-1) == -1) { passed_num++; } else { printf("fail\n"); } }

  TST("blk_mark_busy(16)\n");
  { if (blk_mark_busy(16) == -1) { passed_num++; } else { printf("fail\n"); } }

  TST("blk_mark_busy(0)\n");
  { int fail = 0;
    memset(blk_busy_flags, 0, sizeof(blk_busy_flags));
    if (blk_mark_busy(0) == -1) 
    { printf("fail, false-negative result\n"); fail = 1; }
    if (blk_busy_flags[0] != 0x01 || blk_busy_flags[1] != 0x00)
    { printf("fail, unexpected result: \n"); 
      printf("  as is:    ");
      for (int i = 0; i < sizeof(blk_busy_flags); i++)
      { printf("%02x ", blk_busy_flags[i]); }
      printf("\n  expected: 01 00\n");
      fail = 1; }
    
    if (!fail) { passed_num++; } }
  
  TST("blk_mark_busy(15)\n");
  { int fail = 0;
    memset(blk_busy_flags, 0, sizeof(blk_busy_flags));
    if (blk_mark_busy(15) == -1) 
    { printf("fail, false-negative result\n"); fail = 1; }
    if (blk_busy_flags[0] != 0x00 || blk_busy_flags[1] != 0x80)
    { printf("fail, unexpected result: \n"); 
      printf("  as is:    ");
      for (int i = 0; i < sizeof(blk_busy_flags); i++)
      { printf("%02x ", blk_busy_flags[i]); }
      printf("\n  expected: 00 80\n");
      fail = 1; }
    
    if (!fail) { passed_num++; } }

  TST("allocate 17 times\n");
  { memset(blk_busy_flags, 0, sizeof(blk_busy_flags));
    int success = 0; int fail = 0;
    for (int i = 0; i < 17; i++)
    { void* res = blk_alloc();
      if (((unsigned char*)res != blk_pool[i]) && (i < 16))
      { printf("fail\n");
        printf("  allocation result is: %p\n", res);
        printf("               must be: %p\n", blk_pool[i]);
        fail = 1; }
      if ((res == NULL) && (i == 16)) { success = 1; } }

    if (success && !fail) { passed_num++; } else { printf("fail\n"); } }

  TST("free all memory allocated before\n");
  { int fail = 0;
    for (int i = 0; i < 16; i++)
    { if (blk_free(blk_pool[i]) < 0) { fail = 1; } }

    if (!fail) { passed_num++; } else { printf("fail\n"); } }

  TST("free NULL pointer\n");
  { if (blk_free(NULL) == -1) { passed_num++; } else { printf("fail\n"); } }

  TST("free unaligned pointer\n");
  { if (blk_free((char*)blk_pool + 2) == -1) { passed_num++; } 
    else { printf("fail\n"); } }

  printf("stat: %d/%d passed\n", passed_num, tests_num);
  if (passed_num != tests_num) { return -1; }
  return 0; }
