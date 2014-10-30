/** @file
    @brief Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <ogvr/PluginKit/Connection.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
ConnectionPtr Connection::createLocalConnection() {
    /// @todo implement
    return ConnectionPtr();
}
Connection::~Connection() {}
Connection::Connection() {}
} // end of namespace ogvr