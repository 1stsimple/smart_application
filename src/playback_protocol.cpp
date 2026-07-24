#include "smart_home/playback_protocol.hpp"
#include "smart_home/hls_auth.hpp"

#include <chrono>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace smart_home {
namespace {

void put32(std::vector<std::uint8_t>& output, std::uint32_t value) {
    output.push_back(static_cast<std::uint8_t>(value >> 24));
    output.push_back(static_cast<std::uint8_t>(value >> 16));
    output.push_back(static_cast<std::uint8_t>(value >> 8));
    output.push_back(static_cast<std::uint8_t>(value));
}

void put64(std::vector<std::uint8_t>& output, std::uint64_t value) {
    for (int shift = 56; shift >= 0; shift -= 8)
        output.push_back(static_cast<std::uint8_t>(value >> shift));
}

void put_text(std::vector<std::uint8_t>& output, const std::string& value) {
    if (value.size() > 1024U * 1024U) throw std::length_error("recording text too long");
    put32(output, static_cast<std::uint32_t>(value.size()));
    output.insert(output.end(), value.begin(), value.end());
}

std::string file_name(const std::string& path) {
    const std::size_t slash = path.find_last_of("/\\");
    return slash == std::string::npos ? path : path.substr(slash + 1);
}

}  // namespace

PlaybackProtocolHandler::PlaybackProtocolHandler(RecordingRepository& repository,
    const std::string& public_hls_base_url, const std::string& access_secret,
    std::int64_t url_ttl_seconds)
    : repository_(repository), public_hls_base_url_(public_hls_base_url),
      access_secret_(access_secret), url_ttl_seconds_(url_ttl_seconds) {
    while (!public_hls_base_url_.empty() && public_hls_base_url_[public_hls_base_url_.size()-1]=='/')
        public_hls_base_url_.erase(public_hls_base_url_.size()-1);
    if (public_hls_base_url_.empty()) throw std::invalid_argument("public HLS URL is empty");
    if (access_secret_.size() < 16 || url_ttl_seconds_ <= 0)
        throw std::invalid_argument("HLS secret must be at least 16 bytes and TTL must be positive");
}

bool PlaybackProtocolHandler::handle(const TcpServer::ConnectionPtr& connection,
                                     const TlvPacket& packet) {
    const bool playback =
        packet.type == static_cast<std::uint32_t>(TaskType::GetPlaybackUrl);
    const bool list =
        packet.type == static_cast<std::uint32_t>(TaskType::QueryRecordingSegments);
    if (!playback && !list) return false;
    std::istringstream input(packet.text());
    std::uint32_t camera = 0, channel = 0;
    std::int64_t begin = 0, end = 0;
    std::string extra;
    if (!(input >> camera >> channel >> begin >> end) || (input >> extra) || end <= begin) {
        connection->send(TlvPacket(TaskType::Error,
            "playback payload: <camera_id> <channel> <begin_ms> <end_ms>"));
        return true;
    }
    const std::vector<RecordingSegment> segments =
        repository_.query(camera, channel, begin, end);
    if (list && segments.size() > 10000U) {
        connection->send(TlvPacket(
            TaskType::Error, "too many recording segments; narrow the time range"));
        return true;
    }
    if (segments.empty() && playback) {
        connection->send(TlvPacket(TaskType::Error, "recording not found"));
        return true;
    }
    const std::int64_t expires = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() + url_ttl_seconds_;
    if (list) {
        std::vector<std::uint8_t> value;
        put32(value, static_cast<std::uint32_t>(segments.size()));
        for (std::size_t i = 0; i < segments.size(); ++i) {
            const RecordingSegment& segment = segments[i];
            const std::string token = hls_access_token(
                access_secret_, segment.camera_id, segment.channel,
                segment.start_ms, segment.end_ms, expires);
            std::ostringstream download;
            download << public_hls_base_url_ << "/media/" << segment.id << ".ts"
                     << "?camera=" << segment.camera_id
                     << "&channel=" << segment.channel
                     << "&begin=" << segment.start_ms
                     << "&end=" << segment.end_ms
                     << "&expires=" << expires << "&token=" << token;
            put64(value, segment.id);
            put32(value, segment.camera_id);
            put32(value, segment.channel);
            put64(value, static_cast<std::uint64_t>(segment.start_ms));
            put64(value, static_cast<std::uint64_t>(segment.end_ms));
            put64(value, segment.size_bytes);
            put32(value, segment.discontinuity ? 1U : 0U);
            put_text(value, file_name(segment.path));
            put_text(value, download.str());
            put_text(value, segment.checksum);
        }
        connection->send(TlvPacket(
            static_cast<std::uint32_t>(TaskType::RecordingSegments), value));
        return true;
    }
    std::ostringstream url;
    const std::string token = hls_access_token(access_secret_, camera, channel,
                                                begin, end, expires);
    url << public_hls_base_url_ << "/hls/vod?camera=" << camera
        << "&channel=" << channel << "&begin=" << begin << "&end=" << end
        << "&expires=" << expires << "&token=" << token;
    connection->send(TlvPacket(TaskType::PlaybackUrl, url.str()));
    return true;
}

}  // namespace smart_home
