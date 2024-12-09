// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct{
  struct spinlock lock;
  struct run *freelist;
} kmem;

struct ref{
  struct spinlock lock;
  int cnt;
};

struct ref refs[(PHYSTOP)/PGSIZE];

void
kinit()
{
  for(int i=0;i<PHYSTOP/PGSIZE;i++){
    initlock(&(refs[i].lock),"kref");
  }
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  int idx=(uint64)pa/PGSIZE;
  acquire(&(refs[idx].lock));
  refs[idx].cnt--;
  if(refs[idx].cnt>0){
    release(&(refs[idx].lock));
    return;
  }
  release(&(refs[idx].lock));
  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r){
    memset((char*)r, 5, PGSIZE); // fill with junk
    int idx=(uint64)r/PGSIZE;
    acquire(&(refs[idx].lock));
    refs[idx].cnt=1;
    release(&(refs[idx].lock));
  }
    
  return (void*)r;
}

int getref(uint64 pa){
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP){
    return -1;
  }
  return refs[(uint64)pa/PGSIZE].cnt;
}

int addref(uint64 pa){
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP){
    return -1;
  }
  int idx=(uint64)pa/PGSIZE;
  acquire(&(refs[idx].lock));
  refs[idx].cnt++;
  release(&(refs[idx].lock));
  return 1;
}