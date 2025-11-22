
# 编译 & 运行
``` shell
Remove-Item -Recurse -Force build
# 2. 配置项目
cmake -S . -B build -G "MinGW Makefiles"
# 3. 编译
cmake --build build
# 4. 运行测试
.\build\lsm_tree_test.exe
```