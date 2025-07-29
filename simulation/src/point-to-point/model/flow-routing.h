#ifndef FLOW_ROUTING_H
#define FLOW_ROUTING_H 
// 头文件保护宏，防止多次包含同一个头文件导致重复定义。
// 头文件保护宏的主要作用是防止头文件被多次包含。如果一个头文件没有保护宏，当它被多个源文件（.cpp 文件）或被其他头文件多次 #include 时，其中的定义（如结构体、类、函数声明等）会被重复处理，导致编译器报错

// 用于定义哈希表（字典），存储 flow 到路径的映射。
#include <unordered_map>
// 引入固定大小的整数类型（如 uint32_t, uint8_t 等）。
#include <cstdint>
#include <vector>

// Flow key structure for packet
struct FlowKey {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint8_t protocol;
    uint16_t src_port;
    uint16_t dst_port;

    // 部分	含义
    // bool	返回值类型，返回 true 或 false 表示两个对象是否相等
    // operator==	要重载的操作符名称 —— ==
    // (const FlowKey& other)	参数列表，表示传入另一个 FlowKey 对象进行比较，使用 const 和引用是为了避免拷贝且防止修改原对象
    // const	成员函数末尾的 const 表示这个函数不会修改当前对象的状态
    // { ... }	函数体，定义了如何判断两个对象是否相等
    bool operator==( const FlowKey& other ) const {
        return src_ip == other.src_ip &&
               dst_ip == other.dst_ip &&
               protocol == other.protocol &&
               src_port == other.src_port &&
               dst_port == other.dst_port;
    }
};

// Hash function for FlowKey
struct FlowKeyHash {
    // std::size_t	返回值类型，表示返回一个无符号整数，用于作为哈希值
    // operator()	表示这是一个函数调用操作符重载，使该结构体可“像函数一样使用”
    // (const FlowKey& key)	接收一个常量引用参数，避免拷贝且防止修改原对象
    // const	成员函数末尾的 const，表示不会修改当前对象的状态
    std::size_t operator()(const FlowKey& key) const { 
        // 定义一个函数调用操作符重载，使结构体可当作哈希函数使用
        std::size_t h1 = std::hash<uint32_t>()(key.src_ip);
        std::size_t h2 = std::hash<uint32_t>()(key.dst_ip);
        std::size_t h3 = std::hash<uint8_t>()(key.protocol);
        std::size_t h4 = std::hash<uint16_t>()(key.src_port);
        std::size_t h5 = std::hash<uint16_t>()(key.dst_port);
        // 将各个字段的哈希值进行异或操作，并通过位移来调整权重，生成最终的哈希值。
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4);
    }
};

// Global variables for flow routing
// extern 关键字用于声明全局变量，表示这些变量在其他文件中定义，当前文件只是引用。
// extern告诉编译器：“这个变量/函数存在，但不在这个文件里，链接时会在其他地方找到它。”
// global_flow_to_path_map是一个哈希表，用于存储 FlowKey 到路径的映射
extern std::unordered_map<FlowKey, uint32_t, FlowKeyHash> global_flow_to_path_map;
// 控制是否启用自定义路由
extern bool global_use_custom_routing;

// Function to extract flow key from packet 
// 从数据包中提取 FlowKey
FlowKey ExtractFlowKeyFromPacket(uint32_t src_ip, uint32_t dst_ip, uint8_t protocol, uint16_t src_port, uint16_t dst_port);

// Function to lookup flow path 查找流的路径
int LookupFlowPath(uint32_t src_ip, uint32_t dst_ip, uint8_t protocol, uint16_t src_port, uint16_t dst_port);

// Function to print routing statistics 打印路由统计信息
void PrintRoutingStatistics();

// Function to reset statistics 重置路由统计信息
void ResetRoutingStatistics();

// 设置和清除全局的流映射和路由配置
// Interface functions for frontend to backend communication
// std::unordered_map 是 C++ 标准库中的一个关联容器，用于存储 键值对（key-value pairs），并基于哈希表实现快速查找。
// 它的模板参数可以有 多个，但最常用的是前 三个：
// std::unordered_map<Key, T, Hash>
void SetGlobalFlowMap(const std::unordered_map<FlowKey, uint32_t, FlowKeyHash>& flow_map);
void SetGlobalCustomRouting(bool enable);
void ClearGlobalFlowMap();

// Direct access to global variables (for frontend use)
//  extern "C" 的作用
// 目的：实现C++与C的混合编程。
// 用extern "C"包裹的函数，其符号名保持C风格，确保C代码可调用。
extern "C" {
    void SetGlobalFlowMapDirect(const FlowKey* keys, const uint32_t* values, size_t size);
    void SetGlobalCustomRoutingDirect(bool enable);
}


// Frontend function declarations (implemented in common.h)
void EnableCustomRouting(bool enable);
void PrecalculateFlowPaths();
void ApplyCustomFlowOverrides();
void SyncFlowPathsWithBackend();
void ClearFlowPaths();
// void SyncFlow


#endif // FLOW_ROUTING_H