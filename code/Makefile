# 指定编译器
CXX = g++

# 指定编译选项 (★ 这里把 -std=c++11 修改为了 -std=c++17)
CXXFLAGS = -O3 -Wall -std=c++17

# 定义目录路径
SRC_DIR = src
BIN_DIR = bin

# 最终可执行文件路径 (bin/parse_tick)
TARGET = $(BIN_DIR)/parse_tick

# 自动获取 src 目录下的所有 .cc 文件
SRCS = $(wildcard $(SRC_DIR)/*.cc)

# 将对应的 .o 文件放在与 .cc 文件相同的 src 目录下
OBJS = $(SRCS:.cc=.o)

# 默认目标
all: $(TARGET)

# 链接规则：生成最终的可执行文件到 bin 目录
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# 编译规则：编译 src/*.cc 到 src/*.o
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理规则
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(BIN_DIR)
