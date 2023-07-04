# 资源格式定义

|文件|说明|
|---|---|
|include/core/v1|core的v1版本格式定义|
|include/meta/v1|meta的v1版本格式定义|
|include/goraw/v1|部分Go语言内置类型的定义|
|test|简单的测试代码|

## 标准资源接口

```cpp
// 从Json对象解码到当前对象
bool decode(const Json & data);
// 将当前对象编码到Json对象
bool encode(Json & data) const;
// 输出当前对象的所有数据，用于调试
void inspect(const string & name = "", unsigned level = 0) const;
```

## 组合资源类型

### `ResList<T>`

当使用RESTful的List方法时，返回的数据采用ResList<T>类型

ResList继承vector类型，加入了序列化、检视等辅助接口

```cpp
template <class T>
class ResList : public vector<T> {...}
```

#### 使用方法

ResList的使用方法与vector一致

```cpp
ResList<Pod> podList;
Pod pod1;
podList.push_back(pod1); // 添加
ResList<Pod>::iterator it = find(podList.begin(), podList.end(), pod1); // 检索
for (ResList<Pod>::iterator it = podList.begin(); it != podList.end(); ++it) { ... } // 遍历
podList.erase(it); // 删除
```

### `WatchEvent<T>`

当使用RESTful的Watch方法时，返回的数据采用WatchEvent<T>类型

WatchEvent结构简单，包含事件类型和数据对象

```cpp
template <typename T>
struct WatchEvent
{
	string type; // ADDED / MODIFIED / DELETED / BOOKMARK / ERROR
	T object; // 数据对象
};
```

#### 使用方法

WatchEvent的数据直接读取使用即可

```cpp
WatchEvent<Pod> we = XXXGetEvent(); // XXXGetEvent是假设返回WatchEvent类型的方法
if (we.type == "ADDED") {
	LocalStore.Add(we.object); // LocalStore是假设的本地缓存
}
```

## 示例

### 创建Pod，设置名称，并序列化

#### 方法1 - 先转换为Json，再转换为字符串

```cpp
Pod pod1;
pod1.metadata.name = "DemoPod";

Json * json = JsonNew(JSON_DICT);
pod1.encode(*json);
char * str = JsonToStr(json);
if (str) // 编码成功
{
	printf("%s\n", str); // 输出字符串查看

	// 释放资源
	free(str);
}
JsonFree(json);
```

#### 方法2 - 直接转换为字符串

```cpp
Pod pod1;
pod1.metadata.name = "DemoPod";

vector<char> data;
if (apis::ResEncodeToJsonStr(pod1, data)) // 编码成功
{
	printf("%.*s\n", data.size(), data.data()); // 输出字符串查看
}
```

### 比较Pod是否一致

```cpp
Pod pod1, pod2;
pod1.metadata.name = "DemoPod";
pod2.metadata.name = "DemoPod";
if (pod1 == pod2)
{
	printf("pod1 equal to pod2\n");
}
```
