//
// Created by maara on 4/19/17.
//

#include <iomanip>
#include <sstream>
#include "Stats.h"

std::ostream& operator<<(std::ostream &out, const SendStatsLine &line) {
    return out << "stats[" << line.bufferId << "]{"
               << "timestamp=" << line.timestamp
               << " dropped=" << line.droppedFrames
               << " msActual=" << (line.nanoPerFrameActual/1000000)
               << " msExpected=" << (line.nanoPerFrameExpected/1000000)
               << " sentBytes=" << line.sendBytesTotal
               << " msCompress=" << line.compressMillis
               << "}";
}

std::ostream& operator<<(std::ostream &out, const RecvStatsLine &line) {
    return out << "stats[" << line.bufferId << "]{"
               << "timestamp=" << line.timestamp
               << " packetsExp=" << line.expectedPackets
               << " packetsRecv=" << line.receivedPackets
               << " bytesExp=" << line.expectedBytes
               << " bytesRecv=" << line.receivedBytes
               << " corrupted=" << line.isCorrupted
               << " displayed=" << line.isDisplayed
               << " msDecompress=" << (line.nanoPerFrameDecompress/1000000)
               << " msExpected=" << (line.nanoPerFrameExpected/1000000)
               << " reported=" << line.reportedFrames
               << " receivedAt=" << line.receivedAt
               << "}";
}

std::ostream &operator<<(std::ostream &out, const StatsPair &pair) {
    return out << "[" << pair.first << ", " << pair.second << "]";
}

std::ostream &operator<<(std::ostream &out, const StatsDiff &pair) {
    uint64_t len = pair.length();
    
    out << "StatsDiff[" << pair.start.first.bufferId << "+" << len << "]{";
    
    if (pair.isOk()) {
        out << "ok}";
    } else {
        bool preSemi = false;
        if (pair.lostBytes() > 0) {
            out << (preSemi?"; ":"") << std::setprecision(4) << (pair.lostBytesRel() * 100.0) << "%(" << pair.lostBytes() << ") of " << pair.totalBytes() << " Bytes lost";
            preSemi = true;
        }
        if (pair.droppedFrames() > 0) {
            out << (preSemi?"; ":"") << pair.droppedFrames() << " frames dropped";
            preSemi = true;
        }
        if (pair.timeCompressRel() >= 0.9) {
            double rel = pair.timeCompressRel()*100.0;
            out << (preSemi?"; ":"") << "compress took " << std::setprecision(rel >= 100 ? 3 : 2) << rel << "% of available time";
            preSemi = true;
        }
        if (pair.timeDecompressRel() >= 0.9) {
            double rel = pair.timeDecompressRel()*100.0;
            out << (preSemi?"; ":"") << "decompress took " << std::setprecision(rel >= 100 ? 3 : 2) << rel << "% of available time";
            preSemi = true;
        }
        if (pair.timeSendRel() >= 0.9) {
            double rel = pair.timeSendRel()*100.0;
            out << (preSemi?"; ":"") << "send took " << std::setprecision(rel >= 100 ? 3 : 2) << rel << "% of available time";
            preSemi = true;
        }
        if (pair.displayed() < len) {
            out << (preSemi?"; ":"") << "displayed " << (pair.displayed()) << "/" << len << " frames";
            preSemi = true;
        }
        if (pair.corrupted() > 0) {
            out << (preSemi?"; ":"") << (pair.corrupted()) << "/" << len << " frames were corrupted";
            preSemi = true;
        }
        out << "}";
    }
    return out;
}

uint64_t StatsDiff::lostPackets() const {
    return (end.second.expectedPackets - start.second.expectedPackets) - (end.second.receivedPackets - start.second.receivedPackets);
}

uint64_t StatsDiff::lostBytes() const {
    return (end.second.expectedBytes - start.second.expectedBytes) - (end.second.receivedBytes - start.second.receivedBytes);
}

uint64_t StatsDiff::totalPackets() const {
    return (end.second.expectedPackets - start.second.expectedPackets);
}

uint64_t StatsDiff::totalBytes() const {
    return end.second.expectedBytes - start.second.expectedBytes;
}

uint64_t StatsDiff::droppedFrames() const {
    return end.first.droppedFrames - start.first.droppedFrames;
}

uint64_t StatsDiff::displayed() const {
    return end.second.isDisplayed - start.second.isDisplayed;
}

uint64_t StatsDiff::corrupted() const {
    return end.second.isCorrupted - start.second.isCorrupted;
}

uint64_t StatsDiff::length() const {
    return end.first.bufferId - start.first.bufferId;
}

uint64_t StatsDiff::timeDecompress() const {
    return end.second.nanoPerFrameDecompress - start.second.nanoPerFrameDecompress;
}

uint64_t StatsDiff::timeCompress() const {
    uint64_t comp = end.first.compressMillis - start.first.compressMillis;
    comp = comp == 0 ? 0 : (comp - 1) * 1000000;
    return comp;
}

uint64_t StatsDiff::timeSend() const {
    return end.first.nanoPerFrameActual - start.first.nanoPerFrameActual;
}

uint64_t StatsDiff::timeExpected() const {
    return end.first.nanoPerFrameExpected - start.first.nanoPerFrameExpected;
}

double StatsDiff::timeDecompressRel() const {
    return ((double) timeDecompress()) / timeExpected();
}

double StatsDiff::timeCompressRel() const {
    return ((double) timeCompress()) / timeExpected();
}

double StatsDiff::timeSendRel() const {
    return ((double) timeSend()) / timeExpected();
}

double StatsDiff::lostBytesRel() const {
    return ((double) lostBytes()) / totalBytes();
}

double StatsDiff::lostPacketsRel() const {
    return totalPackets() == 0 ? 0 : ((double) lostPackets()) / totalPackets();
}

bool StatsDiff::isOk() const {
    return lostPackets() == 0
           && corrupted() == 0
           && droppedFrames() == 0
           && timeCompressRel() >= 1
           && timeDecompressRel() >= 1
           && timeSendRel() >= 1;
}

std::ostream &StatsDiff::printDetail(std::ostream &out) const {
    return printDetail(out, (const std::string &) " ");
}

std::ostream &StatsDiff::printDetail(std::ostream &out, const std::string &separator) const {
    double rel;
    out << (*this) << ":" << separator;
    out << "timeExpMs " << timeExpected() << separator;
    rel = timeDecompressRel()*100;
    out << "timeDecMs " << timeDecompress()/1000000 << "(" << std::setprecision(rel >= 100 ? 3 : 2) << rel << "%)" << separator;
    rel = timeCompressRel()*100;
    out << "timeEncMs " << timeCompress()/1000000 << "(" << std::setprecision(rel >= 100 ? 3 : 2) << rel << "%)" << separator;
    rel = timeSendRel();
    out << "timeSendMs " << timeSend()/1000000 << "(" << std::setprecision(rel >= 100 ? 3 : 2) << rel << "%)" << separator;
    out << "bytes " << totalBytes() << separator;
    rel = lostBytesRel();
    out << "lostBytes " << lostBytes() << "(" << std::setprecision(rel >= 100 ? 3 : 2) << rel << "%)" << separator;
    out << "corrupted " << corrupted() << separator;
    out << "displayed " << displayed();
    return out;
}

