#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct shm_page {
    uint id;
    char *frame;
    int refcnt;
  } shm_pages[64];
} shm_table;

void shminit() {
  int i;
  initlock(&(shm_table.lock), "SHM lock");
  acquire(&(shm_table.lock));
  for (i = 0; i< 64; i++) {
    shm_table.shm_pages[i].id =0;
    shm_table.shm_pages[i].frame =0;
    shm_table.shm_pages[i].refcnt =0;
  }
  release(&(shm_table.lock));
}

int shm_open(int id, char **pointer) {

//you write this
  bool exists = false;
  int i;

  acquire(&(shm_table.lock));
  for (i = 0; i < 64; i++) {
    if (shm_table.shm_pages[i].id == id) {
        bool exists = true;
        break;
    }
  }

  if (exists) {
    struct proc * curpoc = myproc();
    mappages(curproc->pgdir, PGROUNDUP(curproc->sz), PGSIZE, 
            V2P(shm_table.shm_pages[i].frame), PTE_W|PTE_U);
    shm_table.sm_pages[i].refcnt++;
    *pointer = (char*)PGROUNDUP(curproc->sz);
    curproc->sz += PGSIZE;
    return 1;
  }
  else {
    for (i = 0; i< 64; i++) {
        if (shm_table.shm_pages[i].id = 0 && 
                shm_table.shm_pages[i].frame = 0 && 
                shm_table.shm_pages[i].refcnt = 0) {
        shm_table.shm_pages[i].id = id;
        // TODO: Map a page and store its address in frame (use kalloc, then mappages)
        shm_table.shm_pages[i].refcnt = 1;
        }
    }
  }

  release(&(shm_table.lock));

return 0; //added to remove compiler warning -- you should decide what to return
}


int shm_close(int id) {
  //you write this too!
  int i = 0;
  // we need to lock the memory before we can start
  initlock(&(shm_table.lock), "SHM lock");
  acquire(&(shm_table.lock));

  // look for shared memory segment
  for (i = 0; i < 64; i++) {
    // if the segment matches
    if (shm_table.shm_pages[i].id == id) {
      shm_table.shm_pages[i].refcnt--;
    }
  }

return 0; //added to remove compiler warning -- you should decide what to return
}
