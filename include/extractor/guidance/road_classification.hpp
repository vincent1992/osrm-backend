#ifndef OSRM_EXTRACTOR_CLASSIFICATION_DATA_HPP_
#define OSRM_EXTRACTOR_CLASSIFICATION_DATA_HPP_

#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_map>

#include <osmium/osm.hpp>

namespace osrm
{
namespace extractor
{
namespace guidance
{

namespace RoadClass
{
typedef std::uint8_t Enum;
// Top priority Road
const constexpr Enum MOTORWAY = 0;
// Second highest priority
const constexpr Enum TRUNK = 2;
// Main roads
const constexpr Enum PRIMARY = 4;
const constexpr Enum SECONDARY = 6;
const constexpr Enum TERTIARY = 8;
// Residential Categories
const constexpr Enum MAIN_RESIDENTIAL = 10;
const constexpr Enum SIDE_RESIDENTIAL = 11;
// Link Category
const constexpr Enum LINK_ROAD = 14;
// Bike Accessible
const constexpr Enum BIKE_PATH = 16;
// Walk Accessible
const constexpr Enum FOOT_PATH = 18;
// Link types are usually not considered in forks, unless amongst each other. 
// a road simply offered for connectivity. Will be ignored in forks/other decisions. Always
// considered non-obvious to continue on
const constexpr Enum CONNECTIVITY = 31;
} // namespace Road Class

class RoadClassification
{
    // a class that behaves like a motorway (separated directions)
    std::uint32_t motorway_class : 1;
    // all types of link classes
    std::uint32_t link_class : 1;
    // a low priority class is a pure connectivity way. It can be ignored in multiple decisions
    // (e.g. fork on a primary vs service will not happen)
    std::uint32_t may_be_ignored : 1;
    // the road priority is used as an indicator for forks. If the roads are of similar priority
    // (difference <=1), we can see the road as a fork. Else one of the road classes is seen as
    // obvious choice
    RoadClass::Enum road_class : 5;

  public:
    // default construction
    RoadClassification() : motorway_class(0), link_class(0), may_be_ignored(1), road_class(RoadClass::CONNECTIVITY) {}

    RoadClassification(bool motorway_class, bool link_class, bool may_be_ignored, RoadClass::Enum road_class)
        : motorway_class(motorway_class), link_class(link_class), may_be_ignored(may_be_ignored),
          road_class(road_class)
    {
    }

    inline bool isMotorwayClass() const { return (0 != motorway_class) && (0 == link_class); }
    inline void setMotorwayFlag(const bool new_value) { motorway_class = new_value; }

    inline bool isRampClass() const { return (0 != motorway_class) && (0 != link_class); }

    inline bool isLinkClass() const { return (0 != link_class); }
    inline void setLinkClass(const bool new_value) { link_class = new_value; }

    inline bool isLowPriorityRoadClass() const { return (0 != may_be_ignored); }
    inline void setLowPriorityFlag(const bool new_value) { may_be_ignored = new_value; }

    inline std::uint32_t getPriority() const { return static_cast<std::uint32_t>(road_class); }

    inline RoadClass::Enum getClass() const { return road_class; }
    inline void setClass(const RoadClass::Enum new_value) { road_class = new_value; }

    inline bool operator==(const RoadClassification &other) const
    {
        return motorway_class == other.motorway_class && link_class == other.link_class &&
               may_be_ignored == other.may_be_ignored && road_class == other.road_class;
    }

    inline std::string toString() const
    {
        return std::string() + (motorway_class ? "motorway" : "normal") +
               (link_class ? "_link" : "") + (may_be_ignored ? " ignorable " : " important ") +
               std::to_string(road_class);
    }
};

inline bool canBeSeenAsFork(const RoadClassification first, const RoadClassification second)
{
    return std::abs(static_cast<int>(first.getPriority()) -
                    static_cast<int>(second.getPriority())) <= 1;
}
} // namespace guidance
} // namespace extractor
} // namespace osrm

#endif // OSRM_EXTRACTOR_CLASSIFICATION_DATA_HPP_
