#ifndef LOKI_TYPEINFO_H
#define LOKI_TYPEINFO_H

#include <typeinfo>

#include "loki.h"

LOKI_NS_BEGIN

class TypeInfo {
public:
   // Constructors/destructors
  TypeInfo() = default;
  TypeInfo(const std::type_info& type_info) {
  pInfo_ = &type_info;
  }
  TypeInfo(const TypeInfo& type_info) {
   pInfo_ = type_info.pInfo_;
  }
  TypeInfo& operator=(const TypeInfo& type_info) {
   pInfo_ = type_info.pInfo_;
   return *this;
  }
  // Compatibility functions
  inline bool before(const TypeInfo& type_info) const {
   return pInfo_->before(*type_info.pInfo_);
  }
  inline const char* name() const {
   return pInfo_->name();
  }

// Comparison operators
  inline bool operator==(const TypeInfo& rhs) const {
    return pInfo_ == rhs.pInfo_;
  }
  inline bool operator!=(const TypeInfo& rhs) const {
    return pInfo_ != rhs.pInfo_;
  }
  inline bool operator<(const TypeInfo& rhs) const {
    return before(rhs);
  }
  inline bool operator<=(const TypeInfo& rhs) const {
    return before(rhs) || pInfo_ == rhs.pInfo_;
  }
  inline bool operator>(const TypeInfo& rhs) const {
    return rhs.before(*this);
  }
  inline bool operator>=(const TypeInfo& rhs) const {
    return rhs.before(*this) || pInfo_ == rhs.pInfo_;
  }
private:
   const std::type_info* pInfo_;
};

LOKI_NS_END

#endif
