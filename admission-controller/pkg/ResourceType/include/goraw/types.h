#ifndef TYPES_H_PW1HFVMQ
#define TYPES_H_PW1HFVMQ

#include "Marshal.h"
#include "zutil/adapter/TimeAdapter.h"

namespace apis { namespace goraw {

typedef string UID;

struct RawExtension {
	// general functions
	RawExtension();
	~RawExtension();

	MARSHAL_JSON;
	void inspect(const string & name = "", unsigned level = 0) const;
	bool operator==(const RawExtension & other) const;

	string getKind();
	string getApiVersion();

	void * typeMeta;
	vector<char> jsonData;
};

struct IntOrString {
	// general functions
	IntOrString(int type = INT, long long intValue = 0, string strValue = "");

	enum {INT, STRING};

	int type;
	long long intValue;
	string strValue;

	MARSHAL_JSON;
	void inspect(const string & name = "", unsigned level = 0) const;
	bool operator==(const IntOrString & other) const;
	string toString() const;
};

struct Time
{
	// general functions
	Time(long long s = 0, long long ns = 0, int loc = 0);
	static string getResName();

	MARSHAL_JSON;
	void inspect(const string & name = "", unsigned level = 0) const;
	void diffInspect(const Time & other, const string & name, unsigned level = 0) const;
	bool operator==(const Time & other) const;

	static Time now();

	bool isZero();
	bool after(const Time & otherTime);
	bool before(const Time & otherTime);
	Time add(const long long & s, const long long & ns = 0);
	Time addMs(const long long & ms);
	Time subMs(const long long & ms);
	long long sub(const Time & otherTime);
	operator zutil::adapter::TimeVal() const;
	string toString() const;

	//!< 自1970年1月1日以来的秒数
	long long s;
	//!< 纳秒数，0-999,999,999
	long long ns;
	//!< 时区，默认为+8
	int loc;
};

typedef Time Timestamp;

struct Duration
{
	// general functions
	Duration();
	static string getResName();

	MARSHAL_JSON;
	void inspect(const string & name = "", unsigned level = 0) const;
	bool operator==(const Duration & other) const;

	//!< 自1970年1月1日以来的秒数
	long long num;

	string unit;
};

template <typename T>
struct WatchEvent
{
	// general functions
	WatchEvent() :
		type(DefaultValue< string >()),
		object(DefaultValue< T >())
	{}

	bool encode(Json & data) const {
		RES_ENCODE_JSON(data, false, "type", this->type);
		RES_ENCODE_JSON(data, false, "object", this->object);
		return true;
	}
	bool decode(const Json & data) {
		RES_DECODE_JSON(data, "type", this->type);
		RES_DECODE_JSON(data, "object", this->object);
		return true;
	}
	void inspect(const string & name = "", unsigned level = 0) const {
		InspectIndent(level);
		printf("%s[WatchEvent]:\n", name.c_str());
		InspectValue("type", this->type, level + 1);
		InspectValue("object", this->object, level + 1);
	}
	bool operator==(const WatchEvent & other) const {
		return this->type == other.type
			&& this->object == other.object;
	}

	// Value: ADDED / MODIFIED / DELETED / BOOKMARK / ERROR
	string type; // `json:"type"`
	T object; // `json:"object"`
};

struct Quantity
{
	struct IntAmount
	{
		IntAmount(long long value = 0, int scale = 0);
		long long value;
		int scale;
		bool operator==(const IntAmount & other) const;
	};

	struct InfAmount
	{
		InfAmount(vector<char> absValue = vector<char>(), int scale = 0, bool neg = false);
		vector<char> absValue;
		int scale;
		bool neg;
		bool operator==(const InfAmount & other) const;
	};

	enum Format { QF_DecimalExponent, QF_BinarySI, QF_DecimalSI };

	// general functions
	Quantity(int format = QF_BinarySI);
	Quantity(const IntAmount & intAmount, int format = QF_BinarySI);
	Quantity(const InfAmount & infAmount, int format = QF_BinarySI);

	MARSHAL_JSON;
	void inspect(const string & name = "", unsigned level = 0) const;
	bool operator==(const Quantity & other) const;

	/**
	 * @brief parseStr 解析数值字符串
	 *
	 * @param [in]    str 数值字符串
	 *
	 * @return true, 解析成功; false, 解析失败
	 */
	bool parseStr(_IN_ const string & str);
	/**
	 * @brief toStr 将Quantity转换为数值字符串
	 *
	 * @return 数值字符串
	 */
	string toStr() const;

	long long milliValue() const;

	IntAmount i; // 用于存储32/64位支持操作的整数(如果d为空)
	shared_ptr<InfAmount> d; // 用于存储无限大的整数
	string s; // 用于存储数字的字符串，避免重复运算
	int format; // 用于存储数值格式
	// DecimalExponent, e.g., 12e6
	// BinarySI       , e.g., 12Mi (12 * 2^20)
	// DecimalSI      , e.g., 12M  (12 * 10^6)
};

}}

#endif /* end of include guard: TYPES_H_PW1HFVMQ */
