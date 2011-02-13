#define SYSCALLS_NUM_EXIT 1
#define SYSCALLS_NUM_READ 3
#define SYSCALLS_NUM_WRITE 4
#define SYSCALLS_NUM_LS 5
#define SYSCALLS_NUM_PS 6
#define SYSCALLS_NUM_RUN 7
#define SYSCALLS_NUM_DEVREQ 8
#define SYSCALLS_NUM_DEVREL 9
#define SYSCALLS_NUM_NICE 10


void sys_exit(task_t *task);
int sys_write(int to, char *buf, int bufsize);
int sys_read(int from, char *buf, int bufsize);
int sys_ls(int mode, char *buf, int bufsize);
int sys_ps(int mode, char *buf, int bufsize);
int sys_run(char *progname);
int sys_devreq(int devnum);
int sys_devrel(int devnum);
int sys_nice(uint32_t pid, uint32_t value);
