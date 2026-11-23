#include <libc.h>

/* NOT NEEDED?, JUST HERE TO STOP MAKE WARNINGS */
extern int addASM(int, int);
extern void exit();
extern int fork();
extern int write(int fd, char *buffer, int size);
extern int writeMSR(int fd, char *buffer, int size);
extern void block();
extern int unblock(int pid);
extern int gettime();
extern int getpid();

char buff[24];
int pid;

int add(int par1, int par2) {
  
  return par1 + par2;

}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
  /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
  /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
  
  /* BASIC CHECKS */   
  /*int dummy = add(0x42, 0x666);       // FIRST SESSION CHECKS
  int dummy2 = addASM(0x666, 0x42);*/
  
  /*if(write(1, "Write test\n", 11) < 0) perror();  // WRITE SYSCALLS
  if(write(0, "ERROR 1\n", 8) < 0) perror();
  if(write(1, (char *) 0, 1) < 0) perror();
  if(write(1, "ERROR 3\n", -1) < 0) perror();*/

  /*if(writeMSR(1, "WriteMSR test\n", 14) < 0) perror();
  if(writeMSR(0, "ERROR 1\n", 8) < 0) perror();
  if(writeMSR(1, (char *) 0, 1) < 0) perror();
  if(writeMSR(1, "ERROR 3\n", -1) < 0) perror();*/

  /*if(write(1, "Get time:\n", 10) < 0) perror();
  itoa(gettime(), buff); // GET TIME
  if(write(1, buff, 10) < 0) perror();
  if(write(1, "\n", 1) < 0) perror();*/

  /*char* p = 0; // PAGEFAULT
  *p = 'x';*/

  /*itoa(getpid(), buff); // GET PID
  if(write(1, "Get pid:\n", 9) < 0) perror();
  if(write(1, buff, 10) < 0) perror();
  if(write(1, "\n", 1) < 0) perror();*/

  if(writeMSR(1, "FORK\n", 5) < 0) perror(); // FORK
  pid = fork();
  if(pid < 0) { 
    if(writeMSR(1, "FORK ERROR\n", 11) < 0) perror();
    else perror();
  }
  else if(pid > 0) {
    if(writeMSR(1, "I'm parent process my pid is: ", 30) < 0) perror();   
    itoa(getpid(), buff);
    if(writeMSR(1, buff, strlen(buff)) < 0) perror();
    if(writeMSR(1, "\n", 1) < 0) perror();
    if(writeMSR(1, "My child's pid is: ", 19) < 0) perror();
    itoa(pid, buff);
    if(writeMSR(1, buff, strlen(buff)) < 0) perror();
    if(writeMSR(1, "\n", 1) < 0) perror();
  }
  else {
    if(writeMSR(1, "I'm child process my pid is: ", 29) < 0) perror();
    itoa(getpid(), buff);
    if(writeMSR(1, buff, strlen(buff)) < 0) perror();
    if(writeMSR(1, "\n", 1) < 0) perror();
  }

  /*if(writeMSR(1, "BLOCK\n", 6) < 0) perror(); // BLOCK
  pid = fork();
  if(pid < 0) { 
    if(writeMSR(1, "FORK ERROR\n", 11) < 0) perror();
    else perror();
  }
  else if(pid == 0) block();*/
  
  /*if(writeMSR(1, "EXIT\n", 6) < 0) perror(); // EXIT
  pid = fork();
  if(pid < 0) { 
    if(writeMSR(1, "FORK ERROR\n", 11) < 0) perror();
    else perror();
  }
  else if(pid > 0) exit();*/

  /*if(writeMSR(1, "UNBLOCK\n", 8) < 0) perror(); // UNBLOCK
  pid = fork();
  if(pid < 0) { 
    if(writeMSR(1, "FORK ERROR\n", 11) < 0) perror();
    else perror();
  }
  else if(pid == 0) block();
  else {
    int i = 100000001; // DELAY
    while(i-- != 0);
    if(unblock(pid) == 0) {
      if(writeMSR(1, "Unblocked\n", 10) < 0) perror();
      while(1){}
    }
    else {
      if(writeMSR(1, "Error\n", 6) < 0) perror();
      perror();
      while(1){}
    }
  }*/

  //while(fork() >= 0); // RR
  /*while(1) { 
    itoa(getpid(), buff); 
    if(writeMSR(1, "\nGet pid: ", 9) < 0) perror();
    if(writeMSR(1, buff, strlen(buff)) < 0) perror();
    int i = 10000001; // AVOID CONSOLE SPAM
    while(i-- != 0) {}
  }*/

  while(1){}
}
