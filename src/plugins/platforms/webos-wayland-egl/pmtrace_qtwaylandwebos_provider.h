// Copyright (c) 2016-2022 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER qtwayland_webos

#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "./pmtrace_qtwaylandwebos_provider.h"

#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus */


#if !defined(_PMTRACE_QTWAYLANDWEBOS_PROVIDER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _PMTRACE_QTWAYLANDWEBOS_PROVIDER_H

#include <lttng/tracepoint.h>

/* "message" tracepoint should be used for single event trace points */
TRACEPOINT_EVENT(
    qtwayland_webos,
    message,
    TP_ARGS(char*, text),
    TP_FIELDS(ctf_string(scope, text)))
/* "touchcoordinate" tracepoint is for printing touch event trace points */
TRACEPOINT_EVENT(
    qtwayland_webos,
    touchcoordinate,
    TP_ARGS(char *, item, float, x, float, y),
    TP_FIELDS(ctf_string(item, item)
              ctf_float(float, xcoord, x)
              ctf_float(float, ycoord, y)))
/* "item" tracepoint should be used for name/value pairs */
TRACEPOINT_EVENT(
    qtwayland_webos,
    item,
    TP_ARGS(char*, text1, char*, text2),
    TP_FIELDS(ctf_string(name, text1) ctf_string(value, text2)))
/* "position" tracepoint records a message and two integer parameters */
TRACEPOINT_EVENT(
    qtwayland_webos,
    position,
    TP_ARGS(char*, text, int, x, int, y),
    TP_FIELDS(ctf_string(scope, text)
              ctf_integer(int, xPos, x)
              ctf_integer(int, yPos, y)))
/* "before"/"after" tracepoint should be used for measuring the
   duration of something that doesn't correspond with a function call or scope */
TRACEPOINT_EVENT(
    qtwayland_webos,
    before,
    TP_ARGS(char*, text),
    TP_FIELDS(ctf_string(scope, text)))
TRACEPOINT_EVENT(
    qtwayland_webos,
    after,
    TP_ARGS(char*, text),
    TP_FIELDS(ctf_string(scope, text)))
/* "scope_entry"/"scope_exit" tracepoints should be used only by
   PmtraceTraceScope class to measure the duration of a scope within
   a function in C++ code. In C code these may be used directly for
   the same purpose, just make sure you trace any early exit from the
   scope such as break statements or gotos.  */
TRACEPOINT_EVENT(
    qtwayland_webos,
    scope_entry,
    TP_ARGS(char*, text),
    TP_FIELDS(ctf_string(scope, text)))
TRACEPOINT_EVENT(
    qtwayland_webos,
    scope_exit,
    TP_ARGS(char*, text),
    TP_FIELDS(ctf_string(scope, text)))
/* "function_entry"/"function_exit" tracepoints should be used only by
   PmtraceTraceFunction class to measure the duration of a function
   in C++ code. In C code it may be used directly for the same
   purpose, just make sure you capture any early exit from the
   function such as return statements. */
TRACEPOINT_EVENT(
    qtwayland_webos,
    function_entry,
    TP_ARGS(const char*, text),
    TP_FIELDS(ctf_string(scope, text)))
TRACEPOINT_EVENT(
    qtwayland_webos,
    function_exit,
    TP_ARGS(const char*, text),
    TP_FIELDS(ctf_string(scope, text)))

#endif /* _PMTRACE_QTWAYLANDWEBOS_PROVIDER_H */

#ifdef __cplusplus
}
#endif /*__cplusplus */
