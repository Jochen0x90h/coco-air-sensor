#pragma once

#include <coco/SSD130x.hpp>
#include <coco/BME680.hpp>
#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/SpiMaster_SPI_DMA.hpp>
#include <coco/platform/SpiDisplayChannel_SPI_DMA.hpp>
#include <coco/platform/OutputPort_GPIO.hpp>
#include <coco/board/config.hpp>


using namespace coco;
using namespace coco::literals;

constexpr int DISPLAY_WIDTH = 128;
constexpr int DISPLAY_HEIGHT = 64;
constexpr SSD130x::Flags DISPLAY_FLAGS = SSD130x::Flags::UG_2864ASWPG01_SPI;


/// @brief Drivers for BME680DisplayTest with UG_2864ASWPG01 and BME680 connected via SPI.
///
/// Use pcb/Display or other SSD1309/SPI display module and connect it to the nucleo board like this:
/// GND -> CN5 7 (GND)
/// 3V3 -> CN6 4 (+3V3)
/// 12V -> CN6 5 (+5V, the nucleo board provides only 5V which is sufficient for testing)
/// SCK -> CN9 4 (PB3)
/// MOSI -> CN9 5 (PB5)
/// D/nC -> CN9 8 (PA8)
/// nCS -> CN5 1 (PA9)
/// nRST -> CN9 7 (PB10, or reset the display module by hand by connecting RESET to GND, then reset the nucleo board)
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

    // SPI for display
    SpiMaster displaySpi{loop,
        gpio::PB3 | gpio::AF5 | gpio::Config::SPEED_MEDIUM, // SPI1 SCK (CN9 4) (don't forget to lookup the alternate function number in the data sheet!)
        gpio::PB5 | gpio::AF5 | gpio::Config::SPEED_MEDIUM, // SPI1 MOSI (CN9 5)
        gpio::NONE, // SPI1 MISO (CN9 6), not used
        spi::SPI1_INFO,
        dma::DMA1_CH1_CH2_INFO};
   SpiDisplayChannel_SPI_DMA displayChannel{displaySpi,
        gpio::PA9 | gpio::Config::SPEED_MEDIUM | gpio::Config::INVERT, // nCS (CN5 1)
        gpio::PA8 | gpio::Config::SPEED_MEDIUM, false, 0x40, // D/nC (CN9 8)
        spi::Format::CLOCK_DIV_128 | spi::Format::PHA1_POL1 | spi::Format::DATA_8};
    SpiMaster::Buffer<1, DISPLAY_WIDTH * DISPLAY_HEIGHT / 8> displayBuffer{displayChannel};

    // reset pin of display
    static constexpr OutputPort_GPIO::Config outConfig[] {
        {gpio::PB10 | gpio::Config::INVERT, false}, // nRST (CN9 7)
    };
    OutputPort_GPIO resetPin{outConfig};

    // SPI for sensor
    SpiMaster sensorSpi{loop,
        gpio::PB13 | gpio::AF5 | gpio::Config::SPEED_MEDIUM, // SPI2 SCK (CN10 30)
        gpio::PB14 | gpio::AF5 | gpio::Config::SPEED_MEDIUM, // SPI2 MISO (CN10 28)
        gpio::PB15 | gpio::AF5 | gpio::Config::SPEED_MEDIUM, // SPI2 MOSI (CN10 26)
        spi::SPI2_INFO,
        dma::DMA1_CH3_CH4_INFO};
    SpiMaster::Channel sensorChannel{sensorSpi,
        gpio::PB1 | gpio::Config::SPEED_MEDIUM | gpio::Config::INVERT, // nCS (CN10 24)
        spi::Format::CLOCK_DIV_128 | spi::Format::PHA0_POL0 | spi::Format::DATA_8};
    SpiMaster::Buffer<1, 128/*BME680::BUFFER_SIZE*/> sensorBuffer{sensorChannel}; // 128 for debug-printing banks

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
void DMA1_Channel1_IRQHandler() {
    drivers.displaySpi.DMA_Rx_IRQHandler();
}
void DMA1_Channel3_IRQHandler() {
    drivers.sensorSpi.DMA_Rx_IRQHandler();
}
}
