################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/commons/collections/dictionary.c \
../Sources/commons/collections/list.c \
../Sources/commons/collections/queue.c 

OBJS += \
./Sources/commons/collections/dictionary.o \
./Sources/commons/collections/list.o \
./Sources/commons/collections/queue.o 

C_DEPS += \
./Sources/commons/collections/dictionary.d \
./Sources/commons/collections/list.d \
./Sources/commons/collections/queue.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/commons/collections/%.o: ../Sources/commons/collections/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -D_FILE_OFFSET_BITS=64 -I"/home/utnso/Desarrollo/Workspace/sisoputnfrba-memcached_engine_example-b007422/memcached-1.6/include" -I"/home/utnso/Desarrollo/Workspace/commons" -O0 -g3 -Wall -c -fmessage-length=0 -std=c99 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


