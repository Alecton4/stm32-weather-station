#include "SD_card.h"
#include <dfs_posix.h> /* 当需要使用文件操作时，需要包含这个头文件 */
// for outputing debugging info
#define DBG_TAG "SD_CARD"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_err_t sdcard_init(void)
{
	rt_thread_mdelay(50); // 这段延时必须加上，系统上电过程中存在延时，否则会出现先挂载后注册块设备sd0的情况
	// 挂载文件系统，参数：块设备名称、挂载目录、文件系统类型、读写标志、私有数据0
	if (dfs_mount(SD_CARD_DEV_NAME, SD_CARD_MOUNT_PATH, SD_CARD_FS, 0, 0) == 0) {
		return RT_EOK;
	} else {
		return -RT_ERROR;
	}
}

rt_err_t sdcard_init_format(void)
{
	rt_thread_mdelay(50); // 这段延时必须加上，系统上电过程中存在延时，否则会出现先挂载后注册块设备sd0的情况
	mkfs("elm", "sd0"); // format SD card
	// 挂载文件系统，参数：块设备名称、挂载目录、文件系统类型、读写标志、私有数据0
	if (dfs_mount(SD_CARD_DEV_NAME, SD_CARD_MOUNT_PATH, SD_CARD_FS, 0, 0) == 0) {
		return RT_EOK;
	} else {
		return -RT_ERROR;
	}
}

rt_err_t sdcard_unmount(void)
{
	if (dfs_unmount(SD_CARD_MOUNT_PATH) == 0) {
		return RT_EOK;
	} else {
		return -RT_ERROR;
	}
}
MSH_CMD_EXPORT(sdcard_unmount, eject sd card);

void fill_buffer(rt_uint8_t *buff, rt_uint32_t buff_length)
{
	rt_uint32_t index;
	/* 往缓冲区填充随机数 */
	for (index = 0; index < buff_length; index++) {
		buff[index] = ((rt_uint8_t)rand()) & 0xff;
	}
}

static int sd_sample(int argc, char *argv[])
{
	rt_err_t ret;
	rt_device_t sd_device;
	char sd_name[RT_NAME_MAX];
	rt_uint8_t *write_buff, *read_buff;
	struct rt_device_blk_geometry geo;
	rt_uint8_t block_num;
	/* 判断命令行参数是否给定了设备名称 */
	if (argc == 2) {
		rt_strncpy(sd_name, argv[1], RT_NAME_MAX);
	} else {
		rt_strncpy(sd_name, SD_CARD_DEV_NAME, RT_NAME_MAX);
	}
	/* 查找设备获取设备句柄 */
	sd_device = rt_device_find(sd_name);
	if (sd_device == RT_NULL) {
		LOG_E("find device %s failed!\r\n", sd_name);
		return RT_ERROR;
	}
	/* 打开设备 */
	// ret = rt_device_open(sd_device, RT_DEVICE_OFLAG_RDWR);
	// if (ret != RT_EOK) {
	// 	LOG_E("ret: %d", ret);
	// 	rt_kprintf("open device %s failed!\r\n", sd_name);
	// 	return ret;
	// }
	rt_memset(&geo, 0, sizeof(geo));
	/* 获取块设备信息 */
	ret = rt_device_control(sd_device, RT_DEVICE_CTRL_BLK_GETGEOME, &geo);
	if (ret != RT_EOK) {
		LOG_E("control device %s failed!\r\n", sd_name);
		return ret;
	}
	LOG_D("device information:\r\n");
	LOG_D("sector  size : %d byte\r\n", geo.bytes_per_sector);
	LOG_D("sector count : %d \r\n", geo.sector_count);
	LOG_D("block   size : %d byte\r\n", geo.block_size);
	/* 准备读写缓冲区空间，大小为一个块 */
	read_buff = rt_malloc(geo.block_size);
	if (read_buff == RT_NULL) {
		LOG_E("no memory for read buffer!\r\n");
		return RT_ERROR;
	}
	write_buff = rt_malloc(geo.block_size);
	if (write_buff == RT_NULL) {
		LOG_E("no memory for write buffer!\r\n");
		rt_free(read_buff);
		return RT_ERROR;
	}
	/* 填充写数据缓冲区，为写操作做准备 */
	fill_buffer(write_buff, geo.block_size);
	/* 把写数据缓冲的数据写入SD卡中，大小为一个块，size参数以块为单位 */
	block_num = rt_device_write(sd_device, 0, write_buff, 1);
	if (1 != block_num) {
		LOG_E("write device %s failed!\r\n", sd_name);
	}
	/* 从SD卡中读出数据，并保存在读数据缓冲区中 */
	block_num = rt_device_read(sd_device, 0, read_buff, 1);
	if (1 != block_num) {
		LOG_E("read %s device failed!\r\n", sd_name);
	}
	/* 比较写数据缓冲区和读数据缓冲区的内容是否完全一致 */
	if (rt_memcmp(write_buff, read_buff, geo.block_size) == 0) {
		LOG_D("Block test OK!\r\n");
	} else {
		LOG_E("Block test Fail!\r\n");
	}
	/* 释放缓冲区空间 */
	rt_free(read_buff);
	rt_free(write_buff);
	return RT_EOK;
}
MSH_CMD_EXPORT(sd_sample, sd device sample);

/**
 * O_RDONLY 	只读方式打开
 * O_WRONLY 	只写方式打开
 * O_RDWR 	以读写方式打开
 * O_CREAT 	如果要打开的文件不存在，则建立该文件
 * O_APPEND 	当读写文件时会从文件尾开始移动，也就是说写入的数据会以附加的方式添加到文件的尾部。
 * O_TRUNC 	如果文件已经存在，则清空文件中的内容
*/

static void append_sample_1(void)
{
	int fd, size;
	char s[] = "RT-Thread Programmer!", buffer[80];
	// !!! important
	s[rt_strlen(s)] = '\n';

	LOG_D("Write string %s to test.txt.\n", s);

	/* 以创建和读写模式打开 /text.txt 文件，如果该文件不存在则创建该文件 */
	fd = open("/text1.txt", O_WRONLY | O_CREAT | O_APPEND);
	if (fd >= 0) {
		write(fd, s, sizeof(s));
		write(fd, s, sizeof(s));
		close(fd);
		LOG_D("Write done.\n");
	}
}
MSH_CMD_EXPORT(append_sample_1, append text sample);

static void readwrite_sample(void)
{
	int fd, size;
	char s[] = "RT-Thread Programmer!", buffer[80];

	LOG_D("Write string %s to test.txt.\r\n", s);

	/* 以创建和读写模式打开 /text.txt 文件，如果该文件不存在则创建该文件 */
	fd = open("/text.txt", O_WRONLY | O_CREAT | O_APPEND);
	if (fd >= 0) {
		write(fd, s, sizeof(s));
		close(fd);
		LOG_D("Write done.\r\n");
	}

	/* 以只读模式打开 /text.txt 文件 */
	fd = open("/text.txt", O_RDONLY);
	if (fd >= 0) {
		size = read(fd, buffer, sizeof(buffer));
		close(fd);
		LOG_D("Read from file test.txt : %s \r\n", buffer);
		if (size < 0)
			return;
	}
}