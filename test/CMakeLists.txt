# Generate a test for a board
# TEST the test application, implemented in ${TEST}.cpp
# BOARD-LIB a library for a board containing SystemInit() and a linker script
function(board_test TEST BOARD_LIB)
    # check if board library exists for the current platform
    if(TARGET ${BOARD_LIB})
        string(REGEX REPLACE ".*\\:" "" BOARD ${BOARD_LIB})
        set(NAME "${TEST}-${BOARD}")
        message("*** Test ${TEST} on board ${BOARD}")

        add_executable(${NAME}
            ${TEST}.cpp
        )
        target_include_directories(${NAME}
            PRIVATE
                ../
                ${BOARD}
        )
        target_link_libraries(${NAME}
            ${BOARD_LIB}
            coco-spi::coco-spi
            coco-i2c::coco-i2c
            coco-mono-display::coco-mono-display
            ${PROJECT_NAME}
        )

        # generate hex file for flashing the target
        if(${CMAKE_CROSSCOMPILING})
            #message("*** Generate Hex for ${NAME} using ${CMAKE_OBJCOPY}")
            add_custom_command(TARGET ${NAME}
                POST_BUILD
                COMMAND ${CMAKE_OBJCOPY} -O ihex ${NAME} ${NAME}.hex
            )
        endif()
    endif()
endfunction()

board_test(BME680Test coco-devboards::native)
#board_test(BME680Test coco-devboards::nucleo-c031c6)
#board_test(BME680Test coco-devboards::nucleo-g431rb)
board_test(BME680Test coco-devboards::nucleo-g474re)

board_test(BME680DisplayTest coco-devboards::emu)
board_test(BME680DisplayTest coco-devboards::nucleo-g431rb)



# test for the official Bosch BME68x Sensor API
if(NOT ${CMAKE_CROSSCOMPILING})
    add_executable(BME68x_SensorAPI
        BME68x_SensorAPI/main.cpp
        BME68x_SensorAPI/bme68x.c
        BME68x_SensorAPI/bme68x.h
        BME68x_SensorAPI/bme68x_defs.h
    )
endif()
