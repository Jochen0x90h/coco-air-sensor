#include "Sensor_native.hpp"


namespace coco {

Sensor_native::Sensor_native(Loop_native &loop, Array<const Config> config, Milliseconds<> interval)
    : InputDevice(State::READY)
    , loop(loop), config(config), interval(interval)
    , callback(makeCallback<Sensor_native, &Sensor_native::handle>(this))
{
    assert(config.size() <= std::size(this->values));
    int count = config.size();
    for (int i = 0; i < count; ++i) {
        this->values[i] = config[i].initialValue;
    }

    loop.invoke(this->callback, interval);
}

Sensor_native::~Sensor_native() {
}

int Sensor_native::get(void *data, int size) {
    int count = std::min(this->config.size(), int(size / sizeof(float)));
    float *values = reinterpret_cast<float *>(data);
    for (int i = 0; i < count; ++i) {
        values[i] = this->values[i];
    }
    return this->sequenceNumber;
}

Awaitable<Device::Events> Sensor_native::untilInput(int sequenceNumber) {
    if (this->sequenceNumber != sequenceNumber)
        return {};
    return {this->st.tasks, Events::READABLE};
}

void Sensor_native::handle() {
    ++this->sequenceNumber;
    int count = config.size();
    for (int i = 0; i < count; ++i) {
        auto &c = this->config[i];

        // generate random deviation
        float r = int(this->random.draw()) * 4.6566e-10 * c.deviation;

        this->values[i] = c.initialValue + r;
    }
    //this->changeTasks.doAll();
    this->st.notify(Events::READABLE);

    this->loop.invoke(this->callback, this->interval);
}

} // namespace coco
