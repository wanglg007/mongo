/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=8 sts=4 et sw=4 tw=99:
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef jit_shared_Architecture_shared_h
#define jit_shared_Architecture_shared_h

namespace js {
namespace jit {

enum class RegTypeName {
    GPR,
    Float32,
    Float64,
    Vector128,
    Any
};

} // namespace jit
} // namespace js

#endif /* jit_shared_Architecture_shared_h */
