// Copyright (c) 2015-2021 LG Electronics, Inc.
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

#include "webosinputdevice_p.h"
#include "webosplatformwindow_p.h"
#include <QtWaylandClient/private/qwaylanddisplay_p.h>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtGui/QPointingDevice>
#else
#include <QtWaylandClient/private/qwaylandtouch_p.h>
#endif
#include <QtGlobal>
#include <QDebug>

#include <QtWaylandClient/private/qwaylanddisplay_p.h>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtWaylandClient/private/qwaylandsurface_p.h>
#endif

#include "qtwaylandwebostracer.h"

#if QT_CONFIG(xkbcommon)
#include <xkbcommon/xkbcommon.h>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#endif
#endif

WebOSInputDevice::WebOSInputDevice(QWaylandDisplay *display, int version, uint32_t id)
    : QWaylandInputDevice(display, version, id)
    , mTouchRegistered(false)
{

}

QWaylandInputDevice::Keyboard *WebOSInputDevice::createKeyboard(QWaylandInputDevice *device)
{
    PMTRACE_FUNCTION;
    return new WebOSKeyboard(device);
}

QWaylandInputDevice::Pointer *WebOSInputDevice::createPointer(QWaylandInputDevice *device)
{
    PMTRACE_FUNCTION;
    return new WebOSPointer(device);
}

QWaylandInputDevice::Touch *WebOSInputDevice::createTouch(QWaylandInputDevice *device)
{
    PMTRACE_FUNCTION;
    return new WebOSTouch(device);
}

void WebOSInputDevice::seat_capabilities(uint32_t caps)
{
    PMTRACE_FUNCTION;
    if (caps & WL_SEAT_CAPABILITY_TOUCH && !mTouch) {
        // This substitues creation of QTouchDevice in QtWayland
        // Then, we will register it when real event comes up
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        // Create new pointing device (name, id, type, pointerType, maxPoints, uniquieId)
        mTouchDevice = new QPointingDevice(QLatin1String("some touchscreen"), 0
                , QInputDevice::DeviceType::TouchScreen, QPointingDevice::PointerType::Finger
                , QInputDevice::Capability::Position, 10, 0);
#else
        mTouchDevice = new QTouchDevice;
        mTouchDevice->setType(QTouchDevice::TouchScreen);
        mTouchDevice->setCapabilities(QTouchDevice::Position);
#endif
    }

    QWaylandInputDevice::seat_capabilities(caps);
}

void WebOSInputDevice::registerTouchDevice()
{
    PMTRACE_FUNCTION;
    if (Q_UNLIKELY(!mTouchRegistered) && Q_LIKELY(mTouchDevice)) {
        mTouchRegistered = true;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QWindowSystemInterface::registerInputDevice(mTouchDevice);
#else
        QWindowSystemInterface::registerTouchDevice(mTouchDevice);
#endif
    }
}

void WebOSInputDevice::setSerial(uint32_t serial)
{
    PMTRACE_FUNCTION;
    mSerial = serial;
}

void WebOSInputDevice::setTime(uint32_t time)
{
    PMTRACE_FUNCTION;
    mTime = time;
}

WebOSInputDevice::WebOSKeyboard::WebOSKeyboard(QWaylandInputDevice *device)
    : Keyboard(device)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#if QT_CONFIG(xkbcommon)
    , mKeymapFd(-1)
    , mKeymapSize(0)
    , mPendingKeymap(false)
#endif
#endif
{

}

#if QT_CONFIG(xkbcommon)

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
static const uint32_t KeyTbl[] = {
    XKB_KEY_Escape,                 Qt::Key_Escape,
    XKB_KEY_Tab,                    Qt::Key_Tab,
    XKB_KEY_ISO_Left_Tab,           Qt::Key_Backtab,
    XKB_KEY_BackSpace,              Qt::Key_Backspace,
    XKB_KEY_Return,                 Qt::Key_Return,
    XKB_KEY_Insert,                 Qt::Key_Insert,
    XKB_KEY_Delete,                 Qt::Key_Delete,
    XKB_KEY_Clear,                  Qt::Key_Delete,
    XKB_KEY_Pause,                  Qt::Key_Pause,
    XKB_KEY_Print,                  Qt::Key_Print,

    XKB_KEY_Home,                   Qt::Key_Home,
    XKB_KEY_End,                    Qt::Key_End,
    XKB_KEY_Left,                   Qt::Key_Left,
    XKB_KEY_Up,                     Qt::Key_Up,
    XKB_KEY_Right,                  Qt::Key_Right,
    XKB_KEY_Down,                   Qt::Key_Down,
    XKB_KEY_Prior,                  Qt::Key_PageUp,
    XKB_KEY_Next,                   Qt::Key_PageDown,

    XKB_KEY_Shift_L,                Qt::Key_Shift,
    XKB_KEY_Shift_R,                Qt::Key_Shift,
    XKB_KEY_Shift_Lock,             Qt::Key_Shift,
    XKB_KEY_Control_L,              Qt::Key_Control,
    XKB_KEY_Control_R,              Qt::Key_Control,
    XKB_KEY_Meta_L,                 Qt::Key_Meta,
    XKB_KEY_Meta_R,                 Qt::Key_Meta,
    XKB_KEY_Alt_L,                  Qt::Key_Alt,
    XKB_KEY_Alt_R,                  Qt::Key_Alt,
    XKB_KEY_Caps_Lock,              Qt::Key_CapsLock,
    XKB_KEY_Num_Lock,               Qt::Key_NumLock,
    XKB_KEY_Scroll_Lock,            Qt::Key_ScrollLock,
    XKB_KEY_Super_L,                Qt::Key_Super_L,
    XKB_KEY_Super_R,                Qt::Key_Super_R,
    XKB_KEY_Menu,                   Qt::Key_Menu,
    XKB_KEY_Hyper_L,                Qt::Key_Hyper_L,
    XKB_KEY_Hyper_R,                Qt::Key_Hyper_R,
    XKB_KEY_Help,                   Qt::Key_Help,

    XKB_KEY_KP_Space,               Qt::Key_Space,
    XKB_KEY_KP_Tab,                 Qt::Key_Tab,
    XKB_KEY_KP_Enter,               Qt::Key_Enter,
    XKB_KEY_KP_Home,                Qt::Key_Home,
    XKB_KEY_KP_Left,                Qt::Key_Left,
    XKB_KEY_KP_Up,                  Qt::Key_Up,
    XKB_KEY_KP_Right,               Qt::Key_Right,
    XKB_KEY_KP_Down,                Qt::Key_Down,
    XKB_KEY_KP_Prior,               Qt::Key_PageUp,
    XKB_KEY_KP_Next,                Qt::Key_PageDown,
    XKB_KEY_KP_End,                 Qt::Key_End,
    XKB_KEY_KP_Begin,               Qt::Key_Clear,
    XKB_KEY_KP_Insert,              Qt::Key_Insert,
    XKB_KEY_KP_Delete,              Qt::Key_Delete,
    XKB_KEY_KP_Equal,               Qt::Key_Equal,
    XKB_KEY_KP_Multiply,            Qt::Key_Asterisk,
    XKB_KEY_KP_Add,                 Qt::Key_Plus,
    XKB_KEY_KP_Separator,           Qt::Key_Comma,
    XKB_KEY_KP_Subtract,            Qt::Key_Minus,
    XKB_KEY_KP_Decimal,             Qt::Key_Period,
    XKB_KEY_KP_Divide,              Qt::Key_Slash,

    XKB_KEY_ISO_Level3_Shift,       Qt::Key_AltGr,
    XKB_KEY_Multi_key,              Qt::Key_Multi_key,
    XKB_KEY_Codeinput,              Qt::Key_Codeinput,
    XKB_KEY_SingleCandidate,        Qt::Key_SingleCandidate,
    XKB_KEY_MultipleCandidate,      Qt::Key_MultipleCandidate,
    XKB_KEY_PreviousCandidate,      Qt::Key_PreviousCandidate,

    XKB_KEY_Mode_switch,            Qt::Key_Mode_switch,
    XKB_KEY_script_switch,          Qt::Key_Mode_switch,

    XKB_KEY_XF86Back,               Qt::Key_Back,
    XKB_KEY_XF86Forward,            Qt::Key_Forward,
    XKB_KEY_XF86Stop,               Qt::Key_Stop,
    XKB_KEY_XF86Refresh,            Qt::Key_Refresh,
    XKB_KEY_XF86Favorites,          Qt::Key_Favorites,
    XKB_KEY_XF86AudioMedia,         Qt::Key_LaunchMedia,
    XKB_KEY_XF86OpenURL,            Qt::Key_OpenUrl,
    XKB_KEY_XF86HomePage,           Qt::Key_HomePage,
    XKB_KEY_XF86Search,             Qt::Key_Search,
    XKB_KEY_XF86AudioLowerVolume,   Qt::Key_VolumeDown,
    XKB_KEY_XF86AudioMute,          Qt::Key_VolumeMute,
    XKB_KEY_XF86AudioRaiseVolume,   Qt::Key_VolumeUp,
    XKB_KEY_XF86AudioPlay,          Qt::Key_MediaTogglePlayPause,
    XKB_KEY_XF86AudioStop,          Qt::Key_MediaStop,
    XKB_KEY_XF86AudioPrev,          Qt::Key_MediaPrevious,
    XKB_KEY_XF86AudioNext,          Qt::Key_MediaNext,
    XKB_KEY_XF86AudioRecord,        Qt::Key_MediaRecord,
    XKB_KEY_XF86AudioPause,         Qt::Key_MediaPause,
    XKB_KEY_XF86Mail,               Qt::Key_LaunchMail,
    XKB_KEY_XF86Calculator,         Qt::Key_Calculator,
    XKB_KEY_XF86Memo,               Qt::Key_Memo,
    XKB_KEY_XF86ToDoList,           Qt::Key_ToDoList,
    XKB_KEY_XF86Calendar,           Qt::Key_Calendar,
    XKB_KEY_XF86PowerDown,          Qt::Key_PowerDown,
    XKB_KEY_XF86ContrastAdjust,     Qt::Key_ContrastAdjust,
    XKB_KEY_XF86Standby,            Qt::Key_Standby,
    XKB_KEY_XF86MonBrightnessUp,    Qt::Key_MonBrightnessUp,
    XKB_KEY_XF86MonBrightnessDown,  Qt::Key_MonBrightnessDown,
    XKB_KEY_XF86KbdLightOnOff,      Qt::Key_KeyboardLightOnOff,
    XKB_KEY_XF86KbdBrightnessUp,    Qt::Key_KeyboardBrightnessUp,
    XKB_KEY_XF86KbdBrightnessDown,  Qt::Key_KeyboardBrightnessDown,
    XKB_KEY_XF86PowerOff,           Qt::Key_PowerOff,
    XKB_KEY_XF86WakeUp,             Qt::Key_WakeUp,
    XKB_KEY_XF86Eject,              Qt::Key_Eject,
    XKB_KEY_XF86ScreenSaver,        Qt::Key_ScreenSaver,
    XKB_KEY_XF86WWW,                Qt::Key_WWW,
    XKB_KEY_XF86Sleep,              Qt::Key_Sleep,
    XKB_KEY_XF86LightBulb,          Qt::Key_LightBulb,
    XKB_KEY_XF86Shop,               Qt::Key_Shop,
    XKB_KEY_XF86History,            Qt::Key_History,
    XKB_KEY_XF86AddFavorite,        Qt::Key_AddFavorite,
    XKB_KEY_XF86HotLinks,           Qt::Key_HotLinks,
    XKB_KEY_XF86BrightnessAdjust,   Qt::Key_BrightnessAdjust,
    XKB_KEY_XF86Finance,            Qt::Key_Finance,
    XKB_KEY_XF86Community,          Qt::Key_Community,
    XKB_KEY_XF86AudioRewind,        Qt::Key_AudioRewind,
    XKB_KEY_XF86BackForward,        Qt::Key_BackForward,
    XKB_KEY_XF86ApplicationLeft,    Qt::Key_ApplicationLeft,
    XKB_KEY_XF86ApplicationRight,   Qt::Key_ApplicationRight,
    XKB_KEY_XF86Book,               Qt::Key_Book,
    XKB_KEY_XF86CD,                 Qt::Key_CD,
    XKB_KEY_XF86Calculater,         Qt::Key_Calculator,
    XKB_KEY_XF86Clear,              Qt::Key_Clear,
    XKB_KEY_XF86ClearGrab,          Qt::Key_ClearGrab,
    XKB_KEY_XF86Close,              Qt::Key_Close,
    XKB_KEY_XF86Copy,               Qt::Key_Copy,
    XKB_KEY_XF86Cut,                Qt::Key_Cut,
    XKB_KEY_XF86Display,            Qt::Key_Display,
    XKB_KEY_XF86DOS,                Qt::Key_DOS,
    XKB_KEY_XF86Documents,          Qt::Key_Documents,
    XKB_KEY_XF86Excel,              Qt::Key_Excel,
    XKB_KEY_XF86Explorer,           Qt::Key_Explorer,
    XKB_KEY_XF86Game,               Qt::Key_Game,
    XKB_KEY_XF86Go,                 Qt::Key_Go,
    XKB_KEY_XF86iTouch,             Qt::Key_iTouch,
    XKB_KEY_XF86LogOff,             Qt::Key_LogOff,
    XKB_KEY_XF86Market,             Qt::Key_Market,
    XKB_KEY_XF86Meeting,            Qt::Key_Meeting,
    XKB_KEY_XF86MenuKB,             Qt::Key_MenuKB,
    XKB_KEY_XF86MenuPB,             Qt::Key_MenuPB,
    XKB_KEY_XF86MySites,            Qt::Key_MySites,
    XKB_KEY_XF86New,                Qt::Key_New,
    XKB_KEY_XF86News,               Qt::Key_News,
    XKB_KEY_XF86OfficeHome,         Qt::Key_OfficeHome,
    XKB_KEY_XF86Open,               Qt::Key_Open,
    XKB_KEY_XF86Option,             Qt::Key_Option,
    XKB_KEY_XF86Paste,              Qt::Key_Paste,
    XKB_KEY_XF86Phone,              Qt::Key_Phone,
    XKB_KEY_XF86Reply,              Qt::Key_Reply,
    XKB_KEY_XF86Reload,             Qt::Key_Reload,
    XKB_KEY_XF86RotateWindows,      Qt::Key_RotateWindows,
    XKB_KEY_XF86RotationPB,         Qt::Key_RotationPB,
    XKB_KEY_XF86RotationKB,         Qt::Key_RotationKB,
    XKB_KEY_XF86Save,               Qt::Key_Save,
    XKB_KEY_XF86Send,               Qt::Key_Send,
    XKB_KEY_XF86Spell,              Qt::Key_Spell,
    XKB_KEY_XF86SplitScreen,        Qt::Key_SplitScreen,
    XKB_KEY_XF86Support,            Qt::Key_Support,
    XKB_KEY_XF86TaskPane,           Qt::Key_TaskPane,
    XKB_KEY_XF86Terminal,           Qt::Key_Terminal,
    XKB_KEY_XF86Tools,              Qt::Key_Tools,
    XKB_KEY_XF86Travel,             Qt::Key_Travel,
    XKB_KEY_XF86Video,              Qt::Key_Video,
    XKB_KEY_XF86Word,               Qt::Key_Word,
    XKB_KEY_XF86Xfer,               Qt::Key_Xfer,
    XKB_KEY_XF86ZoomIn,             Qt::Key_ZoomIn,
    XKB_KEY_XF86ZoomOut,            Qt::Key_ZoomOut,
    XKB_KEY_XF86Away,               Qt::Key_Away,
    XKB_KEY_XF86Messenger,          Qt::Key_Messenger,
    XKB_KEY_XF86WebCam,             Qt::Key_WebCam,
    XKB_KEY_XF86MailForward,        Qt::Key_MailForward,
    XKB_KEY_XF86Pictures,           Qt::Key_Pictures,
    XKB_KEY_XF86Music,              Qt::Key_Music,
    XKB_KEY_XF86Battery,            Qt::Key_Battery,
    XKB_KEY_XF86Bluetooth,          Qt::Key_Bluetooth,
    XKB_KEY_XF86WLAN,               Qt::Key_WLAN,
    XKB_KEY_XF86UWB,                Qt::Key_UWB,
    XKB_KEY_XF86AudioForward,       Qt::Key_AudioForward,
    XKB_KEY_XF86AudioRepeat,        Qt::Key_AudioRepeat,
    XKB_KEY_XF86AudioRandomPlay,    Qt::Key_AudioRandomPlay,
    XKB_KEY_XF86Subtitle,           Qt::Key_Subtitle,
    XKB_KEY_XF86AudioCycleTrack,    Qt::Key_AudioCycleTrack,
    XKB_KEY_XF86Time,               Qt::Key_Time,
    XKB_KEY_XF86Select,             Qt::Key_Select,
    XKB_KEY_XF86View,               Qt::Key_View,
    XKB_KEY_XF86TopMenu,            Qt::Key_TopMenu,
    XKB_KEY_XF86Red,                Qt::Key_Red,
    XKB_KEY_XF86Green,              Qt::Key_Green,
    XKB_KEY_XF86Yellow,             Qt::Key_Yellow,
    XKB_KEY_XF86Blue,               Qt::Key_Blue,
    XKB_KEY_XF86Bluetooth,          Qt::Key_Bluetooth,
    XKB_KEY_XF86Suspend,            Qt::Key_Suspend,
    XKB_KEY_XF86Hibernate,          Qt::Key_Hibernate,
    XKB_KEY_XF86TouchpadToggle,     Qt::Key_TouchpadToggle,
    XKB_KEY_XF86TouchpadOn,         Qt::Key_TouchpadOn,
    XKB_KEY_XF86TouchpadOff,        Qt::Key_TouchpadOff,
    XKB_KEY_XF86AudioMicMute,       Qt::Key_MicMute,
    XKB_KEY_XF86Launch0,            Qt::Key_Launch0,
    XKB_KEY_XF86Launch1,            Qt::Key_Launch1,
    XKB_KEY_XF86Launch2,            Qt::Key_Launch2,
    XKB_KEY_XF86Launch3,            Qt::Key_Launch3,
    XKB_KEY_XF86Launch4,            Qt::Key_Launch4,
    XKB_KEY_XF86Launch5,            Qt::Key_Launch5,
    XKB_KEY_XF86Launch6,            Qt::Key_Launch6,
    XKB_KEY_XF86Launch7,            Qt::Key_Launch7,
    XKB_KEY_XF86Launch8,            Qt::Key_Launch8,
    XKB_KEY_XF86Launch9,            Qt::Key_Launch9,
    XKB_KEY_XF86LaunchA,            Qt::Key_LaunchA,
    XKB_KEY_XF86LaunchB,            Qt::Key_LaunchB,
    XKB_KEY_XF86LaunchC,            Qt::Key_LaunchC,
    XKB_KEY_XF86LaunchD,            Qt::Key_LaunchD,
    XKB_KEY_XF86LaunchE,            Qt::Key_LaunchE,
    XKB_KEY_XF86LaunchF,            Qt::Key_LaunchF,
    0,                              0
};

static int lookupKeysym(xkb_keysym_t key)
{
    int code = 0;
    int i = 0;
    while (KeyTbl[i]) {
        if (key == KeyTbl[i]) {
            code = (int)KeyTbl[i+1];
            break;
        }
        i += 2;
    }

    return code;
}

int WebOSInputDevice::WebOSKeyboard::keysymToQtKey(xkb_keysym_t keysym, Qt::KeyboardModifiers modifiers, xkb_state *state, xkb_keycode_t code)
{
    Q_UNUSED(state);
    Q_UNUSED(code);

    int qtkey = 0;
    QString text;
    uint utf32 = xkb_keysym_to_utf32(keysym);
    if (utf32)
        text = QString::fromUcs4(&utf32, 1);

    if (keysym >= XKB_KEY_F1 && keysym <= XKB_KEY_F35) {
        qtkey =  Qt::Key_F1 + (int(keysym) - XKB_KEY_F1);
    } else if (keysym >= XKB_KEY_KP_Space && keysym <= XKB_KEY_KP_9) {
        if (keysym >= XKB_KEY_KP_0) {
            // numeric keypad keys
            qtkey = Qt::Key_0 + ((int)keysym - XKB_KEY_KP_0);
        } else {
            qtkey = lookupKeysym(keysym);
        }
        modifiers |= Qt::KeypadModifier;
    } else if (text.length() == 1 && text.unicode()->unicode() > 0x1f
            && text.unicode()->unicode() != 0x7f
            && !(keysym >= XKB_KEY_dead_grave && keysym <= XKB_KEY_dead_currency)
            && (int)(keysym) < ((int)Qt::Key_Escape)) {
        qtkey = text.unicode()->toUpper().unicode();
    } else {
        // any other keys
        qtkey = lookupKeysym(keysym);
    }

    return qtkey ? qtkey : keysym;
}
#else
std::pair<int, QString> WebOSInputDevice::WebOSKeyboard::keysymToQtKey(xkb_keysym_t keysym, Qt::KeyboardModifiers &modifiers)
{
    int code = 0;
    PMTRACE_FUNCTION;
    QString text;
    uint utf32 = xkb_keysym_to_utf32(keysym);
    if (utf32)
        text = QString::fromUcs4(&utf32, 1);

    if (keysym >= XKB_KEY_F1 && keysym <= XKB_KEY_F35) {
        code =  Qt::Key_F1 + (int(keysym) - XKB_KEY_F1);
    } else if (keysym >= XKB_KEY_KP_Space && keysym <= XKB_KEY_KP_9) {
        if (keysym >= XKB_KEY_KP_0) {
            // numeric keypad keys
            code = Qt::Key_0 + ((int)keysym - XKB_KEY_KP_0);
        } else {
            code = Keyboard::keysymToQtKey(keysym);
        }
        modifiers |= Qt::KeypadModifier;
    } else if (text.length() == 1 && text.unicode()->unicode() > 0x1f
            && text.unicode()->unicode() != 0x7f
            && !(keysym >= XKB_KEY_dead_grave && keysym <= XKB_KEY_dead_currency)
            && (int)(keysym) < ((int)Qt::Key_Escape)) {
        code = text.unicode()->toUpper().unicode();
    } else {
        // any other keys
        code = Keyboard::keysymToQtKey(keysym);
    }
    return { (code ? code : keysym), text };
}
#endif

#endif // QT_CONFIG(xkbcommon)

void WebOSInputDevice::WebOSKeyboard::keyboard_key(uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
    PMTRACE_FUNCTION;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    auto *window = Keyboard::focusWindow();
    if (window && !window->window()->isActive()) {
        // Due to the asynchronous window activation by the upstream commit f497a5b,
        // the window may not yet be activated at the time when the very first key
        // event arrives.
        // The window activation is triggered by a wl_keyboard::enter event from
        // the compositor and we can assume that a key event is always sent to a
        // surface focused. Thus there is no issue with activating window earlier
        // than the enter event is processed completely.
        // Note that the activation done here does not affect to active window
        // handling in QWaylandDisplay.(See QWaylandDisplay::handleWaylandSync())
        QWindowSystemInterface::handleWindowActivated(window->window());
    }
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#if QT_CONFIG(xkbcommon)
    if (!loadKeyMap())
        return;
#endif
#endif

    Keyboard::keyboard_key(serial, time, key, state);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    //In WebOS, we don't support repeat key by qtwayland
    stopRepeat();
#endif
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void WebOSInputDevice::WebOSKeyboard::keyboard_keymap(uint32_t format, int32_t fd, uint32_t size)
{
    PMTRACE_FUNCTION;
#if QT_CONFIG(xkbcommon)
    if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        close(fd);
        return;
    }

    mKeymapFd = fd;
    mKeymapSize = size;
    mPendingKeymap = true;
#else
    Keyboard::keyboard_keymap(format, fd, size);
#endif
}

void WebOSInputDevice::WebOSKeyboard::keyboard_modifiers(uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
{
    PMTRACE_FUNCTION;

#if QT_CONFIG(xkbcommon)
    if (!loadKeyMap())
        return;
#endif

    Keyboard::keyboard_modifiers(serial, mods_depressed, mods_latched, mods_locked, group);
}

#if QT_CONFIG(xkbcommon)
bool WebOSInputDevice::WebOSKeyboard::loadKeyMap()
{
    if (!mPendingKeymap)
        return true;

    if (mKeymapFormat != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        qWarning() << "unknown keymap format:" << mKeymapFormat;
        close(mKeymapFd);
        return false;
    }

    Keyboard::keyboard_keymap(mKeymapFormat, mKeymapFd, mKeymapSize);
    mPendingKeymap = false;
    return true;
}
#endif
#endif

WebOSInputDevice::WebOSPointer::WebOSPointer(QWaylandInputDevice *device)
    : Pointer(device)
    , m_origin(QPointF(0, 0))
{
}

void WebOSInputDevice::WebOSPointer::pointer_enter(uint32_t serial, struct wl_surface *surface, wl_fixed_t sx, wl_fixed_t sy)
{
    Q_UNUSED(sx);
    Q_UNUSED(sy);
    PMTRACE_FUNCTION;

    if (!surface)
        return;

    QWaylandWindow *window = QWaylandWindow::fromWlSurface(surface);
    if (!window)
        return;

    WebOSInputDevice *parent = static_cast<WebOSInputDevice*>(mParent);
    parent->setTime(QWaylandDisplay::currentTimeMillisec());
    parent->setSerial(serial);
    mEnterSerial = serial;

    bool focusChanged = false;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QWaylandSurface *qwSurface = window->waylandSurface();
    if (mFocus != qwSurface) {
        mFocus = qwSurface;
        focusChanged = true;
    }
#else
    if (mFocus != window) {
        mFocus = window;
        focusChanged = true;
    }
#endif

    if (focusChanged) {
        WebOSPlatformWindow *ww = static_cast<WebOSPlatformWindow *>(window);
        m_origin = ww->position();
        connect(ww, &WebOSPlatformWindow::resizeRequested, parent, [this] {
            this->pauseEvents();
        });
        connect(ww, &WebOSPlatformWindow::positionChanged, parent, [this](const QPointF &position) {
            this->flushPausedEvents(position);
        });
    }

    QWaylandWindow *grab = QWaylandWindow::mouseGrab();
    if (!grab) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QtWaylandClient::QWaylandPointerEvent enter(QEvent::Type::Enter, Qt::ScrollBegin, window,
                parent->getTime(), mSurfacePos, mGlobalPos, mButtons, Qt::NoButton, parent->modifiers());
#else
        QtWaylandClient::QWaylandPointerEvent enter(QtWaylandClient::QWaylandPointerEvent::Enter, parent->getTime(),
                mSurfacePos, mGlobalPos, mButtons, parent->modifiers());
#endif
        window->handleMouse(parent, enter);
    }
}

void WebOSInputDevice::WebOSPointer::pointer_leave(uint32_t time, struct wl_surface *surface)
{
    PMTRACE_FUNCTION;

    WebOSInputDevice *parent = static_cast<WebOSInputDevice*>(mParent);
    WebOSPlatformWindow *ww = static_cast<WebOSPlatformWindow *>(QWaylandWindow::fromWlSurface(surface));
    if (ww)
        ww->disconnect(parent);

    if (Q_UNLIKELY(m_paused))
        flushPausedEvents();

    Pointer::pointer_leave(time, surface);
}

void WebOSInputDevice::WebOSPointer::pointer_motion(uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    PMTRACE_FUNCTION;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (mFocus.isNull())
        return;
#else
    if (!mFocus)
        return;
#endif

    if (Q_UNLIKELY(m_paused)) {
        qDebug() << "Delayed pointer_motion:" << time << wl_fixed_to_double(surface_x) << wl_fixed_to_double(surface_y);
        Event e;
        e.type = Type::motion;
        e.args.motion = {time, surface_x, surface_y};
        m_pendingEvents << e;
        return;
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QWaylandWindow *window = mFocus->waylandWindow();
    if (window)
        Pointer::pointer_motion(time, surface_x / window->devicePixelRatio(), surface_y / window->devicePixelRatio());
#else
    Pointer::pointer_motion(time, surface_x / mFocus->devicePixelRatio(), surface_y / mFocus->devicePixelRatio());
#endif
}

void WebOSInputDevice::WebOSPointer::pointer_button(uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
    PMTRACE_FUNCTION;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (mFocus.isNull())
        return;
#else
    if (!mFocus)
        return;
#endif

    if (Q_UNLIKELY(m_paused)) {
        qDebug() << "Delayed pointer_button:" << serial << time << button << state;
        Event e;
        e.type = Type::button;
        e.args.button = {serial, time, button, state};
        m_pendingEvents << e;
        return;
    }

    Pointer::pointer_button(serial, time, button, state);
}

void WebOSInputDevice::WebOSPointer::pointer_axis(uint32_t time, uint32_t axis, int32_t value)
{
    PMTRACE_FUNCTION;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (mFocus.isNull())
        return;
#else
    if (!mFocus)
        return;
#endif

    if (Q_UNLIKELY(m_paused)) {
        qDebug() << "Delayed pointer_axis:" << time << axis << value;
        Event e;
        e.type = Type::axis;
        e.args.axis = {time, axis, value};
        m_pendingEvents << e;
        return;
    }

    Pointer::pointer_axis(time, axis, value);
}

void WebOSInputDevice::WebOSPointer::pauseEvents()
{
    m_paused = true;
    // Pause events upto the given interval just in case the compositor doesn't respond
    connect(&m_pauseTimer, &QTimer::timeout, static_cast<WebOSInputDevice*>(mParent), [this] {
        this->flushPausedEvents();
    });
    m_pauseTimer.setSingleShot(true);
    m_pauseTimer.setInterval(500);
    m_pauseTimer.start();
    qDebug() << "Start delayed pointer event handling, current origin:" << m_origin;
}

void WebOSInputDevice::WebOSPointer::flushPausedEvents(const QPointF &origin)
{
    QPointF newOrigin = origin;
    if (newOrigin.isNull())
        newOrigin = m_origin;

    if (!m_paused) {
        // Nothing to be done except for updating m_origin
        m_origin = newOrigin;
        return;
    }

    m_paused = false;
    m_pauseTimer.stop();

    qreal dx = m_origin.x() - newOrigin.x();
    qreal dy = m_origin.y() - newOrigin.y();

    // Replay queued events with coordinates translated to newOrigin
    while (!m_pendingEvents.isEmpty()) {
        Event e = m_pendingEvents.takeFirst();
        switch (e.type) {
        case Type::motion:
            qDebug() << "Sending delayed pointer_motion(adjusted):" << e.args.motion.time
                << wl_fixed_to_double(e.args.motion.surface_x) << "+" << dx
                << wl_fixed_to_double(e.args.motion.surface_y) << "+" << dy;
            pointer_motion(e.args.motion.time, e.args.motion.surface_x + wl_fixed_from_double(dx), e.args.motion.surface_y + wl_fixed_from_double(dy));
            break;
        case Type::button:
            qDebug() << "Sending delayed pointer_button:" << e.args.button.serial << e.args.button.time
                << e.args.button.button << e.args.button.state;
            pointer_button(e.args.button.serial, e.args.button.time, e.args.button.button, e.args.button.state);
            break;
        case Type::axis:
            qDebug() << "Sending delayed pointer_axis:" << e.args.axis.time
                << e.args.axis.axis << e.args.axis.value;
            pointer_axis(e.args.axis.time, e.args.axis.axis, e.args.axis.value);
            break;
        }
    }

    qDebug() << "End delayed pointer event handling with origin:" << m_origin << "=>" << newOrigin;

    m_origin = newOrigin;
}

WebOSInputDevice::WebOSTouch::WebOSTouch(QWaylandInputDevice *device)
    : Touch(device)
{
}

void WebOSInputDevice::WebOSTouch::registerTouchDevice()
{
    PMTRACE_FUNCTION;
    WebOSInputDevice *device = static_cast<WebOSInputDevice *>(mParent);
    device->registerTouchDevice();
}

void WebOSInputDevice::WebOSTouch::touch_down(uint32_t serial, uint32_t time, struct wl_surface *surface, int32_t id, wl_fixed_t x, wl_fixed_t y)
{
    PMTRACE_FUNCTION;
    if (!surface)
        return;
    QWaylandWindow *window = QWaylandWindow::fromWlSurface(surface);
    if (!window)
        return;
    Touch::touch_down(serial, time, surface, id, x / window->devicePixelRatio(), y / window->devicePixelRatio());
}

void WebOSInputDevice::WebOSTouch::touch_motion(uint32_t time, int32_t id, wl_fixed_t x, wl_fixed_t y)
{
    PMTRACE_FUNCTION;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (mFocus.isNull())
        return;
#else
    if (!mFocus)
        return;
#endif

    Touch::touch_motion(time, id, x / mFocus->devicePixelRatio(), y / mFocus->devicePixelRatio());
}

void WebOSInputDevice::WebOSTouch::touch_cancel()
{
    PMTRACE_FUNCTION;
    registerTouchDevice();
    Touch::touch_cancel();
}

void WebOSInputDevice::WebOSTouch::touch_frame()
{
    PMTRACE_FUNCTION;
    registerTouchDevice();
    Touch::touch_frame();
}
