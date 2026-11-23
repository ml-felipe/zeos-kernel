/*
 * libc.c 
 */

#include <libc.h>

#include <types.h>

int errno;

void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

extern int writeMSR(int fd, char *buffer, int size);

void perror(){

  switch(errno)
  {
    case 9:
      writeMSR(1, "EBADF: Bad file number\n", 23);
      break;
    case 10:
      writeMSR(1, "ECHILD: No child processes\n", 27);
      break;
    case 12:
      writeMSR(1, "ENOMEM: Cannot allocate memory\n", 31);
      break;
    case 13:
      writeMSR(1, "EACCESS: Permission denied\n", 27);
      break;
    case 14:
      writeMSR(1, "EFAULT: Bad address\n", 20);
      break;
    case 22:
      writeMSR(1, "EINVAL: Invalid argument\n", 25);
      break;
    case 38:
      writeMSR(1, "ENOSYS: Function not implemented\n", 33);
      break;
    default:
      char aux_buffer[24];
      writeMSR(1, "ERROR CODE: ", 12);
      itoa(errno, aux_buffer);
      writeMSR(1, aux_buffer, strlen(aux_buffer));
      writeMSR(1, "\n", 1);
      break;
  }
}

