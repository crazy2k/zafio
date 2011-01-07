struct waited_device_t {
    task_t* task;
    void* device;
};

struct keyboard_t {
    char buffer[256];
} keyboard;

extern waited_device_t wdevs[WAIITING_DEVS_NUM] = { NULL };
/*
wdevs[0] = { a_task, &kerboard  };

((kerboard *)wdevs[0].device)->buffer ; //el buffer del teclado
*/

