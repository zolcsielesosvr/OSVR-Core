/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

// Internal Includes
#include "LedIdentifier.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace vbtracker {

    class OsvrHdkLedIdentifier : public LedIdentifier {
      public:
        /// @brief Give it a list of patterns to use.  There is a string for
        /// each LED, and each is encoded with '*' meaning that the LED is
        /// bright and '.' that it is dim at this point in time. All patterns
        /// must have the same length.
        OsvrHdkLedIdentifier(const PatternStringList &PATTERNS);

        ~OsvrHdkLedIdentifier() override;

        /// @brief Determine an ID based on a list of brightnesses
        /// This truncates the passed-in list to only as many elements
        /// as are in the pattern list, to keep it from growing too
        /// large and wasting time and space.
        ZeroBasedBeaconId getId(ZeroBasedBeaconId currentId,
                                BrightnessList &brightnesses, bool &lastBright,
                                bool blobsKeepId) override;

        void nextFrame() override;

      private:
        void rotatePatterns(uint8_t count = 1);
        bool isInSync() {return fail_count < 3;}
        ZeroBasedBeaconId detectPattern(ZeroBasedBeaconId currentId, uint16_t bits);

        static const uint8_t max_fail_count = std::numeric_limits<uint8_t>::max();

        std::vector<uint16_t> d_patterns; //< Patterns by index
        uint8_t d_length;        //< Length of all patterns
        uint8_t detected_patterns = 0;
        uint8_t fail_count = max_fail_count;
        std::array<uint8_t, 16> match_at_rotation;
    };

} // End namespace vbtracker
} // End namespace osvr
