// Copyright 2010 The Ginsu Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef C_SALT_TYPE_H_
#define C_SALT_TYPE_H_

#include <nacl/npruntime.h>

#include <string>
#include <vector>

#include "boost/noncopyable.hpp"
#include "boost/shared_ptr.hpp"

// A polymorphic type container.  All the various number and string types
// are derived from this basic class.  The value is immutable.

namespace c_salt {

class ScriptingBridge;

class Type : public boost::noncopyable {
 public:
  typedef int TypeId;
  typedef std::vector<Type*> TypeArray;

  static const int kClassVersion = 1;
  enum {
    kNullTypeId,
    kBoolTypeId,
    kInt32TypeId,
    kDoubleTypeId,
    kStringTypeId,
    kObjectTypeId
  };

  // Create a new Type from the contents of |np_var|.  The caller is
  // responsible for freeing the resulting Type.  Note that complex types
  // such as strings and objects are retained in the browser, and a pointer
  // to that memory is cached in the Type.  This is the only way to create
  // a generic Type.  To create a specific Type, use one of the sub-class
  // ctors.
  static Type* CreateFromNPVariant(const NPVariant& np_var);

  // Create a new Type form another Type, but change the underlying storage
  // hint to be |type_id|.
  static Type* CreateFromTypeWithTypeId(const Type& type, TypeId type_id);

  // Create a new TypeArray from the contents of |np_array|.  The caller is
  // responsible for deleting the resulting array.
  static TypeArray* CreateArrayFromNPVariant(const ScriptingBridge* bridge,
                                             const NPVariant& np_array);

  explicit Type(TypeId type_id);
  virtual ~Type();

  TypeId type_id() const {
    return type_id_;
  }

  // Package the underlying representation as an NPVariant.  On return,
  // |np_var| will contain the NPVariant representation of the Type's intrinsic
  // value.  If |np_var| is NULL, this method does nothing and returns.
  virtual bool ConvertToNPVariant(NPVariant* np_var) const;

  // Return the closest representation of the internal type that matches the
  // requested type.  Some conversion are not possible; if the the internal
  // type is an object, an attempt is made to evaluate the object.
  virtual bool bool_value() const;
  virtual int32_t int32_value() const;
  virtual double double_value() const;
  // Note that this returns a copy of the internal string: Use with caution!
  // TODO(depsringer): We should probably consider handling string separately
  // from the rest of the variants.
  virtual std::string string_value() const;
  virtual NPObject* object_value() const;

 private:
  uint32_t class_version_;
  TypeId type_id_;

  Type();  // Not implemented, do not use.
};

typedef boost::shared_ptr<Type> SharedType;

}  // namespace c_salt

#endif  // C_SALT_TYPE_H_
