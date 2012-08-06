################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/commons/bitarray.c \
../Sources/commons/config.c \
../Sources/commons/string.c 

OBJS += \
./Sources/commons/bitarray.o \
./Sources/commons/config.o \
./Sources/commons/string.o 

C_DEPS += \
./Sources/commons/bitarray.d \
./Sources/commons/config.d \
./Sources/commons/string.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/commons/%.o: ../Sources/commons/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -D_FILE_OFFSET_BITS=64 -I"/home/utnso/Desarrollo/Workspace/sisoputnfrba-memcached_engine_example-b007422/memcached-1.6/include" -I"/home/utnso/Desarrollo/Workspace/commons" -O0 -g3 -Wall -c -fmessage-length=0 -std=c99 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


