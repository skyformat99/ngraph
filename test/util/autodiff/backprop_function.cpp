// ----------------------------------------------------------------------------
// Copyright 2017 Nervana Systems Inc.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// ----------------------------------------------------------------------------

#include <cassert>
#include <list>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "backprop_function.hpp"
#include "ngraph/autodiff/adjoints.hpp"
#include "ngraph/function.hpp"
#include "ngraph/node.hpp"
#include "ngraph/ops/add.hpp"
#include "ngraph/ops/broadcast.hpp"
#include "ngraph/ops/constant.hpp"
#include "ngraph/ops/convert.hpp"
#include "ngraph/ops/tuple.hpp"
#include "ngraph/types/type.hpp"

using namespace ngraph;

std::shared_ptr<Function> autodiff::backprop_function(const std::shared_ptr<Function>& f)
{
    auto Y = f->get_result();
    auto Xs = f->get_parameters();
    auto C = std::make_shared<op::Parameter>(Y->get_value_type());
    std::vector<std::shared_ptr<Node>> dYdXs(Xs.size());
    transform(Xs.begin(), Xs.end(), dYdXs.begin(), [C, Y](const std::shared_ptr<Node>& X) {
        return Y->backprop_node(X, C);
    });
    auto result = std::make_shared<op::Tuple>(dYdXs);
    std::vector<std::shared_ptr<op::Parameter>> params(Xs);
    params.push_back(C);
    return std::make_shared<Function>(result, result->get_value_type(), params);
}