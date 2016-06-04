################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../PNN/bio.o \
../PNN/bootasm.o \
../PNN/bootblock.o \
../PNN/bootblockother.o \
../PNN/bootmain.o \
../PNN/cat.o \
../PNN/console.o \
../PNN/echo.o \
../PNN/entry.o \
../PNN/entryother.o \
../PNN/exec.o \
../PNN/file.o \
../PNN/forktest.o \
../PNN/fs.o \
../PNN/grep.o \
../PNN/ide.o \
../PNN/init.o \
../PNN/initcode.o \
../PNN/ioapic.o \
../PNN/kalloc.o \
../PNN/kbd.o \
../PNN/kill.o \
../PNN/lapic.o \
../PNN/ln.o \
../PNN/log.o \
../PNN/ls.o \
../PNN/main.o \
../PNN/mkdir.o \
../PNN/mp.o \
../PNN/myMemTest.o \
../PNN/picirq.o \
../PNN/pipe.o \
../PNN/printf.o \
../PNN/proc.o \
../PNN/rm.o \
../PNN/sanity.o \
../PNN/sh.o \
../PNN/spinlock.o \
../PNN/stressfs.o \
../PNN/string.o \
../PNN/swtch.o \
../PNN/syscall.o \
../PNN/sysfile.o \
../PNN/sysproc.o \
../PNN/timer.o \
../PNN/trap.o \
../PNN/trapasm.o \
../PNN/uart.o \
../PNN/ulib.o \
../PNN/umalloc.o \
../PNN/usertests.o \
../PNN/usys.o \
../PNN/vectors.o \
../PNN/vm.o \
../PNN/wc.o \
../PNN/zombie.o 

C_SRCS += \
../PNN/bio.c \
../PNN/bootmain.c \
../PNN/cat.c \
../PNN/console.c \
../PNN/echo.c \
../PNN/exec.c \
../PNN/file.c \
../PNN/forktest.c \
../PNN/fs.c \
../PNN/grep.c \
../PNN/ide.c \
../PNN/init.c \
../PNN/ioapic.c \
../PNN/kalloc.c \
../PNN/kbd.c \
../PNN/kill.c \
../PNN/lapic.c \
../PNN/ln.c \
../PNN/log.c \
../PNN/ls.c \
../PNN/main.c \
../PNN/memide.c \
../PNN/mkdir.c \
../PNN/mkfs.c \
../PNN/mp.c \
../PNN/myMemTest.c \
../PNN/picirq.c \
../PNN/pipe.c \
../PNN/printf.c \
../PNN/proc.c \
../PNN/rm.c \
../PNN/sanity.c \
../PNN/sh.c \
../PNN/spinlock.c \
../PNN/stressfs.c \
../PNN/string.c \
../PNN/syscall.c \
../PNN/sysfile.c \
../PNN/sysproc.c \
../PNN/testMassive.c \
../PNN/timer.c \
../PNN/trap.c \
../PNN/uart.c \
../PNN/ulib.c \
../PNN/umalloc.c \
../PNN/usertests.c \
../PNN/vm.c \
../PNN/wc.c \
../PNN/zombie.c 

S_UPPER_SRCS += \
../PNN/bootasm.S \
../PNN/entry.S \
../PNN/entryother.S \
../PNN/initcode.S \
../PNN/swtch.S \
../PNN/trapasm.S \
../PNN/usys.S \
../PNN/vectors.S 

ASM_SRCS += \
../PNN/bootblock.asm \
../PNN/cat.asm \
../PNN/echo.asm \
../PNN/entryother.asm \
../PNN/forktest.asm \
../PNN/grep.asm \
../PNN/init.asm \
../PNN/initcode.asm \
../PNN/kernel.asm \
../PNN/kill.asm \
../PNN/ln.asm \
../PNN/ls.asm \
../PNN/mkdir.asm \
../PNN/myMemTest.asm \
../PNN/rm.asm \
../PNN/sanity.asm \
../PNN/sh.asm \
../PNN/stressfs.asm \
../PNN/usertests.asm \
../PNN/wc.asm \
../PNN/zombie.asm 

OBJS += \
./PNN/bio.o \
./PNN/bootasm.o \
./PNN/bootblock.o \
./PNN/bootmain.o \
./PNN/cat.o \
./PNN/console.o \
./PNN/echo.o \
./PNN/entry.o \
./PNN/entryother.o \
./PNN/exec.o \
./PNN/file.o \
./PNN/forktest.o \
./PNN/fs.o \
./PNN/grep.o \
./PNN/ide.o \
./PNN/init.o \
./PNN/initcode.o \
./PNN/ioapic.o \
./PNN/kalloc.o \
./PNN/kbd.o \
./PNN/kernel.o \
./PNN/kill.o \
./PNN/lapic.o \
./PNN/ln.o \
./PNN/log.o \
./PNN/ls.o \
./PNN/main.o \
./PNN/memide.o \
./PNN/mkdir.o \
./PNN/mkfs.o \
./PNN/mp.o \
./PNN/myMemTest.o \
./PNN/picirq.o \
./PNN/pipe.o \
./PNN/printf.o \
./PNN/proc.o \
./PNN/rm.o \
./PNN/sanity.o \
./PNN/sh.o \
./PNN/spinlock.o \
./PNN/stressfs.o \
./PNN/string.o \
./PNN/swtch.o \
./PNN/syscall.o \
./PNN/sysfile.o \
./PNN/sysproc.o \
./PNN/testMassive.o \
./PNN/timer.o \
./PNN/trap.o \
./PNN/trapasm.o \
./PNN/uart.o \
./PNN/ulib.o \
./PNN/umalloc.o \
./PNN/usertests.o \
./PNN/usys.o \
./PNN/vectors.o \
./PNN/vm.o \
./PNN/wc.o \
./PNN/zombie.o 

C_DEPS += \
./PNN/bio.d \
./PNN/bootmain.d \
./PNN/cat.d \
./PNN/console.d \
./PNN/echo.d \
./PNN/exec.d \
./PNN/file.d \
./PNN/forktest.d \
./PNN/fs.d \
./PNN/grep.d \
./PNN/ide.d \
./PNN/init.d \
./PNN/ioapic.d \
./PNN/kalloc.d \
./PNN/kbd.d \
./PNN/kill.d \
./PNN/lapic.d \
./PNN/ln.d \
./PNN/log.d \
./PNN/ls.d \
./PNN/main.d \
./PNN/memide.d \
./PNN/mkdir.d \
./PNN/mkfs.d \
./PNN/mp.d \
./PNN/myMemTest.d \
./PNN/picirq.d \
./PNN/pipe.d \
./PNN/printf.d \
./PNN/proc.d \
./PNN/rm.d \
./PNN/sanity.d \
./PNN/sh.d \
./PNN/spinlock.d \
./PNN/stressfs.d \
./PNN/string.d \
./PNN/syscall.d \
./PNN/sysfile.d \
./PNN/sysproc.d \
./PNN/testMassive.d \
./PNN/timer.d \
./PNN/trap.d \
./PNN/uart.d \
./PNN/ulib.d \
./PNN/umalloc.d \
./PNN/usertests.d \
./PNN/vm.d \
./PNN/wc.d \
./PNN/zombie.d 


# Each subdirectory must supply rules for building sources it contributes
PNN/%.o: ../PNN/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

PNN/%.o: ../PNN/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler'
	as  -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

PNN/%.o: ../PNN/%.asm
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler'
	as  -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


