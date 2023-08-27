// Copyright (c) 2013-2023 LG Electronics, Inc.
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

#ifndef KEYSYMHELPER_H
#define KEYSYMHELPER_H

#include <xkbcommon/xkbcommon.h>
#include <qweboskeyextension.h>
#include <QKeyEvent>

struct XkbQtKey
{
    xkb_keysym_t xkbkey;
    int qtkey;
};

static const struct XkbQtKey g_XkbQtKeyMap[] = {
    { XKB_KEY_Shift_L,     Qt::Key_Shift },
    { XKB_KEY_Control_L,   Qt::Key_Control },
    { XKB_KEY_Super_L,     Qt::Key_Super_L },
    { XKB_KEY_Alt_L,       Qt::Key_Alt },

    { XKB_KEY_Shift_R,     Qt::Key_Shift },
    { XKB_KEY_Control_R,   Qt::Key_Control },
    { XKB_KEY_Super_R,     Qt::Key_Super_R },
    { XKB_KEY_Alt_R,       Qt::Key_Alt },

    { XKB_KEY_Menu,        Qt::Key_Menu },
    { XKB_KEY_Escape,      Qt::Key_Escape },

    { XKB_KEY_BackSpace,   Qt::Key_Backspace },
    { XKB_KEY_Return,      Qt::Key_Return },
    { XKB_KEY_Tab,         Qt::Key_Tab },
    { XKB_KEY_Caps_Lock,   Qt::Key_CapsLock },

    { XKB_KEY_F1,          Qt::Key_F1 },
    { XKB_KEY_F2,          Qt::Key_F2 },
    { XKB_KEY_F3,          Qt::Key_F3 },
    { XKB_KEY_F4,          Qt::Key_F4 },
    { XKB_KEY_F5,          Qt::Key_F5 },
    { XKB_KEY_F6,          Qt::Key_F6 },
    { XKB_KEY_F7,          Qt::Key_F7 },
    { XKB_KEY_F8,          Qt::Key_F8 },
    { XKB_KEY_F9,          Qt::Key_F9 },
    { XKB_KEY_F10,         Qt::Key_F10 },
    { XKB_KEY_F11,         Qt::Key_F11 },
    { XKB_KEY_F12,         Qt::Key_F12 },

    { XKB_KEY_Print,       Qt::Key_Print },
    { XKB_KEY_Pause,       Qt::Key_Pause },
    { XKB_KEY_Scroll_Lock, Qt::Key_ScrollLock },

    { XKB_KEY_Insert,      Qt::Key_Insert },
    { XKB_KEY_Delete,      Qt::Key_Delete },
    { XKB_KEY_Home,        Qt::Key_Home },
    { XKB_KEY_End,         Qt::Key_End },
    { XKB_KEY_Prior,       Qt::Key_PageUp },
    { XKB_KEY_Next,        Qt::Key_PageDown },

    { XKB_KEY_Up,          Qt::Key_Up },
    { XKB_KEY_Left,        Qt::Key_Left },
    { XKB_KEY_Down,        Qt::Key_Down },
    { XKB_KEY_Right,       Qt::Key_Right },

    { XKB_KEY_Num_Lock,    Qt::Key_NumLock },

    { XKB_KEY_XF86Back,    Qt::Key_webOS_Back },
};

static const struct XkbQtKey g_XkbQtKeypadMap[] = {
    { XKB_KEY_KP_Divide,   Qt::Key_Slash },
    { XKB_KEY_KP_Multiply, Qt::Key_Asterisk },
    { XKB_KEY_KP_Subtract, Qt::Key_Minus },
    { XKB_KEY_KP_Add,      Qt::Key_Plus },

    { XKB_KEY_KP_Home,     Qt::Key_Home },
    { XKB_KEY_KP_Up,       Qt::Key_Up },
    { XKB_KEY_KP_Prior,    Qt::Key_PageUp },
    { XKB_KEY_KP_Left,     Qt::Key_Left },
    { XKB_KEY_KP_Begin,    Qt::Key_Clear },
    { XKB_KEY_KP_Right,    Qt::Key_Right },
    { XKB_KEY_KP_End,      Qt::Key_End },
    { XKB_KEY_KP_Down,     Qt::Key_Down },
    { XKB_KEY_KP_Next,     Qt::Key_PageDown },

    { XKB_KEY_KP_Insert,   Qt::Key_Insert },
    { XKB_KEY_KP_Delete,   Qt::Key_Delete },
    { XKB_KEY_KP_Enter,    Qt::Key_Enter },
    { XKB_KEY_KP_Decimal,  Qt::Key_Period },

    { XKB_KEY_KP_0,        Qt::Key_0 },
    { XKB_KEY_KP_1,        Qt::Key_1 },
    { XKB_KEY_KP_2,        Qt::Key_2 },
    { XKB_KEY_KP_3,        Qt::Key_3 },
    { XKB_KEY_KP_4,        Qt::Key_4 },
    { XKB_KEY_KP_5,        Qt::Key_5 },
    { XKB_KEY_KP_6,        Qt::Key_6 },
    { XKB_KEY_KP_7,        Qt::Key_7 },
    { XKB_KEY_KP_8,        Qt::Key_8 },
    { XKB_KEY_KP_9,        Qt::Key_9 },
};

static const struct XkbQtKey g_XkbQtMediaMap[] = {
    { XKB_KEY_Cancel,      Qt::Key_MediaStop }, // keymap for NRCU Stop button

    { XKB_KEY_XF86AudioPlay, Qt::Key_MediaPlay },
    { XKB_KEY_XF86AudioStop, Qt::Key_MediaStop },
    { XKB_KEY_XF86AudioPrev, Qt::Key_MediaPrevious },
    { XKB_KEY_XF86AudioNext, Qt::Key_MediaNext },

    { XKB_KEY_XF86AudioRecord, Qt::Key_MediaRecord },
    { XKB_KEY_XF86AudioRewind, Qt::Key_AudioRewind },
    { XKB_KEY_XF86AudioForward, Qt::Key_AudioForward },
};


static Qt::Key xkbKeyToQtKey(xkb_keysym_t xkbkey)
{
    unsigned i;
    for (i = 0; i < sizeof(g_XkbQtKeyMap) / sizeof(XkbQtKey); i++) {
        if (xkbkey == g_XkbQtKeyMap[i].xkbkey)
            return (Qt::Key) g_XkbQtKeyMap[i].qtkey;
    }
    for (i = 0; i < sizeof(g_XkbQtKeypadMap) / sizeof(XkbQtKey); i++) {
        if (xkbkey == g_XkbQtKeypadMap[i].xkbkey)
            return (Qt::Key) g_XkbQtKeypadMap[i].qtkey;
    }
    for (i = 0; i < sizeof(g_XkbQtMediaMap) / sizeof(XkbQtKey); i++) {
        if (xkbkey == g_XkbQtMediaMap[i].xkbkey)
            return (Qt::Key) g_XkbQtMediaMap[i].qtkey;
    }
    return Qt::Key_unknown;
}

static bool isKeypadKey(xkb_keysym_t xkbkey)
{
    unsigned i;
    for (i = 0; i < sizeof(g_XkbQtKeypadMap) / sizeof(XkbQtKey); i++) {
        if (xkbkey == g_XkbQtKeypadMap[i].xkbkey)
            return true;
    }
    return false;
}

struct XkbQtModifier
{
    const char *xkbName;
    Qt::KeyboardModifier qtModifier;
};

static const struct XkbQtModifier g_xkbQtModifierMap[] = {
    { XKB_MOD_NAME_SHIFT, Qt::ShiftModifier },
    { XKB_MOD_NAME_CTRL, Qt::ControlModifier },
    { XKB_MOD_NAME_ALT, Qt::AltModifier },
};

static Qt::KeyboardModifier getQtModifierByXkbName(const char *name)
{
    for (unsigned i = 0; i < sizeof(g_xkbQtModifierMap) / sizeof(XkbQtModifier); i++) {
        if (0 == strcmp(name, g_xkbQtModifierMap[i].xkbName))
            return g_xkbQtModifierMap[i].qtModifier;
    }
    return Qt::NoModifier;
}

class XkbQtModifiersMap {
public:
    XkbQtModifiersMap() {}

    void applyWaylandModifiersMap(struct wl_array *map)
    {
        m_map.clear();

        int index = 0;
        char *p = (char *)map->data;
        while ((const char *)p < (const char *)(map->data + map->size)) {
            Qt::KeyboardModifier qtModifier = getQtModifierByXkbName(p);
            if (Qt::NoModifier != qtModifier)
                m_map.insert(index, qtModifier);

            index++;
            p += strlen(p) + 1;
        }
    }

    Qt::KeyboardModifiers convertNativeModifiersToQt(uint32_t nativeModifiers)
    {
        Qt::KeyboardModifiers qtModifiers = Qt::NoModifier;
        for (int index : m_map.keys()) {
            if (nativeModifiers & (1 << index))
                qtModifiers |= m_map[index];
        }
        return qtModifiers;
    }

private:
    QMap<int, Qt::KeyboardModifier> m_map;
};

#endif // KEYSYMHELPER_H
