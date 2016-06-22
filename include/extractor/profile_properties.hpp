#ifndef PROFILE_PROPERTIES_HPP
#define PROFILE_PROPERTIES_HPP

#include <boost/assert.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace osrm
{
namespace extractor
{

struct ProfileProperties
{
    static const constexpr std::size_t MAX_WEIGHT_NAME_SIZE = 256;

    ProfileProperties()
        : traffic_signal_penalty(0), u_turn_penalty(0), continue_straight_at_waypoint(true),
          use_turn_restrictions(false)
    {
        std::fill(weight_name, weight_name + MAX_WEIGHT_NAME_SIZE, 0);
        const std::string default_name = "duration";
        BOOST_ASSERT(default_name.size() < MAX_WEIGHT_NAME_SIZE - 1);
        std::copy(default_name.begin(), default_name.end(), weight_name);
        BOOST_ASSERT(weight_name[MAX_WEIGHT_NAME_SIZE - 1] == '\0');
    }

    double GetUturnPenalty() const { return u_turn_penalty / 10.; }

    void SetUturnPenalty(const double u_turn_penalty_)
    {
        u_turn_penalty = boost::numeric_cast<int>(u_turn_penalty_ * 10.);
    }

    double GetTrafficSignalPenalty() const { return traffic_signal_penalty / 10.; }

    void SetTrafficSignalPenalty(const double traffic_signal_penalty_)
    {
        traffic_signal_penalty = boost::numeric_cast<int>(traffic_signal_penalty_ * 10.);
    }

    void SetWeightName(const std::string &name)
    {
        auto length = std::min<std::size_t>(name.length(), MAX_WEIGHT_NAME_SIZE - 1);
        std::copy(name.c_str(), name.c_str() + length, weight_name);
        // Make sure this is always zero terminated
        BOOST_ASSERT(weight_name[MAX_WEIGHT_NAME_SIZE - 1] == '\0');
    }

    std::string GetWeightName() const
    {
        // Make sure this is always zero terminated
        BOOST_ASSERT(weight_name[MAX_WEIGHT_NAME_SIZE - 1] == '\0');
        return std::string(weight_name);
    }

    //! penalty to cross a traffic light in deci-seconds
    std::int32_t traffic_signal_penalty;
    //! penalty to do a uturn in deci-seconds
    std::int32_t u_turn_penalty;
    //! depending on the profile, force the routing to always continue in the same direction
    bool continue_straight_at_waypoint;
    //! flag used for restriction parser (e.g. used for the walk profile)
    bool use_turn_restrictions;
    //! stores the name of the weight (e.g. 'duration', 'distance', 'safety')
    char weight_name[MAX_WEIGHT_NAME_SIZE];
};
}
}

#endif
