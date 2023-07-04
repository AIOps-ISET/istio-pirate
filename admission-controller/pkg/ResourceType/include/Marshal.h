#ifndef MARSHAL_H_HU1NDXVW
#define MARSHAL_H_HU1NDXVW

#include <sstream>
#include <string>
#include <vector>
#include <map>
using std::stringstream;
using std::string;
using std::vector;
using std::map;

#include <stdio.h>

#include "zutil/use_boost.h"
#include "zutil/base/Logger.h"
using zutil::base::Logger;

#include "Json.h"

namespace apis {

#define MARSHAL_JSON_TYPE Json
#define MARSHAL_JSON \
	bool decode(const MARSHAL_JSON_TYPE & data); \
	bool encode(MARSHAL_JSON_TYPE & data) const;
#define MARSHAL_MAP_TYPE map<string, string>
#define MARSHAL_MAP \
	bool decode(const MARSHAL_MAP_TYPE & data); \
	bool encode(MARSHAL_MAP_TYPE & data) const;

// DefauleValue Create And Compare {{{1
template <typename T>
inline T DefaultValue();

inline void DefaultValueSet(long & v)           { v = ~(long)0;                        }
inline void DefaultValueSet(long long & v)      { v = ~(long long)0;                   }
inline void DefaultValueSet(short & v)          { v = ~(short)0;                       }
inline void DefaultValueSet(int & v)            { v = ~(int)0;                         }
inline void DefaultValueSet(double & v)         { v = ~(long)0;                        }
inline void DefaultValueSet(float & v)          { v = ~(short)0;                       }
#ifdef ACOREOS
inline void DefaultValueSet(bool & v) __attribute__((optimize("O0")));
#endif
inline void DefaultValueSet(bool & v)           { *(char *)&v = -1;                    }
inline void DefaultValueSet(string & v)         { v.clear();                           }
template <typename T>
inline void DefaultValueSet(T & v)              { v = T();                             }
template <typename V>
inline void DefaultValueSet(map<string, V> & v) { v.clear();                           }
template <typename V>
inline void DefaultValueSet(vector<V> & v)      { v.clear();                           }
template <typename PT>
inline void DefaultValueSet(shared_ptr<PT> & v) { v.reset(new PT(DefaultValue<PT>())); }

template <typename T>
inline T DefaultValue() { T t; DefaultValueSet(t); return t; }

// Check equal to default {{{2
template <typename T>
inline bool EqualDefault(const T & val) { return val == DefaultValue<T>(); }
template <typename PT>
inline bool EqualDefault(const shared_ptr<PT> & val) { return val == shared_ptr<PT>() || *val == DefaultValue<PT>(); }
template <typename T>
inline bool NotEqualDefault(const T & val) { return !(val == DefaultValue<T>()); }
template <typename PT>
inline bool NotEqualDefault(const shared_ptr<PT> & val) { return val != shared_ptr<PT>() && !(*val == DefaultValue<PT>()); }

template<> inline bool EqualDefault   <float> (const float  & val) { float  dv = DefaultValue<float >(); return val - dv < 0.0001 && dv - val < 0.0001; }
template<> inline bool EqualDefault   <double>(const double & val) { double dv = DefaultValue<double>(); return val - dv < 0.0001 && dv - val < 0.0001; }
template<> inline bool NotEqualDefault<float> (const float  & val) { float  dv = DefaultValue<float >(); return val - dv > 0.0001 || dv - val > 0.0001; }
template<> inline bool NotEqualDefault<double>(const double & val) { double dv = DefaultValue<double>(); return val - dv > 0.0001 || dv - val > 0.0001; }

template <typename T>
inline T GetValue(const T & val) { return val; }
template <typename PT>
inline PT GetValue(const shared_ptr<PT> & val) {
	if (NotEqualDefault(val))
		return *val;
	else
		return DefaultValue<PT>();
}
// }}}1

// Json Marshal Helper {{{1
void JsonStr2HTTPStr(const vector<char> & json, vector<char> & http);
void HTTPStr2JsonStr(const vector<char> & http, vector<char> & json);

// Encode {{{2
// Add to json {{{3
inline Json * JsonAdd(Json * node, const string & key, const long         & value) { return JsonAddNodeInteger(node, key.c_str(), value)         ; }
inline Json * JsonAdd(Json * node, const string & key, const long long    & value) { return JsonAddNodeInteger(node, key.c_str(), value)         ; }
inline Json * JsonAdd(Json * node, const string & key, const short        & value) { return JsonAddNodeInteger(node, key.c_str(), value)         ; }
inline Json * JsonAdd(Json * node, const string & key, const int          & value) { return JsonAddNodeInteger(node, key.c_str(), value)         ; }
inline Json * JsonAdd(Json * node, const string & key, const double       & value) { return JsonAddNodeFloat  (node, key.c_str(), value, 3)      ; }
inline Json * JsonAdd(Json * node, const string & key, const float        & value) { return JsonAddNodeFloat  (node, key.c_str(), value, 3)      ; }
inline Json * JsonAdd(Json * node, const string & key, const bool         & value) { return JsonAddNodeBoolean(node, key.c_str(), value)         ; }
inline Json * JsonAdd(Json * node, const string & key, const string       & value) { return JsonAddNodeString (node, key.c_str(), value.c_str()) ; }
inline Json * JsonAdd(Json * node, const string & key, const vector<char> & value) {
	string str(value.data(), value.size());
	return JsonAdd(node, key, str);
}
// inline Json * JsonAdd(Json * node, const string & key, const char   * value) { return JsonAddNodeString (node, key.c_str(), value)        ; }

template <typename T>
inline Json * JsonAdd(Json * node, const string & key, const T & value) {
	Json * newNode = JsonNew(JSON_DICT);
	if (!newNode)
		return NULL;
	if (!value.encode(*newNode)) {
		delete newNode;
		newNode = NULL;
	}
	if (newNode && !JsonAddNodeJson(node, key.c_str(), newNode)) {
		delete newNode;
		newNode = NULL;
	}
	return newNode;
}
template <typename V>
inline Json * JsonAdd(Json * node, const string & key, const vector<V> & value) {
	Json * newNode = JsonNew(JSON_SET);
	if (!newNode)
		return NULL;
	for (typename vector<V>::const_iterator it = value.begin(); it != value.end(); ++it) {
		if (!JsonAdd(newNode, "", *it)) {
			delete newNode;
			newNode = NULL;
		}
	}
	if (newNode && !JsonAddNodeJson(node, key.c_str(), newNode)) {
		delete newNode;
		newNode = NULL;
	}
	return newNode;
}

template <typename PT>
inline Json * JsonAdd(Json * node, const string & key, const shared_ptr<PT> & value) {
	return JsonAdd(node, key, *value);
}

template <typename V>
inline Json * JsonAdd(Json * node, const string & key, const map<string, V> & value) {
	Json * newNode = JsonNew(JSON_DICT);
	if (!newNode)
		return NULL;
	for (typename map<string, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
		if (!JsonAdd(newNode, it->first.c_str(), it->second)) {
			delete newNode;
			newNode = NULL;
		}
	}
	if (newNode && !JsonAddNodeJson(node, key.c_str(), newNode)) {
		delete newNode;
		newNode = NULL;
	}
	return newNode;
}

// Macro RES_ENCODE_JSON {{{3
#define RES_ENCODE_JSON(rootNode, omitempty, key, obj) \
	if (!omitempty || !EqualDefault(obj)) { \
		if (!JsonAdd(&rootNode, key, GetValue(obj))) { LOG_ERROR("JsonAdd key(%s) to 0x%llx failed", string(key).c_str(), (long long unsigned)&rootNode); return false; } }

// EncodeResToStr {{{3
template <typename T>
inline bool ResEncodeToJsonStr(T & t, vector<char> & data, bool wide = false)
{
	vector<char> plainData;
	Json * jsonNode = JsonNew(JSON_DICT);
	if (!jsonNode) {
		LOG_ERROR("create jsonNode failed");
		return false;
	}
	if (!t.encode(*jsonNode)) {
		JsonFree(jsonNode);
		LOG_ERROR("encode jsonNode failed");
		return false;
	}
	int jsonStrLen = (wide ? JsonToStrLengthWide : JsonToStrLength)(jsonNode);
	plainData.resize(jsonStrLen + 1);
	(wide ? JsonToStrLocalWide : JsonToStrLocal)(jsonNode, jsonStrLen, plainData.data());
	plainData.pop_back();
	data.clear();
	JsonStr2HTTPStr(plainData, data);
	JsonFree(jsonNode);
	return true;
}

// Decode {{{2
// Check type {{{3
inline bool JsonCheckTypeEqual(Json * node, long      & val) { return node->type == JSON_INTEGER; (void)val; }
inline bool JsonCheckTypeEqual(Json * node, short     & val) { return node->type == JSON_INTEGER; (void)val; }
inline bool JsonCheckTypeEqual(Json * node, long long & val) { return node->type == JSON_INTEGER; (void)val; }
inline bool JsonCheckTypeEqual(Json * node, int       & val) { return node->type == JSON_INTEGER; (void)val; }
inline bool JsonCheckTypeEqual(Json * node, double    & val) { return node->type == JSON_FLOAT  ; (void)val; }
inline bool JsonCheckTypeEqual(Json * node, float     & val) { return node->type == JSON_FLOAT  ; (void)val; }
inline bool JsonCheckTypeEqual(Json * node, bool      & val) { return node->type == JSON_BOOLEAN; (void)val; }
inline bool JsonCheckTypeEqual(Json * node, string    & val) { return node->type == JSON_STRING ; (void)val; }
template <typename V>
inline bool JsonCheckTypeEqual(Json * node, map<string, V> & val) { return node->type == JSON_DICT; (void)val; }
template <typename V>
inline bool JsonCheckTypeEqual(Json * node, vector<V> & val) { return node->type == JSON_SET; (void)val; }
template <typename PT>
inline bool JsonCheckTypeEqual(Json * node, shared_ptr<PT> & val) { return JsonCheckTypeEqual(node, *val); }

// Get value {{{3
inline bool JsonGet(Json * node, long   & val)
{ if (!JsonCheckTypeEqual(node  , val)) { LOG_ERROR("type(%d) of JsonNode(0x%x) is not long"        , node->type , node); return false; } val = node->value.i             ; return true; }
inline bool JsonGet(Json * node , long long & val)
{ if (!JsonCheckTypeEqual(node  , val)) { LOG_ERROR("type(%lld) of JsonNode(0x%x) is not long long" , node->type , node); return false; } val = node->value.i             ; return true; }
inline bool JsonGet(Json * node , short  & val)
{ if (!JsonCheckTypeEqual(node  , val)) { LOG_ERROR("type(%d) of JsonNode(0x%x) is not short"       , node->type , node); return false; } val = node->value.i             ; return true; }
inline bool JsonGet(Json * node , int    & val)
{ if (!JsonCheckTypeEqual(node  , val)) { LOG_ERROR("type(%d) of JsonNode(0x%x) is not int"         , node->type , node); return false; } val = node->value.i             ; return true; }
inline bool JsonGet(Json * node , double & val)
{ if (!JsonCheckTypeEqual(node  , val)) { LOG_ERROR("type(%d) of JsonNode(0x%x) is not double"      , node->type , node); return false; } val = node->value.f.value       ; return true; }
inline bool JsonGet(Json * node , float  & val)
{ if (!JsonCheckTypeEqual(node  , val)) { LOG_ERROR("type(%d) of JsonNode(0x%x) is not float"       , node->type , node); return false; } val = (float)node->value.f.value; return true; }
inline bool JsonGet(Json * node , bool   & val)
{ if (!JsonCheckTypeEqual(node  , val)) { LOG_ERROR("type(%d) of JsonNode(0x%x) is not bool"        , node->type , node); return false; } val = node->value.b != 0        ; return true; }
inline bool JsonGet(Json * node , string & val)
{ if (!JsonCheckTypeEqual(node  , val)) { LOG_ERROR("type(%d) of JsonNode(0x%x) is not string"      , node->type , node); return false; } val = node->value.c             ; return true; }
inline bool JsonGet(Json * node, vector<char> & val) {
	string str;
	if (!JsonGet(node, str))
		return false;
	val.clear();
	val.insert(val.end(), str.begin(), str.end());
	return true;
}
template <typename T>
inline bool JsonGet(Json * node, T      & val) { return val.decode(*node); }
template <typename V>
inline bool JsonGet(Json * node, vector<V> & val) {
	val.clear();
	for (Json * child = node->child; child; child = child->next) {
		V tmp;
		if (!JsonGet(child, tmp))
			return false;
		val.push_back(tmp);
	}
	return true;
}
template <typename PT>
inline bool JsonGet(Json * node, shared_ptr<PT> & val) {
	PT tmp;
	if (!JsonGet(node, tmp))
		return false;
	if (EqualDefault(val))
		val.reset(new PT(tmp));
	else
		*val = tmp;
	return true;
}
template <typename V>
inline bool JsonGet(Json * node, map<string, V> & val) {
	val.clear();
	for (Json * child = node->child; child; child = child->next) {
		V tmp;
		if (!JsonGet(child, tmp))
			return false;
		val[child->key] = tmp;
	}
	return true;
}

// Macro RES_DECODE_JSON {{{3
#define RES_DECODE_JSON(rootNode, key, obj) { \
	Json * vNode = JsonGetChildByName(&rootNode, key); \
	if (vNode && !JsonGet(vNode, obj)) return false; \
	}

// DecodeResFromJsonStr {{{3
template <typename T>
inline bool ResDecodeFromJsonStr(T & t, const vector<char> & data)
{
	if (data.empty())
		return false;
	vector<char> plainData;
	HTTPStr2JsonStr(data, plainData);
	Json * jsonNode = JsonCreateFromStr(plainData.data());
	if (!jsonNode) {
		LOG_ERROR("JsonCreateFromStr failed: [%d] %.*s", data.size(), data.size() > 300 ? 300 : data.size(), data.data());
		return false;
	}
	if (!t.decode(*jsonNode)) {
		LOG_ERROR("JsonNode decode failed");
		JsonFree(jsonNode);
		return false;
	}
	return true;
}
// Map[string]string Marshal Helper {{{1
inline bool TransVal2Str(const long      & val, string & valStr) { stringstream ss; ss << val; valStr = ss.str(); return !valStr.empty(); }
inline bool TransVal2Str(const long long & val, string & valStr) { stringstream ss; ss << val; valStr = ss.str(); return !valStr.empty(); }
inline bool TransVal2Str(const short     & val, string & valStr) { stringstream ss; ss << val; valStr = ss.str(); return !valStr.empty(); }
inline bool TransVal2Str(const int       & val, string & valStr) { stringstream ss; ss << val; valStr = ss.str(); return !valStr.empty(); }
inline bool TransVal2Str(const double    & val, string & valStr) { stringstream ss; ss << val; valStr = ss.str(); return !valStr.empty(); }
inline bool TransVal2Str(const float     & val, string & valStr) { stringstream ss; ss << val; valStr = ss.str(); return !valStr.empty(); }
inline bool TransVal2Str(const bool      & val, string & valStr) { valStr = val ? "true" : "false"; return true; }
inline bool TransVal2Str(const string    & val, string & valStr) { stringstream ss; ss << val; valStr = ss.str(); return !valStr.empty(); }
inline bool TransVal2Str(const char      * val, string & valStr) { stringstream ss; ss << val; valStr = ss.str(); return !valStr.empty(); }

inline bool TransStr2Val(const string & valStr, long       & val) { stringstream ss(valStr); ss >> val; return true; }
inline bool TransStr2Val(const string & valStr, long long  & val) { stringstream ss(valStr); ss >> val; return true; }
inline bool TransStr2Val(const string & valStr, short      & val) { stringstream ss(valStr); ss >> val; return true; }
inline bool TransStr2Val(const string & valStr, int        & val) { stringstream ss(valStr); ss >> val; return true; }
inline bool TransStr2Val(const string & valStr, double     & val) { stringstream ss(valStr); ss >> val; return true; }
inline bool TransStr2Val(const string & valStr, float      & val) { stringstream ss(valStr); ss >> val; return true; }

inline int local_stricmp(const char * s1, const char * s2)
{
	if (s1 == s2) return 0;
	if (s1 == NULL) return -1;
	if (s2 == NULL) return 1;
	while(*s1 == *s2) {
		if (*s1 != 0) {
			++s1;
			++s2;
		} else {
			return 0;
		}
	}
	return *s1 - *s2;
}
inline bool TransStr2Val(const string & valStr, bool   & val) { val = local_stricmp(valStr.c_str(), "true") == 0; return true; }
inline bool TransStr2Val(const string & valStr, string & val) { stringstream ss(valStr); ss >> val; return true; }
inline bool TransStr2Val(const string & valStr, char   * val) { stringstream ss(valStr); ss >> val; return true; }

inline bool MapAddValue(map<string, string> & m, const char * key, const long      & val) { return TransVal2Str(val, m[key]); }
inline bool MapAddValue(map<string, string> & m, const char * key, const long long & val) { return TransVal2Str(val, m[key]); }
inline bool MapAddValue(map<string, string> & m, const char * key, const short     & val) { return TransVal2Str(val, m[key]); }
inline bool MapAddValue(map<string, string> & m, const char * key, const int       & val) { return TransVal2Str(val, m[key]); }
inline bool MapAddValue(map<string, string> & m, const char * key, const double    & val) { return TransVal2Str(val, m[key]); }
inline bool MapAddValue(map<string, string> & m, const char * key, const float     & val) { return TransVal2Str(val, m[key]); }
inline bool MapAddValue(map<string, string> & m, const char * key, const bool      & val) { return TransVal2Str(val, m[key]); }
inline bool MapAddValue(map<string, string> & m, const char * key, const string    & val) { return TransVal2Str(val, m[key]); }
inline bool MapAddValue(map<string, string> & m, const char * key, const char      * val) { return TransVal2Str(val, m[key]); }
template <typename T>
inline bool MapAddValue(map<string, string> & m, const char * key, const T & val)              { return val.encode(m); (void)key; }
template <typename V>
inline bool MapAddValue(map<string, string> & m, const char * key, const vector<V> & val)      { return true; (void)m; (void)key; (void)val; } // FIXME
template <typename PT>
inline bool MapAddValue(map<string, string> & m, const char * key, const shared_ptr<PT> & val) { return MapAddValue(m, key, *val); }

inline bool MapGetValue(const map<string, string> & m, const char * key, long      & val) { if (m.find(key) != m.end()) return TransStr2Val(m.find(key)->second, val); return true; }
inline bool MapGetValue(const map<string, string> & m, const char * key, long long & val) { if (m.find(key) != m.end()) return TransStr2Val(m.find(key)->second, val); return true; }
inline bool MapGetValue(const map<string, string> & m, const char * key, short     & val) { if (m.find(key) != m.end()) return TransStr2Val(m.find(key)->second, val); return true; }
inline bool MapGetValue(const map<string, string> & m, const char * key, int       & val) { if (m.find(key) != m.end()) return TransStr2Val(m.find(key)->second, val); return true; }
inline bool MapGetValue(const map<string, string> & m, const char * key, double    & val) { if (m.find(key) != m.end()) return TransStr2Val(m.find(key)->second, val); return true; }
inline bool MapGetValue(const map<string, string> & m, const char * key, float     & val) { if (m.find(key) != m.end()) return TransStr2Val(m.find(key)->second, val); return true; }
inline bool MapGetValue(const map<string, string> & m, const char * key, bool      & val) { if (m.find(key) != m.end()) return TransStr2Val(m.find(key)->second, val); return true; }
inline bool MapGetValue(const map<string, string> & m, const char * key, string    & val) { if (m.find(key) != m.end()) return TransStr2Val(m.find(key)->second, val); return true; }
inline bool MapGetValue(const map<string, string> & m, const char * key, char      * val) { if (m.find(key) != m.end()) return TransStr2Val(m.find(key)->second, val); return true; }
template <typename T>
inline bool MapGetValue(const map<string, string> & m, const char * key, T & val)              { return val.decode(m); (void)key; }
template <typename V>
inline bool MapGetValue(const map<string, string> & m, const char * key, vector<V> & val)      { return true; (void)m; (void)key; (void)val; } // FIXME
template <typename PT>
inline bool MapGetValue(const map<string, string> & m, const char * key, shared_ptr<PT> & val) { return MapGetValue(m, key, *val); }

#define RES_ENCODE_MAP(m, omitempty, key, obj) \
	if (!(omitempty && EqualDefault(obj))) { if (!MapAddValue(m, key, obj)) { LOG_ERROR("MapAddValue [%s] failed", string(key).c_str()); return false; } }
#define RES_DECODE_MAP(m, key, obj) \
	if (!MapGetValue(m, key, obj)) { LOG_ERROR("MapGetValue [%s] failed", string(key).c_str()); return false; }
// }}}1

// Inspect Helper {{{1
inline void InspectIndent(unsigned level) { for (unsigned i = 0; i < level; ++i) printf("  "); printf("- "); }
inline void InspectValue(const string & name, const long            & v, unsigned level = 0);
inline void InspectValue(const string & name, const long            & v, unsigned level)              { if (EqualDefault(v)) return; InspectIndent(level); printf("%s: %ld\n" , name.c_str(), v);                    }
inline void InspectValue(const string & name, const short           & v, unsigned level = 0);
inline void InspectValue(const string & name, const short           & v, unsigned level)              { if (EqualDefault(v)) return; InspectIndent(level); printf("%s: %d\n"  , name.c_str(), v);                    }
inline void InspectValue(const string & name, const int             & v, unsigned level = 0);
inline void InspectValue(const string & name, const int             & v, unsigned level)              { if (EqualDefault(v)) return; InspectIndent(level); printf("%s: %d\n"  , name.c_str(), v);                    }
inline void InspectValue(const string & name, const long long       & v, unsigned level = 0);
inline void InspectValue(const string & name, const long long       & v, unsigned level)              { if (EqualDefault(v)) return; InspectIndent(level); printf("%s: %lld\n", name.c_str(), v);                    }
inline void InspectValue(const string & name, const unsigned long   & v, unsigned level = 0);
inline void InspectValue(const string & name, const unsigned long   & v, unsigned level)              { if (EqualDefault(v)) return; InspectIndent(level); printf("%s: %lu\n" , name.c_str(), v);                    }
inline void InspectValue(const string & name, const unsigned short  & v, unsigned level = 0);
inline void InspectValue(const string & name, const unsigned short  & v, unsigned level)              { if (EqualDefault(v)) return; InspectIndent(level); printf("%s: %u\n"  , name.c_str(), v);                    }
inline void InspectValue(const string & name, const unsigned int    & v, unsigned level = 0);
inline void InspectValue(const string & name, const unsigned int    & v, unsigned level)              { if (EqualDefault(v)) return; InspectIndent(level); printf("%s: %u\n"  , name.c_str(), v);                    }
inline void InspectValue(const string & name, const double          & v, unsigned level = 0);
inline void InspectValue(const string & name, const double          & v, unsigned level)              { if (EqualDefault(v)) return; InspectIndent(level); printf("%s: %f\n"  , name.c_str(), v);                    }
inline void InspectValue(const string & name, const float           & v, unsigned level = 0);
inline void InspectValue(const string & name, const float           & v, unsigned level)              { if (EqualDefault(v)) return; InspectIndent(level); printf("%s: %f\n"  , name.c_str(), v);                    }
inline void InspectValue(const string & name, const char            & v, unsigned level = 0);
inline void InspectValue(const string & name, const char            & v, unsigned level)              { if (EqualDefault(v)) return; InspectIndent(level); printf("%s: %c\n"  , name.c_str(), v);                    }
inline void InspectValue(const string & name, const bool            & v, unsigned level = 0);
inline void InspectValue(const string & name, const bool            & v, unsigned level)              { if (EqualDefault(v)) return; InspectIndent(level); printf("%s: %s\n"  , name.c_str(), v ? "true" : "false"); }
inline void InspectValue(const string & name, const string          & v, unsigned level = 0);
inline void InspectValue(const string & name, const string          & v, unsigned level)              { if (EqualDefault(v)) return; InspectIndent(level); printf("%s: %s\n"  , name.c_str(), v.c_str());            }
template <typename T> inline void InspectValue(const string & name, const T & v, unsigned level = 0);
template <typename T> inline void InspectValue(const string & name, const T & v, unsigned level)      { if (EqualDefault(v)) return; v.inspect(name, level);                                                         }
template <typename V> inline void InspectValue(const string & name, const vector<V> & v, unsigned level = 0);
template <typename V> inline void InspectValue(const string & name, const vector<V> & v, unsigned level) {
	if (EqualDefault(v)) return;
	InspectIndent(level);
	printf("%s [VEC]:\n", name.c_str());
	int id = -1;
	char idStr[10] = {0};
	for (typename vector<V>::const_iterator it = v.begin(); it != v.end(); ++it) {
		sprintf(idStr, "%d", ++id);
		InspectValue(idStr, *it, level + 1);
	}
}
template <typename PT> inline void InspectValue(const string & name, const shared_ptr<PT> & v, unsigned level = 0);
template <typename PT> inline void InspectValue(const string & name, const shared_ptr<PT> & v, unsigned level) { if (EqualDefault(v)) return; InspectValue(name, *v, level); }
template <typename V> inline void InspectValue(const string & name, const map<string, V> & v, unsigned level = 0);
template <typename V> inline void InspectValue(const string & name, const map<string, V> & v, unsigned level) {
	if (EqualDefault(v)) return;
	InspectIndent(level);
	printf("%s [MAP]:\n", name.c_str());
	for (typename map<string, V>::const_iterator it = v.begin(); it != v.end(); ++it)
		InspectValue(it->first, it->second, level + 1);
}

// Equal Helper {{{2
template <typename PT>
inline bool PtrEqual(const shared_ptr<PT> & v1, const shared_ptr<PT> & v2) {
	bool v1NULL = EqualDefault(v1);
	bool v2NULL = EqualDefault(v2);
	if (v1NULL && v2NULL)
		return true;
	if (!v1NULL && !v2NULL)
		return *v1 == *v2;
	return false;
}

};

#endif /* end of include guard: MARSHAL_H_HU1NDXVW */
// vim:fdm=marker

