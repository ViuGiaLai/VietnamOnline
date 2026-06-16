#include "vno/VNO.hpp"
#include <cassert>
#include <iostream>

int main() {
    using namespace vno::validation;
    using namespace vno::constants;

    assert(isWithinCheckInRadius(50.0, POI_RADIUS_T1));
    assert(!isWithinCheckInRadius(150.0, POI_RADIUS_T1));
    assert(canAfford(100000, 50000));
    assert(!canAfford(10000, 50000));
    assert(canAcceptQuest(2));
    assert(!canAcceptQuest(3));
    assert(isPhotoPriceValid(5000));
    assert(!isPhotoPriceValid(100));

    std::cout << "All validation tests passed.\n";
    return 0;
}
