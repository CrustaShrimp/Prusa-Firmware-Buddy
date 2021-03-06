#include "window_header.hpp"
#include "config.h"
#include "marlin_client.h"
#include "i18n.h"
#include "marlin_events.h"

#ifdef BUDDY_ENABLE_ETHERNET
    #include "wui_api.h"
#endif //BUDDY_ENABLE_ETHERNET

void window_header_t::update_ETH_icon() {
#ifdef BUDDY_ENABLE_ETHERNET
    if (get_eth_status() == ETH_UNLINKED) {
        LAN_Off();
    } else if (get_eth_status() == ETH_NETIF_DOWN) {
        LAN_On();
    } else {
        LAN_Activate();
    }
#else
    LAN_Off();
#endif // BUDDY_ENABLE_ETHERNET
}

void window_header_t::SetIcon(int16_t id_res) {
    icon_base.SetIdRes(id_res);
    Invalidate();
}

void window_header_t::SetText(string_view_utf8 txt) {
    label.SetText(txt);
    Invalidate();
}

void window_header_t::EventClr() {
    EventClr_MediaInserted();
    EventClr_MediaRemoved();
    EventClr_MediaError();
}

bool window_header_t::EventClr_MediaInserted() {
    /* lwip fces only read states, invalid states by another thread never mind */
    update_ETH_icon();
    if (marlin_event_clr(MARLIN_EVT_MediaInserted)) {
        USB_Activate();
        return 1;
    }
    return 0;
}

bool window_header_t::EventClr_MediaRemoved() {
    /* lwip fces only read states, invalid states by another thread never mind */
    update_ETH_icon();
    if (marlin_event_clr(MARLIN_EVT_MediaRemoved)) {
        USB_On();
        return 1;
    }
    return 0;
}

bool window_header_t::EventClr_MediaError() {
    /* lwip fces only read states, invalid states by another thread never mind */
    update_ETH_icon();
    if (marlin_event_clr(MARLIN_EVT_MediaError)) {
        return 1;
    }
    return 0;
}

static const size_t icon_usb_width = 36 + 10;
static const size_t icon_lan_width = 20 + 10;
static const size_t icons_width = icon_usb_width + icon_lan_width;

window_header_t::window_header_t(window_t *parent)
    : window_frame_t(parent, GuiDefaults::RectHeader)
    , icon_base(this, rect_ui16(rect.x + 10, rect.y, rect.h, rect.h), 0)
    , label(this, rect_ui16(rect.x + 10 + rect.h, rect.y, rect.w - icons_width - 10 - rect.h, rect.h))
    , icon_usb(this, rect_ui16(rect.x + rect.w - icon_usb_width, rect.y, icon_usb_width, rect.h), IDR_PNG_header_icon_usb)
    , icon_lan(this, rect_ui16(rect.x + rect.w - icons_width, rect.y, icon_lan_width, rect.h), IDR_PNG_header_icon_lan) {
    label.alignment = ALIGN_LEFT_CENTER;

    marlin_vars()->media_inserted ? USB_Activate() : USB_On();

    update_ETH_icon();
    Disable();
}

void window_header_t::USB_Off() { icon_usb.Hide(); }
void window_header_t::USB_On() {
    icon_usb.Show();
    icon_usb.Shadow();
}
void window_header_t::USB_Activate() {
    icon_usb.Show();
    icon_usb.Unshadow();
}
void window_header_t::LAN_Off() { icon_lan.Hide(); }
void window_header_t::LAN_On() {
    icon_lan.Show();
    icon_lan.Shadow();
}
void window_header_t::LAN_Activate() {
    icon_lan.Show();
    icon_lan.Unshadow();
}

window_header_t::header_states_t window_header_t::GetStateUSB() const {
    if (!icon_usb.IsVisible())
        return header_states_t::OFF;
    return icon_usb.IsEnabled() ? header_states_t::ACTIVE : header_states_t::ON;
}
window_header_t::header_states_t window_header_t::GetStateLAN() const {
    if (!icon_lan.IsVisible())
        return header_states_t::OFF;
    return icon_lan.IsEnabled() ? header_states_t::ACTIVE : header_states_t::ON;
}
