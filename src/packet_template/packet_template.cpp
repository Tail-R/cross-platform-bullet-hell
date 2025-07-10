#include "packet_template.hpp"

PayloadType get_payload_type(const PacketPayload& payload) {
    return std::visit([](auto&& msg) -> PayloadType {
        using T = std::decay_t<decltype(msg)>;

        if      constexpr (std::is_same_v<T, ClientHello>)              return PayloadType::ClientHello;
        else if constexpr (std::is_same_v<T, ServerAccept>)             return PayloadType::ServerAccept;
        else if constexpr (std::is_same_v<T, ClientGoodbye>)            return PayloadType::ClientGoodbye;
        else if constexpr (std::is_same_v<T, ServerGoodbye>)            return PayloadType::ServerGoodbye;
        else if constexpr (std::is_same_v<T, ClientGameRequest>)        return PayloadType::ClientGameRequest;
        else if constexpr (std::is_same_v<T, ServerGameResponse>)       return PayloadType::ServerGameResponse;
        else if constexpr (std::is_same_v<T, ClientReconnectRequest>)   return PayloadType::ClientReconnectRequest;
        else if constexpr (std::is_same_v<T, ServerReconnectResponse>)  return PayloadType::ServerReconnectResponse;
        else if constexpr (std::is_same_v<T, ClientInput>)              return PayloadType::ClientInput;
        else if constexpr (std::is_same_v<T, FrameSnapshot>)            return PayloadType::FrameSnapshot;
        else                                                            return PayloadType::Unknown;
    }, payload);
}