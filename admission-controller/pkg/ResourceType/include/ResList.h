#ifndef RESLIST_H_LWPYSL8Q
#define RESLIST_H_LWPYSL8Q

#include <vector>
using std::vector;

#include "Marshal.h"
#include "meta/v1/types.h"
#include "ejson/include/Json.h"

namespace apis {

// ResList {{{2
template <class T>
struct ResList : public apis::meta::v1::TypeMeta, public vector<T>
{
    ResList() :
		apis::meta::v1::TypeMeta(getResName(), "v1"),
		metadata(DefaultValue< apis::meta::v1::ListMeta >())
    {
        items = this;
    }
	static string getResName()
	{
		return T::getResName() + "List";
	}
    bool decode(const Json & data)
    {
		((apis::meta::v1::TypeMeta *)this)->decode(data);
		RES_DECODE_JSON(data, "metadata", this->metadata);
		RES_DECODE_JSON(data, "items", *this->items);
        return true;
    }
    bool encode(Json & data) const
    {
		((apis::meta::v1::TypeMeta *)this)->encode(data);
		RES_ENCODE_JSON(data, true, "metadata", this->metadata);
		RES_ENCODE_JSON(data, false, "items", *this->items);
        return true;
    }
    bool operator==(const ResList<T> &other) const {
		return this->metadata == other.metadata
			&& *this->items == *other.items
			&& *(apis::meta::v1::TypeMeta *)this == *(apis::meta::v1::TypeMeta *)&other;
    }
    void inspect(const string & name = "", unsigned level = 0) const {
        InspectIndent(level);
        printf("%s[%sList]:\n", name.c_str(), T::getResName());
		((apis::meta::v1::TypeMeta *)this)->inspect("typeMeta", level + 1);
		InspectValue("metadata", this->metadata, level + 1);
		InspectValue("items", *this->items, level + 1);
    }

    meta::v1::ListMeta metadata; // `json:"metadata,omitempty"`
    vector<T> * items;
};

}

#endif /* end of include guard: RESLIST_H_LWPYSL8Q */
