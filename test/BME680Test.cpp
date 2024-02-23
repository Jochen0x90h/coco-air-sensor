#include <BME680Test.hpp>
#include <coco/Loop.hpp>
#include <coco/debug.hpp>
#include <coco/StreamOperators.hpp>


using namespace coco;

const String units[] = {
    "°C",
    "%",
    "hPa",
    "Ω",
};

Coroutine test(Loop &loop, InputDevice &sensor) {
    while (true) {
        // get measured values
        float values[4];
        int sequenceNumber = sensor.get(values);

        // print measured values
        for (size_t i = 0; i < std::size(values); ++i) {
            debug::out << flt(values[i]) << units[i] << '\n';
        }

        // wait for new input data (with new sequence number)
        co_await sensor.untilInput(sequenceNumber);
    }
}

int main() {
    test(drivers.loop, drivers.sensor);

    drivers.loop.run();
}
