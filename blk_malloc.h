#ifndef BLK_MALLOC_H
#define BLK_MALLOC_H

/*! \page main_page Block memory allocator
 *  \tableofcontents
 *
 *  \section motivation Motivation
 *  This code was written as a test task for an interview in one of the largest
 *  software companies on the Russian market. 
 *
 *  \section about About
 *  This allocator can allocate a fixed-size block of memory. It uses a static
 *  array to store your data which size you can set up before compilation. Also
 *  you can specify synchronization mechanism to use it in RTOS. The block size
 *  is also customizable. And finally you can set up alignment to use it in
 *  systems which doesn't apply unaligned access for some types.
 *
 *  Also it well-documented in doxygen-style format.
 *
 *  \section setup Setup 
 *  You must specify BLK_MALLOC_BLOCK_ALIGN, BLK_MALLOC_BLOCK_SIZE,
 *  BLK_MALLOC_POOL_SIZE. If you using RTOS you also must specify
 *  BLK_MALLOC_LOCK and BLK_MALLOC_UNLOCK macros any method you know.
 *
 *  \section testing Testing
 *  this code well tested, code coverage at present moment is 100%.
 *  just type "make test" and see what it print
 *
 *  \section building Building
 *  just type "make" and in ./rls folder you will see libblkmalloc.a and header.
 *  */

#define BLK_MALLOC_BLOCK_ALIGN 4  ///< alignment of allocated block in bytes
#define BLK_MALLOC_BLOCK_SIZE  3  ///< size of allocated block in bytes
#define BLK_MALLOC_POOL_SIZE   16 ///< size of pool in blocks

#define BLK_MALLOC_LOCK()         ///< code to lock protection mutex
#define BLK_MALLOC_UNLOCK()       ///< code to unlock protection mutex

#define BLK_MALLOC_OUTPUT_ENABLE /**< this definition allows print messages
                                      which may be generated during work
                                      of allocator. these messages are useful
                                      while you debugging your program. all
                                      messages has format:
                                      [AAA](BBB)[CCC] DDD
                                      field AAA indicates unit which generates
                                                mesage, in our case it is 
                                                "blk_mem"
                                      field BBB indicates finction which
                                                generates message. this field
                                                arrives in warning or error
                                                messages only
                                      field CCC indicates type of message. it
                                                may be warning and field becomes
                                                as [wrn], message and field
                                                becomes as [err] or simple
                                                information message and this
                                                field will be redused
                                      field DDD indicates information message
                                                which can help you in debugging
                                                your application */

//By condition this memory allocator and standard malloc might have diffirent
//behavour because standard malloc allocates n-bytes of memory but block memory
//allocator shall allocate just one fixed size block of memory. So interfaces of
//allocators may be diffirent.

/** \brief  allocate one block of dynamic memory
 *  \return pointer to allocated block or error indicator 
 *  \retval NULL  error
 *  \retval !NULL valid block pointer */
void* blk_alloc(void);

/** \brief  free selected block from dynamic memory
 *  \arg    ptr pointer to block
 *  \return result of operation
 *  \retval 0   ok
 *  \retval -1  error */
int blk_free(void* ptr);

#endif//ifndef BLK_MALLOC_H
