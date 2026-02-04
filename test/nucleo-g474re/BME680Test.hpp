#pragma once

#include <coco/BME680.hpp>
#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/SpiMaster_SPI_DMA.hpp>
#include <coco/board/config.hpp>


using namespace coco;
using namespace coco::literals;


/// @brief Drivers for BME680Test with BME680 connected via SPI.
///
/// Use BME680 module and connect it to the nucleo board like this:
/// GND -> CN10 20 (GND)
/// 3V3 -> CN7 16 (+3V3)
/// SCL -> CN10 30 (PB13)
/// SDA -> CN10 26 (PB15)
/// SDO -> CN10 28 (PB14)
/// nCS -> CN10 24 (PB1)
struct Drivers {
    Loop_TIM2 loop{APB1_TIMER_CLOCK, Loop_TIM2::Mode::POLL};

    using SpiMaster = SpiMaster_SPI_DMA;

    // SPI for sensor
    SpiMaster sensorSpi{loop,
        gpio::PB13 | gpio::AF5 | gpio::Config::SPEED_MEDIUM, // SPI2 SCK (CN10 30)
        gpio::PB15 | gpio::AF5 | gpio::Config::SPEED_MEDIUM, // SPI2 MOSI (CN10 26)
        gpio::PB14 | gpio::AF5 | gpio::Config::SPEED_MEDIUM, // SPI2 MISO (CN10 28)
        spi::SPI2_INFO,
        dma::DMA1_CH3_CH4_INFO};
    SpiMaster::Channel sensorChannel{sensorSpi,
        gpio::PB1 | gpio::Config::SPEED_MEDIUM | gpio::Config::INVERT, // nCS (CN10 24)
        spi::Format::CLOCK_DIV_128 | spi::Format::PHA0_POL0 | spi::Format::DATA_8};
    SpiMaster::Buffer<1, 128/*BME680::BUFFER_SIZE*/> sensorBuffer{sensorChannel}; // 128 for debug-printing banks

    // sensor
    BME680 sensor{loop, sensorBuffer, {
        BME680::Mode::SPI,
        BME680::Oversampling::X2, // temperature oversampling
        BME680::Oversampling::X2, // pressure oversampling
        BME680::Filter::C7, // filter for temperature and pressure
        BME680::Oversampling::X2, // humidity oversampling
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
