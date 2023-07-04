#include "ztest/ztest.h"
#include "ejson/include/Json.h"

// #include "core/v1/TestResObj.h"
#include "goraw/types.h"
#include "core/v1/types.h"
#include "meta/v1/types.h"
#include "utils/metaV1Helper.h"
using namespace apis::goraw;
using namespace apis::core::v1;
using namespace apis::meta::v1;
using namespace apis::utils;

// 0, 正常, 1, encode错误, 2, decode错误, 3, 转换前后不一致, -1, 其他无关错误
template<typename T>
int Resource_Tranform_Test(T & q)
{
	T otherQ;
	Json * resJson = JsonNew(JSON_DICT);
	if (!resJson) return -1;
	if (!q.encode(*resJson)) return JsonFree(resJson), 1;
	if (!otherQ.decode(*resJson)) return JsonFree(resJson), 2;
	if (!(q == otherQ)) return JsonFree(resJson), 3;
	return JsonFree(resJson), 0;
}

#define LSM_CHECK(cond) if (!(cond)) { printf("[ERROR:%d] TestLabelSelectorMatcher failed\n", __LINE__); return false; }
bool TestLabelSelectorMatcher(LabelSelector ls, map<string, string> label, bool expect)
{
	LabelSelectorMatcher lsm, lsm_copy;
	LSM_CHECK(lsm.parse(ls));
	// test equal
	LSM_CHECK(lsm_copy.parseStr(lsm.toStr()));
	LSM_CHECK(lsm_copy == lsm);
	// test match map
	LSM_CHECK(lsm.matchLabelMap(label) == expect);
	// test match vector<char>
	Pod pod;
	pod.metadata.labels = label;
	vector<char> data;
	LSM_CHECK(apis::ResEncodeToJsonStr(pod, data));
	LSM_CHECK(lsm.matchData(data) == expect);
	// test match json
	shared_ptr<Json> json(JsonCreateFromStr(data.data()), JsonFree);
	LSM_CHECK(json);
	LSM_CHECK(lsm.matchJson(json.get()) == expect);
	return true;
}

LabelSelector LabelSelectorGenerate(string key, string op)
{
	LabelSelector ls;
	LabelSelectorRequirement lsr;
	lsr.key = key;
	lsr.op = op;
	ls.matchExpressions.push_back(lsr);
	return ls;
}

ZTEST_GROUP(ResourceType, NULL, NULL) {
	ZTEST_CASE(GetResName) {
		Pod pod;
		// getResName(bool lower = false, bool multi = false);
		ZTEST(pod.getResName() == "Pod")
		ZTEST(pod.getResName(false, true) == "Pods")
		ZTEST(pod.getResName(true) == "pod")
		ZTEST(pod.getResName(true, true) == "pods")
	} ZTEST_CASE_END;
	ZTEST_CASE(NodeCondition_Time_null) {
		string jsonStr = "{\"type\":\"Ready\",\"status\":\"Unknown\",\"lastHeartbeatTime\":null,\"lastTransitionTime\":\"2022-02-28T05:34:47Z\",\"reason\":\"NodeStatusUnknown\",\"message\":\"Kubelet stopped posting node status.\"}";
		vector<char> data;
		data.insert(data.end(), jsonStr.begin(), jsonStr.end());

		NodeCondition cond;
		apis::ResDecodeFromJsonStr(cond, data);
		ZASSERT(apis::EqualDefault(cond.lastHeartbeatTime));
	} ZTEST_CASE_END;
	ZTEST_CASE(Pod---Json) {
		Pod pod1, pod2;
		pod1.metadata.name = "Pod1";
		vector<char> data;
		apis::ResEncodeToJsonStr(pod1, data);
		apis::ResDecodeFromJsonStr(pod2, data);
		ZASSERT(pod1 == pod2);
		ZASSERT(pod2.metadata.name == "Pod1");
	} ZTEST_CASE_END;
	ZTEST_CASE(Container---Json) {
		Container cont1, cont2;
		cont1.name = "cont";
		cont1.image = "image";
		cont1.command.push_back("cmd1");
		cont1.args.push_back("arg1");
		cont1.workingDir = "workingDir";
		vector<char> data;
		apis::ResEncodeToJsonStr(cont1, data);
		apis::ResDecodeFromJsonStr(cont2, data);
		ZASSERT(cont1 == cont2);
		ZASSERT(cont2.name == "cont");
		ZASSERT(cont2.image == "image");
	} ZTEST_CASE_END;
	ZTEST_CASE(WatchEvent---Json) {
		Pod pod;
		pod.metadata.name = "Pod1";
		WatchEvent<Pod> we1, we2;
		we1.type = "ADDED";
		we1.object = pod;
		vector<char> data;
		apis::ResEncodeToJsonStr(we1, data);
		apis::ResDecodeFromJsonStr(we2, data);
		ZASSERT(we1 == we2);
		ZASSERT(we2.object.metadata.name == "Pod1");
	} ZTEST_CASE_END;
	ZTEST_CASE(ResList---Json) {
		Pod pod1, pod2;
		pod1.metadata.name = "Pod1";
		pod2.metadata.name = "Pod2";
		PodList list1, list2;
		list1.push_back(pod1);
		list1.push_back(pod2);
		list1.metadata.resourceVersion = "1";
		vector<char> data;
		apis::ResEncodeToJsonStr(list1, data);
		apis::ResDecodeFromJsonStr(list2, data);
		ZASSERT(list1 == list2);
		ZASSERT(list2.metadata.resourceVersion == "1");
		ZASSERT(list2.size() == 2 && list2[0] == pod1 && list2[1] == pod2);
	} ZTEST_CASE_END;
	ZTEST_CASE(RestOptions---Map) {
		ListOptions listOp1, listOp2;
		listOp1.watch = true;
		map<string, string> param;
		listOp1.encode(param);
		listOp2.decode(param);
		ZASSERT(listOp1 == listOp2);
		ZASSERT(listOp2.watch == true);
	} ZTEST_CASE_END;
	ZTEST_CASE(Quantity---Json) {
		Quantity q1, q2;
		q1.i.value = 11111;
		q1.i.scale = 6;
		q1.format = Quantity::QF_DecimalSI;
		// q1.inspect();
		ZTEST(0 == Resource_Tranform_Test<Quantity>(q1));
		for (int i = 0; i < 20; ++i)
			q2.d->absValue.push_back('9');
		q2.d->neg = true;
		q2.d->scale = 3;
		q2.format = Quantity::QF_BinarySI;
		// q2.inspect();
		ZTEST(0 == Resource_Tranform_Test<Quantity>(q2));
	} ZTEST_CASE_END;
	ZTEST_CASE(Quantity---String) {
		string str = "11111M";
		Quantity q1, q2;
		q1.parseStr(str);
		ZASSERT(q1.toStr() == str);
	} ZTEST_CASE_END;
	ZTEST_CASE(IntOrString---Json) {
		IntOrString intVal, strVal, intStrVal;
		intVal.type = IntOrString::INT;
		intVal.intValue = 99;
		strVal.type = IntOrString::STRING;
		strVal.strValue = "aaa";
		intStrVal.type = IntOrString::STRING;
		intStrVal.strValue = "99";
		ZTEST(0 == Resource_Tranform_Test<IntOrString>(intVal));
		ZTEST(0 == Resource_Tranform_Test<IntOrString>(strVal));
		ZTEST(0 == Resource_Tranform_Test<IntOrString>(intStrVal));
		ZTEST(intVal == intStrVal);
	} ZTEST_CASE_END;
	ZTEST_CASE(IntOrString---Pod) {
		Pod pod;
		pod.metadata.name = "podDemoForIntOrStringTest";
		Container cont;
		pod.spec.containers.push_back(cont);
		HTTPGetAction & act = *(cont.livenessProbe->httpGet);
		act.port.type = IntOrString::INT;
		act.port.intValue = 80;
		ZTEST(0 == Resource_Tranform_Test<Pod>(pod));
	} ZTEST_CASE_END;
	ZTEST_CASE(ConditionTime) {
		NodeCondition cond;
		cond.lastHeartbeatTime = Time::now();
		ZASSERT(Resource_Tranform_Test(cond) == 0);
	} ZTEST_CASE_END;
	ZTEST_CASE(Node---JSON) {
		Node node, nodeUnschedulable;
		nodeUnschedulable.spec.unschedulable = true;
		ZTEST(0 == Resource_Tranform_Test<Node>(node));
		ZTEST(0 == Resource_Tranform_Test<Node>(nodeUnschedulable));
	} ZTEST_CASE_END;
	ZTEST_CASE(ResDiff) {
		printf("Run TestCase [ResDiff]\n");
		// TODO: 支持其他资源类型
		Time t1, t2;
		t1.s = 1;
		t2.s = 2;
		t1.diffInspect(t2, "time");
	} ZTEST_CASE_END;
	ZTEST_CASE(LabelSelectorMatcher) {
		printf("Run TestCase [LabelSelectorMatcher]\n");
		// test Label
		{
			LabelSelector ls;
			ls.matchLabels["key"] = "val";
			map<string, string> matchMap;
			matchMap["key"] = "val";
			ZASSERT(TestLabelSelectorMatcher(ls, matchMap, true));
			matchMap["key"] = "value";
			ZASSERT(TestLabelSelectorMatcher(ls, matchMap, false));
		}
		// test LabelSelectorRequirement
		{
			// In
			LabelSelector ls = LabelSelectorGenerate("key", "In");
			ls.matchExpressions[0].values.push_back("val");
			map<string, string> matchMap;
			matchMap["key"] = "val";
			ZASSERT(TestLabelSelectorMatcher(ls, matchMap, true));
			matchMap["key"] = "value";
			ZASSERT(TestLabelSelectorMatcher(ls, matchMap, false));
		}
		{
			// NotIn
			LabelSelector ls = LabelSelectorGenerate("key", "NotIn");
			ls.matchExpressions[0].values.push_back("val");
			map<string, string> matchMap;
			matchMap["key"] = "val";
			ZASSERT(TestLabelSelectorMatcher(ls, matchMap, false));
			matchMap["key"] = "value";
			ZASSERT(TestLabelSelectorMatcher(ls, matchMap, true));
		}
		{
			// Exists
			LabelSelector ls = LabelSelectorGenerate("key", "Exists");
			map<string, string> matchMap;
			ZASSERT(TestLabelSelectorMatcher(ls, matchMap, false));
			matchMap["key"] = "";
			ZASSERT(TestLabelSelectorMatcher(ls, matchMap, true));
		}
		// {
		// 	// DoesNotExists
		// 	LabelSelector ls = LabelSelectorGenerate("key", "DoesNotExist");
		// 	map<string, string> matchMap;
		// 	ZASSERT(TestLabelSelectorMatcher(ls, matchMap, true));
		// 	matchMap["key"] = "";
		// 	ZASSERT(TestLabelSelectorMatcher(ls, matchMap, false));
		// }
		{
			// StrWithUrlChar
			LabelSelectorMatcher ls;
			ls.parseStr("!service.kubernetes.io%2Fheadless%2C!service.kubernetes.io%2Fservice-proxy-name");
			map<string, string> matchMap;
			ZASSERT(ls.matchLabelMap(matchMap));
			matchMap["service.kubernetes.io/headless"] = "";
			matchMap["service.kubernetes.io/service-proxy-name"] = "";
			ZASSERT(!ls.matchLabelMap(matchMap));
		}
	} ZTEST_CASE_END;
	ZTEST_CASE(FieldSelectorMatcher) {
		printf("Run TestCase [FieldSelectorMatcher]\n");
		FieldSelectorMatcher fsEqualNode1, fsNotEqualNode1;
		fsEqualNode1.parseStr("metadata.name=node1");
		fsNotEqualNode1.parseStr("metadata.name!=node1");
		Node node1, node2;
		node1.metadata.name = "node1";
		node2.metadata.name = "node2";
		vector<char> nodeData1, nodeData2;
		ZASSERT(apis::ResEncodeToJsonStr(node1, nodeData1));
		ZASSERT(apis::ResEncodeToJsonStr(node2, nodeData2));
		ZASSERT(fsEqualNode1.matchData(nodeData1));
		ZASSERT(!fsEqualNode1.matchData(nodeData2));
		ZASSERT(fsNotEqualNode1.matchData(nodeData2));
		ZASSERT(!fsNotEqualNode1.matchData(nodeData1));
	} ZTEST_CASE_END;
} ZTEST_GROUP_END;

#ifdef ACOREOS
extern "C" int Init()
#else
int main()
#endif
{
	ZTEST_RUN_GROUP(ResourceType, stdout);
	return 0;
}
