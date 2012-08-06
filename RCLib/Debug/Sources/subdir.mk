################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/AllocationAlgorithms.c \
../Sources/DataStructures.c \
../Sources/MemcachedInterface.c \
../Sources/ParticionesDinamicas.c \
../Sources/ReplacementAlgorithms.c \
../Sources/helpers.c \
../Sources/mathExtensions.c 

OBJS += \
./Sources/AllocationAlgorithms.o \
./Sources/DataStructures.o \
./Sources/MemcachedInterface.o \
./Sources/ParticionesDinamicas.o \
./Sources/ReplacementAlgorithms.o \
./Sources/helpers.o \
./Sources/mathExtensions.o 

C_DEPS += \
./Sources/AllocationAlgorithms.d \
./Sources/DataStructures.d \
./Sources/MemcachedInterface.d \
./Sources/ParticionesDinamicas.d \
./Sources/ReplacementAlgorithms.d \
./Sources/helpers.d \
./Sources/mathExtensions.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -D_FILE_OFFSET_BITS=64 -I"/home/utnso/Desarrollo/Workspace/sisoputnfrba-memcached_engine_example-b007422/memcached-1.6/include" -I"/home/utnso/Desarrollo/Workspace/commons" -O0 -g3 -Wall -c -fmessage-length=0 -std=c99 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


