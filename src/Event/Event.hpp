#pragma once

class Event {
public:
    bool canceled = false;

    void cancel() {
        canceled = true;
    }
};
