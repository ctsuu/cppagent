//
// Copyright Copyright 2009-2019, AMT – The Association For Manufacturing Technology (“AMT”)
// All rights reserved.
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//

#pragma once

#include "component_configuration.hpp"
#include "globals.hpp"

#include <utility>
#include <vector>

namespace mtconnect
{
  struct CoordinateSystem
  {
    CoordinateSystem(const CoordinateSystem &s) = default;
    CoordinateSystem() = default;

    // Attributes
    std::string m_id;
    std::string m_type;
    std::string m_name;
    std::string m_nativeName;
    std::string m_parentIdRef;

    // Elements
    // Transformation
    std::string m_rotation;
    std::string m_translation;

    // Origin
    std::string m_origin;
  };

  class CoordinateSystems : public ComponentConfiguration
  {
   public:
    CoordinateSystems() = default;
    virtual ~CoordinateSystems() = default;

    const std::list<std::unique_ptr<CoordinateSystem>> &getCoordinateSystems() const
    {
      return m_coordinateSystems;
    }
    void addCoordinateSystem(CoordinateSystem *s)
    {
      m_coordinateSystems.emplace_back(std::move(s));
    }
    void addCoordinateSystem(std::unique_ptr<CoordinateSystem> &s)
    {
      m_coordinateSystems.emplace_back(std::move(s));
    }

   protected:
    std::list<std::unique_ptr<CoordinateSystem>> m_coordinateSystems;
  };
}  // namespace mtconnect
