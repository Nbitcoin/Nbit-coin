#include “共识.h”
#include <string>
#include <map>
#include < stdint.h >
#include “uint256.h”
#include “块.h”
#include “随机.h”

定位证明结构（PoL 层）
结构 位置证明 {
    std::字符串 节点IP;
    std::字符串 gps哈希;
    布尔语 isVerified;
 int64_t verifyTime;
};

双质押信息结构（产品 层）
结构 质押信息 {
    uint64_t mainCoin金额;
    uint64_t usdt金额;
    int64_t 质押时间;
    uint256 节点 同上;
};

全局存储：节点位置（PoL）+双质押信息（PoS）
std::地图<uint256, LocationProof> nodeLocationMap;
std::地图<uint256, StakingInfo> nodeStakingMap;

油料：验证节点位置真实性
布尔语 验证位置(常量 uint256& nodeId, 常量 std::string& nodeIP, 常量 std::string& gpsHash) {
    布尔语 isRealLocation = 假;
    如果 (nodeIP.找到("192.168.") == 标准：：字符串：：npos && ！gpsHash.空()) {
        isRealLocation = 真;
    }
    节点位置图[节点 Id] = {nodeIP, gpsHash, isRealLocation, 获取时间()};
    返回 isRealLocation;
}

PoL：检查位置是否有效（未过期 + 已验证）
布尔语 IsLocationValid(常量 uint256& nodeId) {
    如果 (nodeLocationMap.找到(节点 Id) == nodeLocationMap.结束()) 返回 假;
    常量 自动& loc = nodeLocationMap[节点 Id];
    返回 loc.已验证 && (获取时间() - 位置。验证时间) <= 3600;
}

产品：注册双重质押（必须先通过油料）
布尔语 质押双资产(常量 uint256& nodeId， uint64_t mainCoin，uint64_t usdt, 常量 std::string& nodeIP, 常量 std::string& gpsHash) {
    如果 (!验证位置(节点 同上、节点IP、gps哈希)) 返回 假;
    如果 (主币 < 500 ||USDT<1000) 返回 假;
    nodeStakingMap[节点 Id] = {主币, usdt, 获取时间()、节点 ID};
 返回 真;
}

PoS：计算质押权重（金额+持续时间）
uint64_t 计算权益权重(常量 uint256& 节点 Id) {
    如果 (nodeStakingMap.找到(节点 Id) == nodeStakingMap.结束()) 返回 0;
    常量 自动& stake = nodeStakingMap[节点 Id];
    uint64_t 质押小时数 =(获取时间() -桩。质押时间) / 3600;
    如果 (stakeHours > 100) 质押小时数 =100;
    返回 stake.主币金额 + (桩。usdt金额 / 10) + 质押小时数;
}

核心：PoS + PoL 组合区块生产者选择
uint256 选择下一个块生产者（） {
 标准：：地图<uint256，uint64_t> validNodes;
    为 (常量 自动& pair : nodeStakingMap) {
        常量 uint256& nodeId = pair.第一;
        如果 (IsLocationValid(节点 同上)) {
            uint64_t 重量 =计算权益权重(节点 同上);
            如果 (重量> 0) 有效节点[节点 Id] = 重量;
        }
    }
    如果 (有效节点.空()) 返回 uint256();

    uint64_t 总重量 =0;
    为 (常量 自动& node : validNodes) totalWeight += node.第二;
    uint64_t 兰特 =GetRand(总重量);
 累积uint64_t = 0;
    为 (常量 自动& node : validNodes) {
        accumulated += node.第二;
        如果 (累计>=兰特) 返回 节点。第一;
    }
    返回 validNodes.rbegin()->第一;
}

验证区块生产者有效性（双层检查）
布尔语 验证区块生产者(常量 uint256& nodeId, 常量 CBlock& block) {
    如果 (!IsLocationValid(节点 Id)) 返回 假;
    如果 (nodeStakingMap.找到(节点 Id) == nodeStakingMap.结束()) 返回 假;
    常量 自动& stake = nodeStakingMap[nodeId];
    if (block.nTime - stake.stakeTime < 30) return false;
    if (CalculateStakeWeight(nodeId) < 200) return false;
 返回 true;
}
