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
  int case1  = 0;
  int i;

  struct proc * curproc = myproc();

  acquire(&(shm_table.lock));
  for (i = 0; i < 64; i++) {
    if (shm_table.shm_pages[i].id == id) {
        case1 = 1;
        break;
    }
  }
  release(&(shm_table.lock));

  if (case1 == 1) {
    cprintf("Case 1\n");
    
    acquire(&(shm_table.lock));
    mappages(curproc->pgdir, (void*)PGROUNDUP(curproc->sz), PGSIZE, 
            V2P(shm_table.shm_pages[i].frame), PTE_W|PTE_U);
    shm_table.shm_pages[i].refcnt++;
    release(&(shm_table.lock));

    cprintf("Page mapped\n");
    *pointer = (char*)PGROUNDUP(curproc->sz);
    curproc->sz += PGSIZE;

    cprintf("End Case 1\n");
  }
  else {
    acquire(&(shm_table.lock));
    for (i = 0; i < 64; i++) {
        if (shm_table.shm_pages[i].id == 0) {
        cprintf("Case 2\n");
        shm_table.shm_pages[i].id = id;
        // TODO: Map a page and store its address in frame (use kalloc, then mappages)
        shm_table.shm_pages[i].frame = kalloc();
        shm_table.shm_pages[i].refcnt = 1;
        mappages(curproc->pgdir, (void*)PGROUNDUP(curproc->sz), PGSIZE,
                V2P(shm_table.shm_pages[i].frame), PTE_W|PTE_U);
        cprintf("Page mapped\n");
        *pointer = (char*)PGROUNDUP(curproc->sz);
        curproc->sz += PGSIZE;
        cprintf("End Case 2\n");
        break;
        }
    }
    release(&(shm_table.lock));
  }
  cprintf("End of shm_open\n");

return 0; //added to remove compiler warning -- you should decide what to return
}


int shm_close(int id) {
  //you write this too!
  int i = 0;
  // we need to lock the memory before we can start
  // initlock(&(shm_table.lock), "SHM lock");
  acquire(&(shm_table.lock));

  // look for shared memory segment
  for (i = 0; i < 64; i++) {
    // if the segment matches
    if (shm_table.shm_pages[i].id == id) {
        if (shm_table.shm_pages[i].refcnt >= 1)
          shm_table.shm_pages[i].refcnt--;
    }
    else {
        shm_table.shm_pages[i].id = 0;
        shm_table.shm_pages[i].frame = 0;
        shm_table.shm_pages[i].refcnt = 0;
    }
  }
  release(&(shm_table.lock));


return 0; //added to remove compiler warning -- you should decide what to return
}
