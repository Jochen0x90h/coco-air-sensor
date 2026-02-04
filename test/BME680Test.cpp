#include <BME680Test.hpp>
#include <coco/Loop.hpp>
#include <coco/convert.hpp>
#include <coco/debug.hpp>


/*
    This test reads the BME680 air sensor and prints the measured values to debug::out.
    See board specific BME680Test.hpp for how to connect the BME680 module.
*/

using namespace coco;

const String units[] = {
    "°C",
    "%",
    "hPa",
    "Ω",
};

Coroutine test(Loop &loop, InputDevice &sensor) {
    float values[4];
    int sequenceNumber = sensor.get(values);
    while (true) {
        // wait for new input data (with new sequence number)
        co_await sensor.untilInput(sequenceNumber);

        // get measured values
        sequenceNumber = sensor.get(values);

        // print measured values
        for (size_t i = 0; i < std::size(values); ++i) {
            debug::out << dec(values[i]) << units[i] << ' ';
        }
        debug::out << '\n';
    }
}

int main() {
    debug::out << "BME680Test\n";

    test(drivers.loop, drivers.sensor);

    drivers.loop.run();
}
