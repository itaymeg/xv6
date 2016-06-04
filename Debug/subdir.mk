################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../bio.o \
../bootasm.o \
../bootblock.o \
../bootblockother.o \
../bootmain.o \
../cat.o \
../console.o \
../echo.o \
../entry.o \
../entryother.o \
../exec.o \
../file.o \
../forktest.o \
../fs.o \
../grep.o \
../ide.o \
../init.o \
../initcode.o \
../ioapic.o \
../kalloc.o \
../kbd.o \
../kill.o \
../lapic.o \
../ln.o \
../log.o \
../ls.o \
../main.o \
../mkdir.o \
../mp.o \
../myMemTest.o \
../picirq.o \
../pipe.o \
../printf.o \
../proc.o \
../rm.o \
../sh.o \
../spinlock.o \
../stressfs.o \
../string.o \
../swtch.o \
../syscall.o \
../sysfile.o \
../sysproc.o \
../timer.o \
../trap.o \
../trapasm.o \
../uart.o \
../ulib.o \
../umalloc.o \
../usertests.o \
../usys.o \
../vectors.o \
../vm.o \
../wc.o \
../zombie.o 

C_SRCS += \
../bio.c \
../bootmain.c \
../cat.c \
../console.c \
../echo.c \
../exec.c \
../file.c \
../forktest.c \
../fs.c \
../grep.c \
../ide.c \
../init.c \
../ioapic.c \
../kalloc.c \
../kbd.c \
../kill.c \
../lapic.c \
../ln.c \
../log.c \
../ls.c \
../main.c \
../memide.c \
../mkdir.c \
../mkfs.c \
../mp.c \
../myMemTest.c \
../picirq.c \
../pipe.c \
../printf.c \
../proc.c \
../rm.c \
../sh.c \
../spinlock.c \
../stressfs.c \
../string.c \
../syscall.c \
../sysfile.c \
../sysproc.c \
../timer.c \
../trap.c \
../uart.c \
../ulib.c \
../umalloc.c \
../usertests.c \
../vm.c \
../wc.c \
../zombie.c 

S_UPPER_SRCS += \
../bootasm.S \
../entry.S \
../entryother.S \
../initcode.S \
../swtch.S \
../trapasm.S \
../usys.S \
../vectors.S 

ASM_SRCS += \
../bootblock.asm \
../cat.asm \
../echo.asm \
../entryother.asm \
../forktest.asm \
../grep.asm \
../init.asm \
../initcode.asm \
../kernel.asm \
../kill.asm \
../ln.asm \
../ls.asm \
../mkdir.asm \
../myMemTest.asm \
../rm.asm \
../sh.asm \
../stressfs.asm \
../usertests.asm \
../wc.asm \
../zombie.asm 

OBJS += \
./bio.o \
./bootasm.o \
./bootblock.o \
./bootmain.o \
./cat.o \
./console.o \
./echo.o \
./entry.o \
./entryother.o \
./exec.o \
./file.o \
./forktest.o \
./fs.o \
./grep.o \
./ide.o \
./init.o \
./initcode.o \
./ioapic.o \
./kalloc.o \
./kbd.o \
./kernel.o \
./kill.o \
./lapic.o \
./ln.o \
./log.o \
./ls.o \
./main.o \
./memide.o \
./mkdir.o \
./mkfs.o \
./mp.o \
./myMemTest.o \
./picirq.o \
./pipe.o \
./printf.o \
./proc.o \
./rm.o \
./sh.o \
./spinlock.o \
./stressfs.o \
./string.o \
./swtch.o \
./syscall.o \
./sysfile.o \
./sysproc.o \
./timer.o \
./trap.o \
./trapasm.o \
./uart.o \
./ulib.o \
./umalloc.o \
./usertests.o \
./usys.o \
./vectors.o \
./vm.o \
./wc.o \
./zombie.o 

C_DEPS += \
./bio.d \
./bootmain.d \
./cat.d \
./console.d \
./echo.d \
./exec.d \
./file.d \
./forktest.d \
./fs.d \
./grep.d \
./ide.d \
./init.d \
./ioapic.d \
./kalloc.d \
./kbd.d \
./kill.d \
./lapic.d \
./ln.d \
./log.d \
./ls.d \
./main.d \
./memide.d \
./mkdir.d \
./mkfs.d \
./mp.d \
./myMemTest.d \
./picirq.d \
./pipe.d \
./printf.d \
./proc.d \
./rm.d \
./sh.d \
./spinlock.d \
./stressfs.d \
./string.d \
./syscall.d \
./sysfile.d \
./sysproc.d \
./timer.d \
./trap.d \
./uart.d \
./ulib.d \
./umalloc.d \
./usertests.d \
./vm.d \
./wc.d \
./zombie.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler'
	as  -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.asm
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler'
	as  -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


