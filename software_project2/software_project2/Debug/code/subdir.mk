################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../code/Anticogging.c" \
"../code/Calibration.c" \
"../code/Controller.c" \
"../code/Encoder.c" \
"../code/FOC.c" \
"../code/FSM.c" \
"../code/Heap.c" \
"../code/PWM.c" \
"../code/Receiver.c" \
"../code/Uart.c" \
"../code/User_Config.c" \
"../code/Util.c" \
"../code/camera_huidu.c" \
"../code/image_process.c" \
"../code/trapTraj.c" 

COMPILED_SRCS += \
"code/Anticogging.src" \
"code/Calibration.src" \
"code/Controller.src" \
"code/Encoder.src" \
"code/FOC.src" \
"code/FSM.src" \
"code/Heap.src" \
"code/PWM.src" \
"code/Receiver.src" \
"code/Uart.src" \
"code/User_Config.src" \
"code/Util.src" \
"code/camera_huidu.src" \
"code/image_process.src" \
"code/trapTraj.src" 

C_DEPS += \
"./code/Anticogging.d" \
"./code/Calibration.d" \
"./code/Controller.d" \
"./code/Encoder.d" \
"./code/FOC.d" \
"./code/FSM.d" \
"./code/Heap.d" \
"./code/PWM.d" \
"./code/Receiver.d" \
"./code/Uart.d" \
"./code/User_Config.d" \
"./code/Util.d" \
"./code/camera_huidu.d" \
"./code/image_process.d" \
"./code/trapTraj.d" 

OBJS += \
"code/Anticogging.o" \
"code/Calibration.o" \
"code/Controller.o" \
"code/Encoder.o" \
"code/FOC.o" \
"code/FSM.o" \
"code/Heap.o" \
"code/PWM.o" \
"code/Receiver.o" \
"code/Uart.o" \
"code/User_Config.o" \
"code/Util.o" \
"code/camera_huidu.o" \
"code/image_process.o" \
"code/trapTraj.o" 


# Each subdirectory must supply rules for building sources it contributes
"code/Anticogging.src":"../code/Anticogging.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc36x "-fE:/software_project/software_project2/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc36x -Y0 -N0 -Z0 -o "$@" "$<"
"code/Anticogging.o":"code/Anticogging.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Calibration.src":"../code/Calibration.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc36x "-fE:/software_project/software_project2/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc36x -Y0 -N0 -Z0 -o "$@" "$<"
"code/Calibration.o":"code/Calibration.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Controller.src":"../code/Controller.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc36x "-fE:/software_project/software_project2/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc36x -Y0 -N0 -Z0 -o "$@" "$<"
"code/Controller.o":"code/Controller.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Encoder.src":"../code/Encoder.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc36x "-fE:/software_project/software_project2/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc36x -Y0 -N0 -Z0 -o "$@" "$<"
"code/Encoder.o":"code/Encoder.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/FOC.src":"../code/FOC.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc36x "-fE:/software_project/software_project2/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc36x -Y0 -N0 -Z0 -o "$@" "$<"
"code/FOC.o":"code/FOC.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/FSM.src":"../code/FSM.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc36x "-fE:/software_project/software_project2/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc36x -Y0 -N0 -Z0 -o "$@" "$<"
"code/FSM.o":"code/FSM.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Heap.src":"../code/Heap.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc36x "-fE:/software_project/software_project2/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc36x -Y0 -N0 -Z0 -o "$@" "$<"
"code/Heap.o":"code/Heap.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/PWM.src":"../code/PWM.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc36x "-fE:/software_project/software_project2/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc36x -Y0 -N0 -Z0 -o "$@" "$<"
"code/PWM.o":"code/PWM.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Receiver.src":"../code/Receiver.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc36x "-fE:/software_project/software_project2/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc36x -Y0 -N0 -Z0 -o "$@" "$<"
"code/Receiver.o":"code/Receiver.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Uart.src":"../code/Uart.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc36x "-fE:/software_project/software_project2/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc36x -Y0 -N0 -Z0 -o "$@" "$<"
"code/Uart.o":"code/Uart.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/User_Config.src":"../code/User_Config.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc36x "-fE:/software_project/software_project2/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc36x -Y0 -N0 -Z0 -o "$@" "$<"
"code/User_Config.o":"code/User_Config.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/Util.src":"../code/Util.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc36x "-fE:/software_project/software_project2/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc36x -Y0 -N0 -Z0 -o "$@" "$<"
"code/Util.o":"code/Util.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/camera_huidu.src":"../code/camera_huidu.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc36x "-fE:/software_project/software_project2/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc36x -Y0 -N0 -Z0 -o "$@" "$<"
"code/camera_huidu.o":"code/camera_huidu.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/image_process.src":"../code/image_process.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc36x "-fE:/software_project/software_project2/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc36x -Y0 -N0 -Z0 -o "$@" "$<"
"code/image_process.o":"code/image_process.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/trapTraj.src":"../code/trapTraj.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc36x "-fE:/software_project/software_project2/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc36x -Y0 -N0 -Z0 -o "$@" "$<"
"code/trapTraj.o":"code/trapTraj.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-code

clean-code:
	-$(RM) ./code/Anticogging.d ./code/Anticogging.o ./code/Anticogging.src ./code/Calibration.d ./code/Calibration.o ./code/Calibration.src ./code/Controller.d ./code/Controller.o ./code/Controller.src ./code/Encoder.d ./code/Encoder.o ./code/Encoder.src ./code/FOC.d ./code/FOC.o ./code/FOC.src ./code/FSM.d ./code/FSM.o ./code/FSM.src ./code/Heap.d ./code/Heap.o ./code/Heap.src ./code/PWM.d ./code/PWM.o ./code/PWM.src ./code/Receiver.d ./code/Receiver.o ./code/Receiver.src ./code/Uart.d ./code/Uart.o ./code/Uart.src ./code/User_Config.d ./code/User_Config.o ./code/User_Config.src ./code/Util.d ./code/Util.o ./code/Util.src ./code/camera_huidu.d ./code/camera_huidu.o ./code/camera_huidu.src ./code/image_process.d ./code/image_process.o ./code/image_process.src ./code/trapTraj.d ./code/trapTraj.o ./code/trapTraj.src

.PHONY: clean-code

