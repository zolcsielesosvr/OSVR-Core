/** @file
    @brief Implementation

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

// Internal Includes
#include "LED.h"
#include "HDKLedIdentifier.h"
#include "IdentifierHelpers.h"

// Library/third-party includes
// - none

// Standard includes
#include <stdexcept>

namespace osvr {
namespace vbtracker {
    static const auto VALIDCHARS = "*.";
    OsvrHdkLedIdentifier::~OsvrHdkLedIdentifier() {}
    // Convert from string encoding representations into lists
    // of boolean values for use in comparison.
    OsvrHdkLedIdentifier::OsvrHdkLedIdentifier(
        const PatternStringList &PATTERNS) {
        // Ensure that we have at least one entry in our list and
        // find the length of the first valid entry.
        d_length = 0;
        if (PATTERNS.empty()) {
            return;
        }

        for (auto &pat : PATTERNS) {
            if (!pat.empty() && pat.find_first_not_of(VALIDCHARS) == pat.npos) {
                // All characters in this pattern are valid - we can consider
                // this to be a valid pattern and thus establish the size.
                d_length = pat.length();
                break;
            }
        }

        if (0 == d_length) {
            // If we still have 0 as the pattern length, return.
            return;
        }

        // Decode each string into a new list of boolean values, making
        // sure each have the correct length.

        for (auto &pat : PATTERNS) {
            uint16_t code = 0;
            if (!pat.empty() && pat.find_first_not_of(VALIDCHARS) == pat.npos) {    // Use 0 for an intentionally disabled beacon/pattern.
                // Make sure the pattern is the correct length.
                if (pat.size() != d_length) {
                    throw std::runtime_error("Got a pattern of incorrect length!");
                }

                for (int i = 0; i < pat.size(); ++i) {
                    code <<= 1;
                    if (pat[i] == '*')
                        code |= 1;
                }
            }

            d_patterns.emplace_back(code);
        }
        match_at_rotation.fill(0);
    }

    void OsvrHdkLedIdentifier::rotatePatterns(uint8_t count)
    {
        for (auto &pat: d_patterns)
            pat = ((pat << count) & ((1 << d_length) - 1)) | (pat >> (d_length - count));
    }

    void OsvrHdkLedIdentifier::nextFrame()
    {
        int d = detected_patterns;
        uint8_t rotation = 1;
        if (!isInSync()) {
            detected_patterns = 0;
            for (int i = 0; i < match_at_rotation.size(); i++) {
                if (match_at_rotation[i] > detected_patterns) {
                    detected_patterns = match_at_rotation[i];
                    rotation = i + 1;
                }
            }
        }

        if (detected_patterns >= 3)
            fail_count = 0;
        else if (fail_count < max_fail_count)
            ++fail_count;
        detected_patterns = 0;
        rotatePatterns(rotation);
        if (!isInSync()) {
            match_at_rotation.fill(0);
        }
    }

    int OsvrHdkLedIdentifier::getId(int currentId,
                                    BrightnessList &brightnesses,
                                    bool &lastBright, bool blobsKeepId) {
        // If we don't have at least the required number of frames of data, we
        // don't know anything.
        if (brightnesses.size() < d_length) {
            return Led::SENTINEL_NO_IDENTIFIER_OBJECT_OR_INSUFFICIENT_DATA;
        }

        // We only care about the d_length most-recent levels.
        truncateBrightnessListTo(brightnesses, d_length);

        // Compute the minimum and maximum brightness values.  If
        // they are too close to each other, we have a light rather
        // than an LED.  If not, compute a threshold to separate the
        // 0's and 1's.
        Brightness minVal, maxVal;
        std::tie(minVal, maxVal) = findMinMaxBrightness(brightnesses);
        // Brightness is currently actually keypoint diameter (radius?) in
        // pixels, and it's being under-estimated by OpenCV.
        static const double TODO_MIN_BRIGHTNESS_DIFF = 0.3;
        if (maxVal - minVal <= TODO_MIN_BRIGHTNESS_DIFF) {
            return Led::SENTINEL_INSUFFICIENT_EXTREMA_DIFFERENCE;
        }
        const auto threshold = (minVal + maxVal) / 2;
        // Set the `lastBright` out variable
        lastBright = brightnesses.back() >= threshold;

        if (blobsKeepId && currentId >= 0) {
            ++detected_patterns;
            // Early out if we already have identified this LED.
            return currentId;
        }

        // Get a list of boolean values for 0's and 1's using
        // the threshold computed above.
        uint16_t bits = getBitsUsingThreshold(brightnesses, threshold);

        return detectPattern(currentId, bits);
    }

    int OsvrHdkLedIdentifier::detectPattern(int currentId, uint16_t bits)
    {
        // No pattern recognized and we should have recognized one, so return
        // a low negative.  We've used -2 so return -3.
        int id = Led::SENTINEL_NO_PATTERN_RECOGNIZED_DESPITE_SUFFICIENT_DATA;
        if (!bits)
            return id;

        // Search through the available patterns to see if the passed-in
        // pattern matches any of them.  If so, return that pattern.
        if (isInSync()) {
            auto it = std::find(d_patterns.begin(), d_patterns.end(), bits);
            if (it != d_patterns.end()) {
                ++detected_patterns;
                id = it - d_patterns.begin();
            }
        } else {
            for (int i = 0; i < d_length; i++) {
                auto it = std::find(d_patterns.begin(), d_patterns.end(), bits);
                if (it != d_patterns.end()) {
                    ++detected_patterns;
                    match_at_rotation[i]++;
                    id = it - d_patterns.begin();
                    break;
                }
                bits = (bits >> 1) | ((bits << (d_length - 1)) & ((1 << d_length) - 1));
            }
        }

        return id;
    }

} // End namespace vbtracker
} // End namespace osvr
