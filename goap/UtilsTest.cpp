#include "goap/BasePredicate.h"
#include "goap/GoapUtils.h"
#include "goap/IPredicate.h"
#include "goap/planners/DirectGoapPlanner.h"
#include "gtest/gtest.h"

TEST(Utils_Substract, When_FirstVectorHasRepeatedPredicates_Substract_ResultEmpty)
{
    std::vector<std::shared_ptr<NAI::Goap::IPredicate>> v1 = {
        std::make_shared<NAI::Goap::BasePredicate>(1, "A"),
        std::make_shared<NAI::Goap::BasePredicate>(2, "B"),
        std::make_shared<NAI::Goap::BasePredicate>(1, "A"),
        std::make_shared<NAI::Goap::BasePredicate>(2, "B")
    };

    std::vector<std::shared_ptr<NAI::Goap::IPredicate>> v2 = {
        std::make_shared<NAI::Goap::BasePredicate>(1, "A"),
        std::make_shared<NAI::Goap::BasePredicate>(2, "B")
    };

    const auto result = NAI::Goap::Utils::Substract(v1, v2);
    ASSERT_EQ(result.size(), 0);
}