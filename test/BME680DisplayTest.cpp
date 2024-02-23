#include <BME680DisplayTest.hpp>
#include <coco/SSD130x.hpp>
#include <coco/font/tahoma8pt1bpp.hpp>
#include <coco/Loop.hpp>
#include <coco/StreamOperators.hpp>
#include <coco/StringBuffer.hpp>
#include <coco/debug.hpp>

/*
    This test reads the BME680 air sensor and shows the measured values on a display.
    See board specific BME680DisplayTest.hpp for how to connect the BME680 module and the display
*/

using namespace coco;


struct Info {
    // number of decimals to display (negative to disalbe suppression of trailing zeros)
    int decimalCount;

    // unit of measurement
    String unit;
};

constexpr Info infos[] = {
    {-1, "°C"}, // temperature
    {-1, "%"}, // air humidity
    {0, "hPa"}, // air pressure
    {-1, "Ω"} // air quality (measures a resistance)
};


Coroutine test(Loop &loop, Buffer &displayBuffer, InputDevice &sensor) {
    SSD130x display(displayBuffer, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_FLAGS);

    // reset and initialize display
    co_await drivers.resetDisplay();
    co_await display.init();

    // clear and enable display
    display.bitmap().clear();
    co_await display.show();
    co_await display.enable();

    while (true) {
        // get measured values
        float values[std::size(infos)];
        int sequenceNumber = sensor.get(values);

        // draw values onto display
        Bitmap bitmap = display.bitmap();
        bitmap.clear();
        for (size_t i = 0; i < std::size(infos); ++i) {
            StringBuffer<32> b;
            auto &info = infos[i];
            b << flt(values[i], info.decimalCount) << info.unit;
            bitmap.drawText(0, i * (tahoma8pt1bpp.height + 2), tahoma8pt1bpp, b);
        }
        co_await display.show();

        // wait for new input data (with new sequence number)
        co_await sensor.untilInput(sequenceNumber);
    }
}

int main() {
    test(drivers.loop, drivers.displayBuffer, drivers.sensor);

    drivers.loop.run();
}
