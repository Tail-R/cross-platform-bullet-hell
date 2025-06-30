#include "packet_template.hpp"

PayloadType Packet::payload_type() {
    return std::visit([](auto&& msg) -> PayloadType {
        using T = std::decay_t<decltype(msg)>;

        if      constexpr (std::is_same_v<T, ClientHello>)              return PayloadType::Hello;
        else if constexpr (std::is_same_v<T, ServerAccept>)             return PayloadType::Accept;
        else if constexpr (std::is_same_v<T, ClientGoodBye>)            return PayloadType::GoodBye;
        else if constexpr (std::is_same_v<T, ServerGoodBye>)            return PayloadType::GoodBye;
        else if constexpr (std::is_same_v<T, ClientGameRequest>)        return PayloadType::GameRequest;
        else if constexpr (std::is_same_v<T, ServerGameResponse>)       return PayloadType::GameResponse;
        else if constexpr (std::is_same_v<T, ClientReconnectRequest>)   return PayloadType::ReconnectRequest;
        else if constexpr (std::is_same_v<T, ServerReconnectResponse>)  return PayloadType::ReconnectResponse;
        else if constexpr (std::is_same_v<T, ClientInput>)              return PayloadType::Input;
        else if constexpr (std::is_same_v<T, FrameSnapshot>)            return PayloadType::FrameSnapshot;
        else                                                            return PayloadType::Unknown;
    }, payload);
}