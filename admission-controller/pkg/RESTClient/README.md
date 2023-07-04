# RESTClient

包含RESTful客户端、资源客户端，接口类似于client-go

## RESTful客户端

示例代码参见test文件夹

### 接口

RESTClient接口包括：

|接口|说明|
|---|---|
|`RESTClient(...)`|创建RESTful客户端，需要指定服务端的IP地址、端口、基础路径（一般为空）、API版本|
|`~RESTClient()`|析构函数，不直接调用|
|`RESTRequest * Verb(int verb) const`|生成针对指定方法的请求|
|`RESTRequest * Post() const`|生成针对Post方法的请求|
|`RESTRequest * Put() const`|生成针对Put方法的请求|
|`RESTRequest * Patch(const string & patchType) const`|生成针对Patch方法的请求|
|`RESTRequest * Get() const`|生成针对Get方法的请求|
|`RESTRequest * Delete() const`|生成针对Delete方法的请求|

其中返回的RESTRequest代表依次RESTful请求，其接口包括：

|接口|说明|
|---|---|
|`RESTRequest(...)`|创建RESTful请求对象，一般不直接调用|
|`RESTRequest * Verb(int verb);`|设置HTTP方法|
|`RESTRequest * Resource(string resource);`|指定资源类型|
|`RESTRequest * SubResource(string subresource);`|指定子资源类型，默认为空|
|`RESTRequest * Name(string name);`|指定资源名称|
|`RESTRequest * NameSpace(string nameSpace);`|指定命名空间|
|`RESTRequest * NameSpaceIfScoped(string nameSpace, bool scoped);`|如果scoped为true，设置命名空间|
|`RESTRequest * Param(string param, string val);`|指定URL中的参数|
|`RESTRequest * Header(string header, string content);`|指定HTTP请求的头信息|
|`template <typename T> RESTRequest * VersionedParams(const T & options)`|将请求选项(如CreateOptions)序列化到参数|
|`RESTRequest * Timeout(zutil::adapter::TimeVal timeout);`|设置请求超时时间|
|`RESTRequest * MaxRetries(int maxRetries);`|设置最大重试次数|
|`RESTRequest * Body(vector<char> body);`|写入数据|
|`RESTRequest * Body(string body);`|写入数据|
|`string URL();`|获取最终的URL路径|
|`bool Do(HTTP::Response & resp);`|发起请求，获取HTTP回复|
|`bool Do(RESTResponse & resp);`|发起请求，获取RESTful回复|
|`bool DoRaw(vector<char> & raw);`|发起请求，获取回复Body的原始数据|
|`bool Watch(Watcher * & watcher);`|发起Watch请求，获得自动分配的Watcher对象|

通过将返回`RESTRequest *`的接口进行流式调用，完成一次请求，如：

```cpp
RESTRequest * req = client.Post();
RESTResponse resp;
CreateOptions opts;
// 指定命名空间，资源类型，子资源类型，请求选项，HTTP头，超时时间，重试次数，重试间隔，并发起请求
req->NameSpace("default")
	->Resource("Res")
	->SubResource("SubRes")
	->Param(opts)
	->Header("key", "value")
	->Timeout(TimeVal(1, 0))
	->MaxRetries(3)
	->RetryInterval(100)
	->Do(resp);
```

### 使用示例

#### GET/PUT/POST/DELETE等

使用流程：

1. 创建客户端(指定IP、端口、资源版本)，回复对象
2. 按照RESTful方法生成请求
3. 填充请求数据
4. 向指定命名空间、资源类型发起请求
5. 判断请求是否成功
6. 判断回复是否正确(200<=状态码<300)

```cpp
RESTClient client("127.0.0.1", 8080, "", "1.0"); // 创建客户端
RESTResponse resp; // 创建回复对象
RESTRequest * req = client.Post(); // 按照RESTful方法生成请求

req->Body("test post"); // 填充请求数据
// 向指定命名空间、资源类型发起请求
if (!req->NameSpace("default")->Resource("pod")->Do(resp))
{
	// 请求失败
	printf("failed\n");
}
else
{
	// 请求成功
	printf("received\n");
	resp.inspect(); // 打印回复信息
}
// 清除请求对象
delete req;
```

#### WATCH

使用流程：

1. 创建客户端，回复对象，Watcher指针
2. 按照RESTful方法生成请求
3. 向指定命名空间、资源类型发起Watch请求
4. 判断请求是否成功
5. 循环从Watcher接收事件并处理

```cpp
RESTClient client("127.0.0.1", 8080, "", "1.0"); // 创建客户端
RESTResponse resp; // 创建回复对象
Watcher * watcher; // 创建Watcher指针

RESTRequest * req = client.Get(); // 按照RESTful方法生成请求

if (!req->NameSpace("default")->Resource("pod")->Watch(watcher)) // 向指定命名空间、资源类型发起Watch请求
{
	// 请求失败
	printf("failed\n");
}
else
{
	// 请求成功
	vector<char> data;
	while (watcher->take(data)) { // 从Watcher中取数据，如果返回失败表示连接断开，watcher自动清除
		printf("received: %.*s", data.size(), data.data());
	}
}
delete req; // 删除请求对象
```

## 资源客户端

资源客户端是基于RESTClient，针对具体资源对象的封装实现

此处以PodClient为例，介绍资源客户端的接口：

|接口|说明|
|---|---|
|`PodClient(_IN_ const Net::RESTful::RESTClient * clnt, _IN_ const string & ns);`|创建Pod客户端|
|`bool Create(_IN_ Pod & obj, _IN_ CreateOptions opts, _OUT_ Pod & targetObj);`|创建Pod|
|`bool Update(_IN_ Pod & obj, _IN_ UpdateOptions opts, _OUT_ Pod & targetObj);`|更新Pod|
|`bool UpdateStatus(_IN_ Pod & obj, _IN_ UpdateOptions opts, _OUT_ Pod & targetObj);`|更新Pod状态|
|`bool Delete(_IN_ string name, _IN_ DeleteOptions opts);`|删除指定名称的Pod|
|`bool DeleteCollection(_IN_ DeleteOptions opts, _IN_ ListOptions listOpts);`|删除集合|
|`bool Get(_IN_ string name, _IN_ GetOptions opts, _OUT_ Pod & targetObj);`|获取指定名称的Pod|
|`bool List(_IN_ ListOptions opts, _OUT_ apis::ResList<Pod> & targetObj);`|列出Pod|
|`bool Watch(_IN_ ListOptions opts, _OUT_ Net::RESTful::Watcher * & watcher);`|对Pod变动进行Watch|
|`bool Patch(_IN_ string name, _IN_ const string & patchType, _IN_ vector<char> data, _IN_ PatchOptions opts, _IN_ string subresources, _OUT_ Pod & targetObj);`|对Pod进行Patch|

### 使用示例

#### Get

```cpp
CoreV1Client coreClnt("127.0.0.1", 8080);
Pod pod;
if (coreClnt.Pods("default")->Get("podForGet", GetOptions(), pod)) {
	// 获取成功，输出Pod内容
	pod.inspect();
}
```

#### Watch

```cpp
CoreV1Client coreClnt("127.0.0.1", 8080);
WatchEvent<Pod> we;
Watcher * watcher;

if (!coreClnt.Pods("default")->Watch(ListOptions(), watcher)) {
	// Watch失败
	printf("failed\n");
} else {
	vector<char> data;
	while (watcher->take(data)) { // take失败表示连接断开，watcher自动删除
		if (!apis::ResDecodeFromJsonStr(we, data)) {
			// 解析失败
			printf("watch failed\n");
		} else {
			// 打印事件内容
			printf("watch: [TYPE]%s, [NAME]%s\n", we.type.c_str(), we.object.metadata.name.c_str());
		}
	}
}
```

## 资源存储(UndeltaStore/FIFO/DeltaFIFO)

资源存储对象用于记录远端对象的变化，并存储数据，一般搭配Reflector使用

三种资源存储方式分别具有以下特征：
* UndeltaStore: 存储所有资源对象的数据，并在每次变化时，通过用户注册的PushFunc函数，将全部资源推送给用户
* FIFO: 存储资源对象的最后一次变化的数据，如果删除则不会记录，并清除该对象未被处理的数据
* DeltaFIFO: 存储每一次变化的动作(添加/删除/更新)，以及对象的数据，提供pop接口主动获取其数据

**三种存储方式在具体数据变化时的行为对比**

| -            | UndeltaStore                               | FIFO                                   | DeltaFIFO                  |
| ---          | ---                                        | ---                                    | ---                         |
| 添加         | 添加对象数据                               | 记录该对象的最新数据，删除未处理的数据 | 记录Delta(添加，对象数据)   |
| 删除         | 删除对象数据                               | 删除该对象的所有未处理数据             | 记录Delta(删除，对象数据)   |
| 更新         | 更新对象数据                               | 记录该对象的最新数据，删除未处理的数据 | 记录Delta(更新，对象数据)   |
| 获取数据接口 | 每次变更通过用户注册的PushFunc推送所有数据 | 用户使用pop接口主动阻塞获取            | 用户使用pop接口主动阻塞获取 |

## 资源映射(ListerWatcher/Reflector/Informer)

### ListerWatcher

仅用于记录指定资源的List方法和Watch方法

### Reflector

Reflector通过注册的ListerWatcher，自动与ApiServer同步指定资源的数据，并在通信故障时，自动重试

Reflector内部维护了资源的版本，从而确保List/Watch机制对资源获取的连续性

#### 示例

```cpp
CoreV1Client coreClnt("127.0.0.1", 8080);

bool listPodDemo(ListOptions & opts, ResList<Pod> & resList) {
	return coreClnt.Pods("default")->List(opts, resList);
}

bool watchPodDemo(ListOptions & opts, shared_ptr<Watcher> & watcher) {
	return coreClnt.Pods("default")->Watch(opts, watcher);
}

int main() {
	// 创建资源对象的存储方法，ListerWatcher接口
	UndeltaStore<Pod> podStore(podPushFunc, podKeyFunc);
	ListerWatcher<Pod> lw(listPodDemo, watchPodDemo);
	// 创建Reflector对象，并启动
	Reflector<Pod> podReflector("podReflector", &podStore, lw);
	podReflector.runWithThread();
	// 之后即可通过资源存储提供的PushFunc/pop接口随时获取资源的最新状态
}
```

### Informer

SharedInformerFactory/SharedIndexInformer用于在多个使用相同资源的用户间进行数据共享，如节点控制器、副本控制器都需要监听节点信息，可以使用同一个Informer

Informer支持通过Lister对象导出本地缓存数据

```cpp
	// 获取共享Informer
	SharedInformerFactory * factory = getSharedInformerFactory();
	SharedIndexInformer<Pod> * podInformer = (SharedIndexInformer<Pod> *)factory->getInformer("pod");
	// 注册回调函数
	// 在所有用户注册完成后，启动factory
	factory->start();
	// 之后可以通过回调获取最新资源数据，或通过Lister手动获取所有数据
	Lister<Pod> * podLister = podInformer->getLister();
	podLister->listObj(...);
```

为了确保某个资源处理过程中，获取的其他资源也是最新状态，一般应在所有资源的Informer生成后，进行统一同步，同步结束后再推送变更数据，该机制由SharedInformerFactory::start提供，内部实现如下所示:

```cpp
// 开始同步
for (map<string, SharedIndexInformerBase *>::iterator it = informerMap.begin(); it != informerMap.end(); ++it)
	it->second->startSync();
// 等待同步
for (map<string, SharedIndexInformerBase *>::iterator it = informerMap.begin(); it != informerMap.end(); ++it)
	it->second->waitSynced();
// 开始推送数据
for (map<string, SharedIndexInformerBase *>::iterator it = informerMap.begin(); it != informerMap.end(); ++it)
	it->second->runWithThread();
```

在此过程中，Informer与Reflector的调用关系如下：

- Informer::startSync(更新本地Indexer) -> Reflector::runWithThread -> 完成List，设置synced = true -> Reflector::waitStarted, 等待started = true
- Informer::waitSynced -> Reflector::waitSynced, 等待synced = true
- Informer::runWithThread -> Reflector::start, 设置started = true, 开始正常listAndWatch循环
