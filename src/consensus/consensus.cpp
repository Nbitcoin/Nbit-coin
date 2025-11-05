#include "consensus.h"
#include <random.h>

std::map<uint256, StakingNode> stakingNodes;

bool StakeCoins(uint64_t amount) {
    if (amount < 1000) {
        return false;
    }
    uint256 nodeId = GetRandHash();
    stakingNodes[nodeId] = {nodeId, amount, GetTime()};
    return true;
}

uint256 SelectNextBlockProducer() {
    if (stakingNodes.empty()) return uint256();
    
    uint64_t totalStake = 0;
    for (const auto& pair : stakingNodes) {
        totalStake += pair.second.stakeAmount;
    }
    
    uint64_t rand = GetRand(totalStake);
    uint64_t accumulated = 0;
    for (const auto& node : stakingNodes) {
        accumulated += node.second.stakeAmount;
        if (accumulated >= rand) {
            return node.first;
        }
    }
    return stakingNodes.rbegin()->first;
}

bool VerifyBlockProducer(const uint256& producerId, uint64_t blockTime) {
    auto it = stakingNodes.find(producerId);
    if (it == stakingNodes.end()) return false;
    if (blockTime - it->second.lastBlockTime < 60) {
        return false;
    }
    it->second.lastBlockTime = blockTime;
    return true;
}

