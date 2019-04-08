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

#include "globals.hpp"
#include "printer.hpp"
#include "cutting_tool.hpp"

namespace mtconnect {
  class JsonPrinter : public Printer
  {
  public:
    JsonPrinter(const std::string version = "", bool pretty = false)
    : Printer(pretty), m_version(version) {}
    virtual ~JsonPrinter() {}
    
    virtual std::string printError(
                                   const unsigned int instanceId,
                                   const unsigned int bufferSize,
                                   const uint64_t nextSeq,
                                   const std::string &errorCode,
                                   const std::string &errorText
                                   )  const override;
    
    virtual std::string printProbe(
                                   const unsigned int instanceId,
                                   const unsigned int bufferSize,
                                   const uint64_t nextSeq,
                                   const unsigned int assetBufferSize,
                                   const unsigned int assetCount,
                                   const std::vector<Device *> &devices,
                                   const std::map<std::string, int> *count = nullptr)  const override;
    
    virtual std::string printSample(
                                    const unsigned int instanceId,
                                    const unsigned int bufferSize,
                                    const uint64_t nextSeq,
                                    const uint64_t firstSeq,
                                    const uint64_t lastSeq,
                                    ObservationPtrArray &results
                                    ) const override;
    
    virtual std::string printAssets(
                                    const unsigned int anInstanceId,
                                    const unsigned int bufferSize,
                                    const unsigned int assetCount,
                                    std::vector<AssetPtr> const &assets) const override;
    
    virtual std::string printCuttingTool(CuttingToolPtr const tool) const override;
    
    virtual std::string mimeType() const override {
      return "application/mtconnect+json";
    }
    
  protected:
    const std::string &hostname() const;
    
    std::string m_version;
    std::string m_hostname;
  };
}

