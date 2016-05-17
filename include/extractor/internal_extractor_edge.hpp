#ifndef INTERNAL_EXTRACTOR_EDGE_HPP
#define INTERNAL_EXTRACTOR_EDGE_HPP

#include "extractor/guidance/classification_data.hpp"
#include "extractor/node_based_edge.hpp"
#include "extractor/travel_mode.hpp"
#include "osrm/coordinate.hpp"
#include "util/strong_typedef.hpp"
#include "util/typedefs.hpp"

#include <boost/assert.hpp>
#include <utility>
#include <variant/variant.hpp>

namespace osrm
{
namespace extractor
{

namespace detail
{
// these are used for duration based mode of transportations like ferries
OSRM_STRONG_TYPEDEF(double, ValueByEdge);
OSRM_STRONG_TYPEDEF(double, ValueByMeter);
using ByEdgeOrByMeterValue = mapbox::util::variant<detail::ValueByEdge, detail::ValueByMeter>;

struct ToValueByEdge
{
    ToValueByEdge(double distance_) : distance(distance_) {}

    ValueByEdge operator()(const ValueByMeter by_meter) const
    {
        return ValueByEdge(distance / static_cast<double>(by_meter));
    }

    ValueByEdge operator()(const ValueByEdge by_edge) const { return by_edge; }

    double distance;
};
}

struct InternalExtractorEdge
{
    using WeightData = detail::ByEdgeOrByMeterValue;
    using DurationData = detail::ByEdgeOrByMeterValue;

    // data that will be written to disk
    NodeBasedEdgeWithOSM result;
    // intermediate edge weight
    WeightData weight_data;
    // intermediate edge duration
    DurationData duration_data;
    // coordinate of the source node
    util::Coordinate source_coordinate;

    InternalExtractorEdge() = default;

    // necessary static util functions for stxxl's sorting
    static InternalExtractorEdge min_osm_value()
    {
        return InternalExtractorEdge{NodeBasedEdgeWithOSM{MIN_OSM_NODEID,
                                                          MIN_OSM_NODEID,
                                                          0,
                                                          0,
                                                          false,
                                                          false,
                                                          false,
                                                          false,
                                                          false,
                                                          TRAVEL_MODE_INACCESSIBLE,
                                                          false,
                                                          guidance::RoadClassificationData()},
                                     detail::ValueByMeter{0.0},
                                     detail::ValueByMeter{0.0},
                                     {}};
    }
    static InternalExtractorEdge max_osm_value()
    {
        return InternalExtractorEdge{NodeBasedEdgeWithOSM{MAX_OSM_NODEID,
                                                          MAX_OSM_NODEID,
                                                          0,
                                                          0,
                                                          false,
                                                          false,
                                                          false,
                                                          false,
                                                          false,
                                                          TRAVEL_MODE_INACCESSIBLE,
                                                          false,
                                                          guidance::RoadClassificationData()},
                                     detail::ValueByMeter{0.0},
                                     detail::ValueByMeter{0.0},
                                     {}};
    }

    static InternalExtractorEdge min_internal_value()
    {
        auto v = min_osm_value();
        v.result.source = 0;
        v.result.target = 0;
        return v;
    }
    static InternalExtractorEdge max_internal_value()
    {
        auto v = max_osm_value();
        v.result.source = std::numeric_limits<NodeID>::max();
        v.result.target = std::numeric_limits<NodeID>::max();
        return v;
    }
};

struct CmpEdgeByInternalStartThenInternalTargetID
{
    using value_type = InternalExtractorEdge;
    bool operator()(const InternalExtractorEdge &lhs, const InternalExtractorEdge &rhs) const
    {
        return (lhs.result.source < rhs.result.source) ||
               ((lhs.result.source == rhs.result.source) &&
                (lhs.result.target < rhs.result.target));
    }

    value_type max_value() { return InternalExtractorEdge::max_internal_value(); }
    value_type min_value() { return InternalExtractorEdge::min_internal_value(); }
};

struct CmpEdgeByOSMStartID
{
    using value_type = InternalExtractorEdge;
    bool operator()(const InternalExtractorEdge &lhs, const InternalExtractorEdge &rhs) const
    {
        return lhs.result.osm_source_id < rhs.result.osm_source_id;
    }

    value_type max_value() { return InternalExtractorEdge::max_osm_value(); }
    value_type min_value() { return InternalExtractorEdge::min_osm_value(); }
};

struct CmpEdgeByOSMTargetID
{
    using value_type = InternalExtractorEdge;
    bool operator()(const InternalExtractorEdge &lhs, const InternalExtractorEdge &rhs) const
    {
        return lhs.result.osm_target_id < rhs.result.osm_target_id;
    }

    value_type max_value() { return InternalExtractorEdge::max_osm_value(); }
    value_type min_value() { return InternalExtractorEdge::min_osm_value(); }
};
}
}

#endif // INTERNAL_EXTRACTOR_EDGE_HPP
