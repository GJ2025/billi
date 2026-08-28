# 指定编译器
CXX = g++

# 在 CXXFLAGS 中添加依赖生成选项 -MMD -MP
CXXFLAGS = -g -O0 -Wall -Wextra -Wunused-variable -Wunused-but-set-variable -Wunused-function -std=c++17 -MMD -MP

# 定义目录路径
SRC_DIR = src
BIN_DIR = bin

# 最终可执行文件路径 (bin/parse_tick)
TARGET = $(BIN_DIR)/parse_tick

# 自动获取 src 目录下的所有 .cc 文件
SRCS = $(wildcard $(SRC_DIR)/*.cc)

# 将对应的 .o 文件放在与 .cc 文件相同的 src 目录下
OBJS = $(SRCS:.cc=.o)

# 自动生成对应的依赖文件列表 (.d)
DEPS = $(OBJS:.o=.d)

# 默认目标
all: $(TARGET)

# 链接规则：生成最终的可执行文件到 bin 目录
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# 编译规则：编译 src/*.cc 到 src/*.o
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 引入由编译器自动生成的依赖文件
-include $(DEPS)

# 清理规则
.PHONY: clean
clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)
	rm -rf $(BIN_DIR)
