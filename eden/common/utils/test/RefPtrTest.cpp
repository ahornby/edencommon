/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "eden/common/utils/RefPtr.h"

#include <folly/portability/GTest.h>

namespace {

using namespace facebook::eden;

class MyThing : public RefCounted {
 public:
  MyThing() = default;
  explicit MyThing(std::string n) : name{std::move(n)} {}
  std::string name;
};

using MyThingPtr = RefPtr<MyThing>;

TEST(RefCounted, make_and_release) {
  auto ptr = makeRefPtr<MyThing>();
}

TEST(RefCounted, access) {
  auto ptr = makeRefPtr<MyThing>("hello");
  EXPECT_TRUE(ptr);
  EXPECT_EQ("hello", ptr->name);
  EXPECT_EQ("hello", (*ptr).name);
  EXPECT_EQ("hello", ptr.get()->name);
}

TEST(RefCounted, moving) {
  auto ptr1 = makeRefPtr<MyThing>();
  EXPECT_TRUE(ptr1);
  MyThingPtr ptr2{std::move(ptr1)};
  EXPECT_FALSE(ptr1);
  EXPECT_TRUE(ptr2);
  ptr1 = std::move(ptr2);
  EXPECT_TRUE(ptr1);
  EXPECT_FALSE(ptr2);

  MyThingPtr& this_self_move_is_intentional = ptr1;
  ptr1 = std::move(this_self_move_is_intentional);
  // Tautology, but all that is required is that the value is in a
  // valid-but-unspecified state.
  // https://ericniebler.com/2017/03/31/post-conditions-on-self-move/
  EXPECT_TRUE(ptr1 || !ptr1);
}

TEST(RefCounted, copy_is_explicit) {
  auto ptr1 = makeRefPtr<MyThing>("hello");
  auto ptr2 = ptr1.copy();
  EXPECT_EQ("hello", ptr1->name);
  EXPECT_EQ("hello", ptr2->name);

  MyThingPtr empty;
  EXPECT_FALSE(empty);
  EXPECT_FALSE(empty.copy());
}

TEST(RefCounted, copies_hold_a_reference) {
  auto ptr1 = makeRefPtr<MyThing>("hello");
  auto ptr2 = ptr1.copy();
  ptr1.reset();

  EXPECT_EQ("hello", ptr2->name);
}

TEST(RefCounted, singletons_dont_deallocate) {
  static MyThing singleton{"singleton"};
  auto ptr1 = MyThingPtr::singleton(singleton);
  EXPECT_TRUE(ptr1);
  EXPECT_EQ("singleton", ptr1->name);
  auto ptr2 = ptr1.copy();
  auto ptr3 = std::move(ptr2);
  EXPECT_TRUE(ptr1);
  EXPECT_FALSE(ptr2);
  EXPECT_TRUE(ptr3);
  EXPECT_EQ("singleton", ptr3->name);
}

TEST(RefCounted, reset) {
  auto ptr = makeRefPtr<MyThing>("hello");
  ptr.reset();
  EXPECT_FALSE(ptr);
}

struct Base : RefCounted {
  virtual char get() {
    return 'b';
  }
};

struct Derived : Base {
  char get() override {
    return 'd';
  }
};

TEST(RefCounted, convert_to_base_owned) {
  auto derived = makeRefPtr<Derived>();
  RefPtr<Base> base = std::move(derived);
  EXPECT_EQ('d', base->get());
}

TEST(RefCounted, convert_to_base_singleton) {
  static Derived singleton;
  auto derived = RefPtr<Derived>::singleton(singleton);
  RefPtr<Base> base = std::move(derived);
  EXPECT_EQ('d', base->get());
}

TEST(RefCounted, RefPtr_of_Derived_cast_to_RefPtr_of_Base) {
  auto derived = makeRefPtr<Derived>();
  const RefPtr<Base>& base = derived.as<Base>();
  EXPECT_EQ('d', base->get());
  EXPECT_EQ(base.get(), derived.get());
}

struct Base2 {
  virtual ~Base2() {}
  // Some member that provides an offset before Base.
  int dummy;
};

struct Derived2 : Base2, Base {
  char get() override {
    return 'e';
  }
};

TEST(RefCounted, Derived_as_to_second_Base_with_offset) {
  auto derived = makeRefPtr<Derived2>();
  const RefPtr<Base>& base = derived.as<Base>();
  EXPECT_EQ('e', base->get());
  EXPECT_EQ(base.get(), derived.get());
}

TEST(RefCounted, bumping_reference_count_through_as_affects_parent) {
  auto derived = makeRefPtr<Derived2>();
  const RefPtr<Base>& base = derived.as<Base>();
  auto base_copy = base.copy();
  derived.reset(); // This also invalidates `base`
  EXPECT_FALSE(derived);
  EXPECT_EQ('e', base_copy->get());
}

TEST(RefCounted, no_reference_bumping_when_as_on_singleton) {
  Derived2 d2;
  auto derived = RefPtr<Derived2>::singleton(d2);
  const RefPtr<Base>& base = derived.as<Base>();
  auto base_copy = base.copy();
  derived.reset(); // This also invalidates `base`
  EXPECT_FALSE(derived);
  EXPECT_EQ('e', base_copy->get());
}

} // namespace
