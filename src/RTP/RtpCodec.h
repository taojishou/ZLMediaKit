//
// Created by xzl on 2018/10/18.
//

#ifndef ZLMEDIAKIT_RTPCODEC_H
#define ZLMEDIAKIT_RTPCODEC_H

#include <memory>
#include "Util/RingBuffer.h"
#include "Rtsp/Rtsp.h"
#include "Player/PlayerBase.h"

using namespace std;
using namespace ZL::Util;
using namespace ZL::Player;

class RtpPacket {
public:
    typedef std::shared_ptr<RtpPacket> Ptr;
    uint8_t interleaved;
    uint8_t PT;
    bool mark;
    uint32_t length;
    uint32_t timeStamp;
    uint16_t sequence;
    uint32_t ssrc;
    uint8_t payload[1560];
    uint8_t offset;
    TrackType type;
};

class RtpRingInterface {
public:
    typedef RingBuffer<RtpPacket::Ptr> RingType;

    RtpRingInterface(){}
    virtual ~RtpRingInterface(){}
    virtual RingType::Ptr getRtpRing() const = 0;
    virtual void setRtpRing(const RingType::Ptr &ring) = 0;
    virtual void inputRtp(const RtpPacket::Ptr &rtp, bool key_pos) = 0;
};

class RtpRing : public RtpRingInterface {
public:
    typedef std::shared_ptr<RtpRing> Ptr;

    RtpRing(){
        //禁用缓存
        _rtpRing = std::make_shared<RingType>(1);
    }
    virtual ~RtpRing(){}

    RingType::Ptr getRtpRing() const override {
        return _rtpRing;
    }

    void setRtpRing(const RingType::Ptr &ring) override {
        _rtpRing = ring;
    }

    void inputRtp(const RtpPacket::Ptr &rtp, bool key_pos) override{
        _rtpRing->write(rtp,key_pos);
    }
protected:
    RingType::Ptr _rtpRing;
};


class RtpInfo{
public:
    typedef std::shared_ptr<RtpInfo> Ptr;

    RtpInfo(uint32_t ui32Ssrc,
               uint32_t ui32MtuSize,
               uint32_t ui32SampleRate,
               uint8_t ui8PlayloadType,
               uint8_t ui8Interleaved) {
        m_ui32Ssrc = ui32Ssrc;
        m_ui32SampleRate = ui32SampleRate;
        m_ui32MtuSize = ui32MtuSize;
        m_ui8PlayloadType = ui8PlayloadType;
        m_ui8Interleaved = ui8Interleaved;
        m_rtpPool.setSize(32);
    }

    virtual ~RtpInfo(){}

    int getInterleaved() const {
        return m_ui8Interleaved;
    }

    int getPlayloadType() const {
        return m_ui8PlayloadType;
    }

    int getSampleRate() const {
        return m_ui32SampleRate;
    }

    uint32_t getSsrc() const {
        return m_ui32Ssrc;
    }

    uint16_t getSeqence() const {
        return m_ui16Sequence;
    }
    uint32_t getTimestamp() const {
        return m_ui32TimeStamp;
    }
    uint32_t getMtuSize() const {
        return m_ui32MtuSize;
    }
protected:
    RtpPacket::Ptr obtainRtp(){
        return m_rtpPool.obtain();
    }
protected:
    uint32_t m_ui32Ssrc;
    uint32_t m_ui32SampleRate;
    uint32_t m_ui32MtuSize;
    uint8_t m_ui8PlayloadType;
    uint8_t m_ui8Interleaved;
    uint16_t m_ui16Sequence = 0;
    uint32_t m_ui32TimeStamp = 0;
    ResourcePool<RtpPacket> m_rtpPool;
};

class RtpCodec : public RtpRing, public FrameRing , public CodecInfo{
public:
    typedef std::shared_ptr<RtpCodec> Ptr;
    RtpCodec(){}
    virtual ~RtpCodec(){}
};

class RtpEncoder : public RtpInfo, public RtpCodec{
public:
    typedef std::shared_ptr<RtpEncoder> Ptr;

    /**
     * @param ui32Ssrc ssrc
     * @param ui32MtuSize mtu大小
     * @param ui32SampleRate 采样率，强制为90000
     * @param ui8PlayloadType pt类型
     * @param ui8Interleaved rtsp interleaved
     */
    RtpEncoder(uint32_t ui32Ssrc,
               uint32_t ui32MtuSize = 1400,
               uint32_t ui32SampleRate = 90000,
               uint8_t ui8PlayloadType = 96,
               uint8_t ui8Interleaved = TrackVideo * 2);
    ~RtpEncoder() {}
};





#endif //ZLMEDIAKIT_RTPCODEC_H