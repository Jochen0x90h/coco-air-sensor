#pragma once

#include <coco/BME680.hpp>
#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/SpiMaster_SPI_DMA.hpp>
#include <coco/board/config.hpp>


using namespace coco;
using namespace coco::literals;


/**
 * Drivers for BME680DisplayTest with UG_2864ASWPG01 and BME680 connected via SPI
 *
 * Use BME680 module and connect it to the nucleo board like this:
 * 3V3: CN7 16
 * GND: CN10 20
 * SCL: CN10 30
 * SDA: CN10 26
 * SDO: CN10 28
 * nCS -> CN10 24
 */
struct Drivers {
    Loop_TIM2 loop{APB1_TIMER_CLOCK, Loop_TIM2::Mode::POLL};

    using SpiMaster = SpiMaster_SPI_DMA;

    // SPI for sensor
    SpiMaster sensorSpi{loop,
        gpio::Config::PB13 | gpio::Config::AF5 | gpio::Config::SPEED_MEDIUM, // SPI2 SCK (CN10 30)
        gpio::Config::PB14 | gpio::Config::AF5 | gpio::Config::SPEED_MEDIUM, // SPI2 MISO (CN10 28)
        gpio::Config::PB15 | gpio::Config::AF5 | gpio::Config::SPEED_MEDIUM, // SPI2 MOSI (CN10 26)
        spi::SPI2_INFO,
        dma::DMA1_CH3_CH4_INFO,
        spi::Config::CLOCK_DIV128 | spi::Config::PHA0_POL0 | spi::Config::DATA_8};
    SpiMaster::Channel sensorChannel{sensorSpi, gpio::Config::PB1 | gpio::Config::SPEED_MEDIUM | gpio::Config::INVERT}; // nCS (CN10 24)
    SpiMaster::Buffer<BME680::BUFFER_SIZE> sensorBuffer{sensorChannel};

    // sensor
    BME680 sensor{loop, sensorBuffer, {
        BME680::Mode::SPI,
        BME680::Oversampling::X2,
        BME680::Oversampling::X2,
        BME680::Filter::C7,
        BME680::Oversampling::X2,
        200, // gas sensor temperature
        30ms} // according to data sheet, 20-30ms are needed to reach target temperature
    };
};

Drivers drivers;

extern "C" {
void DMA1_Channel3_IRQHandler() {
    drivers.sensorSpi.DMA_Rx_IRQHandler();
}
}
