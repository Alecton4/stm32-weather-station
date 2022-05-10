#ifndef MY_SD_CARD_H
#define MY_SD_CARD_H

#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>

#define SD_CARD_DEV_NAME "sd0"
#define SD_CARD_MOUNT_PATH "/"
#define SD_CARD_FS "elm"

rt_err_t sdcard_init(void);
rt_err_t sdcard_unmount(void);

#endif