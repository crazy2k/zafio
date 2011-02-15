#ifndef __SCHED_H__

#define __SCHED_H__

int ps(uint32_t mode, char *buf, uint32_t bufsize);
int ls(uint32_t mode, char *buf, uint32_t bufsize);
int run(char *progname);
int nice(uint32_t pid, uint32_t value);
int waitpid(uint32_t pid);

#endif
