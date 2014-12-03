/** @file
    @brief Header

    Must be c-safe!

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

/*
// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)
*/

#ifndef INCLUDED_ClientCallbackTypesC_h_GUID_4D43A675_C8A4_4BBF_516F_59E6C785E4EF
#define INCLUDED_ClientCallbackTypesC_h_GUID_4D43A675_C8A4_4BBF_516F_59E6C785E4EF

/* Internal Includes */
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/ReturnCodesC.h>
#include <osvr/Util/Pose3C.h>
#include <osvr/Util/TimeValueC.h>
#include <osvr/Util/StdInt.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @addtogroup ClientKit
    @{
*/

/** @brief Report type for a position callback on a tracker interface */
struct OSVR_PositionReport {
    /** @brief Identifies the sensor that the report comes from */
    int32_t sensor;
    /** @brief The position vector */
    struct OSVR_Vec3 xyz;
};

/** @brief C function type for a position callback on a tracker interface */
typedef void (*OSVR_PositionCallback)(void *userdata,
                                      const struct OSVR_TimeValue *timestamp,
                                      const struct OSVR_PositionReport *report);

/** @brief Report type for an orientation callback on a tracker interface */
struct OSVR_OrientationReport {
    /** @brief Identifies the sensor that the report comes from */
    int32_t sensor;
    /** @brief The rotation unit quaternion */
    struct OSVR_Quaternion rotation;
};

/** @brief C function type for an orientation callback on a tracker interface */
typedef void (*OSVR_OrientationCallback)(
    void *userdata, const struct OSVR_TimeValue *timestamp,
    const struct OSVR_OrientationReport *report);

/** @brief Report type for a pose (position and orientation) callback on a
    tracker interface
*/
struct OSVR_PoseReport {
    /** @brief Identifies the sensor that the report comes from */
    int32_t sensor;
    /** @brief The pose structure, containing a position vector and a rotation
        quaternion
    */
    struct OSVR_Pose3 pose;
};

/** @brief C function type for a pose (position and orientation) callback on a
    tracker interface
*/
typedef void (*OSVR_PoseCallback)(void *userdata,
                                  const struct OSVR_TimeValue *timestamp,
                                  const struct OSVR_PoseReport *report);

/** @} */
OSVR_EXTERN_C_END

#endif
