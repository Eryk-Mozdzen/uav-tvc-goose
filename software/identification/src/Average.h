#pragma once

class Average {
    double avg = 0;
    int number = 0;

public:
    Average();
    void reset();
    void append(const double sample);
    double get() const;
};
