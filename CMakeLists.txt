cmake_minimum_required(VERSION 3.28.3)

project(stm32-firmware)

add_executable(main)

target_sources(main
	PRIVATE
		src/main.c
)

option(CMAKE_BUILD_TYPE "Debug mode" On)
