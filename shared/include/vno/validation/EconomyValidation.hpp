#pragma once
#include "vno/constants/EconomyConstants.hpp"

namespace vno::validation {

inline bool isP2PWithinDailyCap(int64_t spent_today, int64_t amount) {
    return (spent_today + amount) <= constants::DAILY_P2P_CAP;
}

inline bool isPhotoPriceValid(int64_t price) {
    return price >= constants::PHOTO_PRICE_FLOOR && price <= constants::PHOTO_PRICE_CAP;
}

inline bool isTourPriceValid(int64_t price) {
    return price >= constants::TOUR_PRICE_FLOOR && price <= constants::TOUR_PRICE_CAP;
}

} // namespace vno::validation
