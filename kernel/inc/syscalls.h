#define SYSCALLS_NUM_EXIT 1
#define SYSCALLS_NUM_READ 3
#define SYSCALLS_NUM_WRITE 4
#define SYSCALLS_NUM_LS 5
#define SYSCALLS_NUM_PS 6
#define SYSCALLS_NUM_RUN 7
#define SYSCALLS_NUM_DEVREQ 8
#define SYSCALLS_NUM_DEVREL 9
#define SYSCALLS_NUM_NICE 10
#define SYSCALLS_NUM_WAITPID 11


void sys_exit(task_t *task);
int sys_write(uint32_t to, char *buf, uint32_t bufsize);
int sys_read(uint32_t from, char *buf, uint32_t bufsize);
int sys_ls(uint32_t mode, char *buf, uint32_t bufsize);
int sys_ps(uint32_t mode, char *buf, uint32_t bufsize);
int sys_run(char *progname);
int sys_devreq(uint32_t devnum);
int sys_devrel(uint32_t devnum);
int sys_nice(uint32_t pid, uint32_t value);
int sys_waitpid(uint32_t pid);
