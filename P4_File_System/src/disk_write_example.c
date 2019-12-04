#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "disk.h"

struct dirent{
  char name[30];
  unsigned short size;
};

unsigned short FAT[DISK_BLOCKS];

void write_fat_junk(){
  for (unsigned short i = 0; i < DISK_BLOCKS; ++i){
    FAT[i] = 0xAAAA;
  }
}

void clear_fat(){
  for (unsigned short i = 0; i < DISK_BLOCKS; ++i){
    FAT[i] = 0;
  }
}

void write_fat(){
  char *fat_ptr = (char*) FAT;
  int total = DISK_BLOCKS * sizeof (unsigned short);
  int block_index = 1;
  while (total > 0){
    block_write(block_index, fat_ptr);
    total -= BLOCK_SIZE;
    fat_ptr += BLOCK_SIZE;
    block_index++;
  }  
}

void read_fat(){
  char *fat_ptr = (char*) FAT;
  int total = DISK_BLOCKS * sizeof (unsigned short);
  int block_index = 1;
  while (total > 0){
    block_read(block_index, fat_ptr);
    total -= BLOCK_SIZE;
    fat_ptr += BLOCK_SIZE;
    block_index++;
  }  
}


int main(int argc, char* argv[]){ 
  char name[] = "my_disk"; // the name of our disk 
  // this is the number of dirents we can fit in a block
  int num_dirents = BLOCK_SIZE / sizeof(struct dirent);
  struct dirent *block = NULL; // pointer to an array of dirents
  struct dirent *start_block = NULL;

  // make and open disk
  if (make_disk(name)){
    printf("failed to make disk %s\n", name);
  }
  if (open_disk(name)){
    printf("failed to open disk %s\n", name);
  }
  
  // allocate space for our block 
  // calloc is convenient here for debugging 
  block = calloc(num_dirents, sizeof(num_dirents));
  assert(block != NULL);
  start_block = block;
  
  // write contents to first dirent 
  strcpy(block->name, "myfile1.txt");
  block->size = 0xBEEF;
  // write contents to the second dirent
  block++;
  strcpy(block->name, "myfile2.txt");
  block->size = 0xDEAD;
  
  // writ the block to the file
  block_write(0, (char*) start_block);

  // write contents of fat to disk
  write_fat_junk();
  write_fat();
  clear_fat();
  read_fat();
  printf("RANDOM FAT ELEMENT: %x\n", FAT[DISK_BLOCKS - 1]);
   
  // clear the block memory (to prove we aren't cheating!"
  memset(start_block, '\0', sizeof(num_dirents));
  
  block_read(0, (char*) start_block);
  return 0;  
}
