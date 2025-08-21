@mainpage

# 🚀 DDS_Bridge  —— 像 ROS 一样用 Fast-DDS  

一个 **极简、零侵入、可扩展** 的类ROS2 Fast-DDS 通信框架，让你：  

- ✨ **一条指令** 发布 / 订阅任意 IDL 定义的数据结构  
- 🛠️ **无需手动** 编写序列化代码、PubSubType、CMake 规则  
- 🔄 **直接替换** ROS 2 的 `create_publisher / create_subscription` 语法
- 📦 **内置常用类 ROS Topic** 开箱即用的[ros common topic](https://index.ros.org/r/common_interfaces/)，零配置即可收发。
- 🌐 **与 ROS 2 Topic 完全兼容** 仅需在 Topic 名前加前缀 rt/，原 ROS 2 节点无需任何改动即可无缝互通。

---

## 1. 快速上手

### 1.1 安装依赖

**Linux:**

## 🐧 在 Linux 上安装 eProsima Fast DDS（官方二进制版）

> 仅需 3 步，10 分钟完成！

---

### 1️⃣ 下载并解压  

进入 [eProsima 官网 Downloads](https://www.eprosima.com/index.php/downloads-all) → 选择 **Fast DDS Linux 最新版** → 解压到任意目录：

```bash
tar -xf fastdds-<version>-linux.tar.gz
cd fastdds-<version>-linux
```

---

### 2️⃣ 一键安装  

运行官方脚本即可一次性装好 **Fast-DDS + 全部依赖**(请确保网络畅通)：

```bash
sudo ./install.sh
```

> Ubuntu20.04 用户可能需要手动移除安装脚本中的python3-xmlschema依赖

**Windows:**

# 🪟 Windows 安装 Fast-DDS（官方二进制版）  

2 分钟速通指南，含 VS 配置、环境变量与 CMake 链接。

---

### 1️⃣ 前置条件

| 必须 | 如何安装 |
|---|---|
| **Visual Studio 2019/2022** | 勾选 “Desktop development with C++” |

---

### 2️⃣ 一键安装

1. 下载 **Fast-DDS Windows Installer**  
   <https://www.eprosima.com/downloads>
2. 双击安装 → 选择 VS 版本 & 架构 → 勾选 **“Add to PATH”**  
3. 完成！组件一览：

安装后组件一览：

| 包名 | 作用 |
|---|---|
| `foonathan_memory_vendor` | STL 兼容内存分配器 |
| `fastcdr` | CDR 序列化库 |
| `fastdds` | Fast-DDS 核心库 |
| `fastddsgen` | 通过 IDL 生成代码的 Java 工具 |

---

### 1.2 克隆 & 编译

```bash
git clone https://github.com/ZzzzzzS/DDS_Bridge.git
cd DDS_Bridge
mkdir build && cd build
cmake .. -DBUILD_EXAMPLE=ON
cmake --build .
```

### 1.3 跑 example

编译后运行example即可，输出类似如下

```bash
."./DDS_Bridge/pose_example.exe"
Hello, Fast DDS!
Received message: Hello from My Node Fast DDS!
Received message: Hello from std node Fast DDS!
Received message: Hello from My Node Fast DDS!
Received message: Hello from std node Fast DDS!
Received message: Hello from My Node Fast DDS!
Received message: Hello from std node Fast DDS!
```

---

## 2. 目录一览

``` bash
|DDS_Bridge
│   .gitignore
│   CMakeLists.txt
│   LICENSE
│   README.md
│
├───example
│       example.cpp # 本地收发示例文件
│
├───msg # 在这里添加自定义的message type
│       geometry_msgs.idl
│       std_msgs.idl
│
└───src
        node.hpp # node类型
        publisher.hpp # publisher类型
        subscriber.hpp # subscriber类型
```

---

## 3. 开发新消息

1. **写 IDL**  
   在msg文件夹中创建新idl文件,如`msg/robot_state.idl`. idl文件语法请参阅[官方文档](https://fast-dds.docs.eprosima.com/en/latest/fastddsgen/dataTypes/dataTypes.html).

   ```idl
   module myapp
   {
       struct Point { double x; double y; };
       struct RobotState { Point pos; double yaw; };
   };
   ```

2. **重新生成与编译**

   ```bash
   cmake -S . -B ./build
   cmake --build build
   ```

3. **代码里直接用**

   ```cpp
   #include "robot_state.hpp" //robot_state idl产生的头文件
   #include "robot_statePubSubTypes.hpp" //robot_state idl产生的头文件

   auto node = std::make_shared<Node>("node");
   auto pub  = node->create_publisher<myapp::RobotState>("state", 10);
   auto sub  = node->create_subscription<myapp::RobotState>(
       "state", 10,
       [](const auto& s){ std::cout << s.pos().x() << "\n"; });

   myapp::RobotState st{};
   pub->publish(st);
   ```

---

## 4. 与ROS2节点通信

ROS2节点测完全无感知，无需任何修改
在DDS_Bridge测创建话题时在ROS2话题前加入``"rt/"``前缀，topic数据类型保持不变。例如ROS2发送``std_msgs/string``类型的数据，话题为``/test_topic``,那么DDS_Bridge接收时仅需将话题名称修改为``"rt/test_topic"``。

```c++
auto std_publisher = std_node->create_publisher<std_msgs::msg::StringPubSubType>("rt/test_topic", 10); //发送
    auto std_subscription = std_node->create_subscription<std_msgs::msg::StringPubSubType>(
        "rt/test_topic", 10,
        [](const std_msgs::msg::String& msg) {
            std::cout << "Received message: " << msg.data() << std::endl;
        }); //接收

```

## 5. API 速查

| ROS 2 习惯写法 | fastdds_comm |
|----------------|--------------|
| `create_publisher<T>` | ✅ 同上 |
| `create_subscription<T>` | ✅ 同上 |
| QoS `depth` 参数 | 已支持 |
| 回调签名 | `void(const T&)` |

---

## 6. 构建选项

| CMake 变量 | 默认值 | 说明 |
|-----------|--------|------|
| `BUILD_EXAMPLE` | `OFF` | 编译示例 |
| `BUILD_DOC` | `OFF` | 编译文档 |

---

## 7. 性能 & 限制

- **零拷贝** 发布：直接 `publish(msg&)`  
- **内存** 每个 topic 一条 `shared_ptr`  
- **线程** 每条 participant 2-3 条线程（与 ROS 2 相同）

---

## 8. 贡献 & 协议

MIT License，欢迎 PR！  
issue → GitHub Discussions。

---
EOF
