// Copyright 2018 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef RCLCPP__REMAPPING_HPP_
#define RCLCPP__REMAPPING_HPP_

#include <memory>
#include <string>

#include "rcl/remap.h"
#include "rclcpp/macros.hpp"

namespace rclcpp
{

/// Encapsulation of a remapping rule.
/**
 * This is an abstract base class that is meant to be inherited from for
 * specific kinds of remapping rules, e.g. node name versus topic name.
 *
 * If derived classes are default constructed, then the remapping rule will
 * match all node names and all names, but will return the identity when you
 * attempt to remap a name.
 */
class Remapping
{
public:
  Remapping();

  /// Construct based on an existing rcl remap rule.
  explicit
  Remapping(const rcl_remap_t & rcl_remap_rule);

  virtual ~Remapping();

  RCLCPP_DISABLE_COPY(Remapping)

  Remapping(Remapping &&);
  Remapping & operator=(Remapping &&);

  class Impl;

  /// Return true if the given node name matches the remapping rule.
  /**
   * This should be the fully qualified node name.
   */
  virtual
  bool
  applies_to_node_name(const char * node_name) const = 0;

  bool
  applies_to_node_name(const std::string & node_name) const;

  /// Return true if the given name (node, namespace, topic, etc...) matches the remapping rule.
  virtual
  bool
  matches_name(const char * name) const = 0;

  bool
  matches_name(const std::string & name) const;

  /// Return true if the remapping rule is global and not node specific.
  bool
  is_global() const;

  /// Return the replacement rule if the node name and name match the rule.
  /**
   * If it applies to all nodes use `nullptr` for `node_name`.
   * If there is no name to be matched against (in the case of node name and
   * node namespace remapping rules) use `nullptr` for `name`.
   */
  virtual
  const char *
  remap(const char * node_name, const char * name) const = 0;

  /// Return the node name for which this rule applies or nullptr if it applies to all nodes.
  const char *
  get_node_name() const;

  /// Return the rule matching string if there is one, otherwise nullptr.
  /**
   * There are cases where there is no matching rule, e.g. for node name and
   * namespace remapping rules.
   */
  const char *
  get_match_string() const;

  /// Return the replacement string, which should always exist (never nullptr).
  const char *
  get_replacement_string() const;

protected:
  std::unique_ptr<Impl> impl_;
};

}  // namespace rclcpp

#endif  // RCLCPP__REMAPPING_HPP_
