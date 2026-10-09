#ifndef KERNEL_H
#define KERNEL_H

#include "types.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"
#include "memory.h"
#include "vfs.h"
#include "vesa.h"
#include "keyboard.h"
#include "mouse.h"
#include "speaker.h"
#include "gamepad.h"
#include "rtc.h"
#include "font.h"
#include "window.h"
#include "compositor.h"
#include "apps.h"
#include "games.h"
#include "store.h"
#include "power.h"

#define OS_NAME "Yazan Game OS"
#define OS_VERSION "1.0.0 (Release)"
#define OS_ARCH "x86 (32-bit Bare Metal)"

#ifdef __cplusplus
extern "C" {
#endif

void kernel_main(uint32_t magic, uint32_t* info);

#ifdef __cplusplus
}
#endif

#endif // KERNEL_H
