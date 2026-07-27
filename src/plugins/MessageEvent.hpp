#pragma once

class MessageEvent {
public:
    bool m_decline;

    void setDecline(bool decline) { m_decline = decline; }
private:
};