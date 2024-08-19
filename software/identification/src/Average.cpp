#include "Average.h"

Average::Average() {

}

void Average::reset() {
    number = 0;
}

void Average::append(const double sample) {
    const double w1 = static_cast<double>(number)/static_cast<double>(number + 1);
    const double w2 = 1./static_cast<double>(number + 1);

    avg = w1*avg + w2*sample;
    number++;
}

double Average::get() const {
    return avg;
}
