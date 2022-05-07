#ifndef __FONT_H
#define __FONT_H

#include <board.h>

#define LINE(x) ((x) * (((struct sFONT *)LCD_GetFont())->Height))
#define LINEY(x) ((x) * (((struct sFONT *)LCD_GetFont())->Width))

/** @defgroup FONTS_Exported_Types
  * @{
  */
struct sFONT {
	const uint8_t *table;
	uint16_t Width;
	uint16_t Height;
};

extern struct sFONT Font24x32;
extern struct sFONT Font16x24;
extern struct sFONT Font8x16;

//要支持中文需要实现本函数，可参考“液晶显示中英文（字库在外部FLASH）”例程
#define GetGBKCode(ucBuffer, usChar)

#endif /*end of __FONT_H    */
