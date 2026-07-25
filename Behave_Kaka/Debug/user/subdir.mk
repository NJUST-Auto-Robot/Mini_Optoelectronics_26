################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../user/cpu0_main.c \
../user/cpu1_main.c \
../user/dgconfig.c \
../user/isr.c 

COMPILED_SRCS += \
user/cpu0_main.src \
user/cpu1_main.src \
user/dgconfig.src \
user/isr.src 

C_DEPS += \
user/cpu0_main.d \
user/cpu1_main.d \
user/dgconfig.d \
user/isr.d 

OBJS += \
user/cpu0_main.o \
user/cpu1_main.o \
user/dgconfig.o \
user/isr.o 


# Each subdirectory must supply rules for building sources it contributes
user/cpu0_main.src: ../user/cpu0_main.c user/subdir.mk
	cctc -cs --dep-file="$(*F).d" --misrac-version=2004 -D__CPU__=tc36x "-fD:/techapps/AURIX/WorkPlace/Behave_Kaka/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc36x -Y0 -N0 -Z0 -o "$@" "$<"
user/cpu0_main.o: user/cpu0_main.src user/subdir.mk
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
user/cpu1_main.src: ../user/cpu1_main.c user/subdir.mk
	cctc -cs --dep-file="$(*F).d" --misrac-version=2004 -D__CPU__=tc36x "-fD:/techapps/AURIX/WorkPlace/Behave_Kaka/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc36x -Y0 -N0 -Z0 -o "$@" "$<"
user/cpu1_main.o: user/cpu1_main.src user/subdir.mk
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
user/dgconfig.src: ../user/dgconfig.c user/subdir.mk
	cctc -cs --dep-file="$(*F).d" --misrac-version=2004 -D__CPU__=tc36x "-fD:/techapps/AURIX/WorkPlace/Behave_Kaka/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc36x -Y0 -N0 -Z0 -o "$@" "$<"
user/dgconfig.o: user/dgconfig.src user/subdir.mk
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
user/isr.src: ../user/isr.c user/subdir.mk
	cctc -cs --dep-file="$(*F).d" --misrac-version=2004 -D__CPU__=tc36x "-fD:/techapps/AURIX/WorkPlace/Behave_Kaka/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc36x -Y0 -N0 -Z0 -o "$@" "$<"
user/isr.o: user/isr.src user/subdir.mk
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-user

clean-user:
	-$(RM) user/cpu0_main.d user/cpu0_main.o user/cpu0_main.src user/cpu1_main.d user/cpu1_main.o user/cpu1_main.src user/dgconfig.d user/dgconfig.o user/dgconfig.src user/isr.d user/isr.o user/isr.src

.PHONY: clean-user

