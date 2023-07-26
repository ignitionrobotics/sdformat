/*
 * Copyright (C) 2023 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pyElement.hh"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>

#include "sdf/Element.hh"
#include "pybind11_helpers.hh"

#include "sdf/config.hh"

using namespace pybind11::literals;
namespace py = pybind11;
namespace sdf
{
// Inline bracket to help doxygen filtering.
inline namespace SDF_VERSION_NAMESPACE {
namespace python
{

using PyClassElement = py::class_<sdf::Element, sdf::ElementPtr>;

template <typename T>
std::string computeSuffix()
{
  // TypeToString returns a value with a space, which would not be a valid 
  // python function name, so we override that here.
  if constexpr (std::is_same_v<T, unsigned int>)
    return "unsigned_int";
  return ParamPrivate::TypeToString<T>();
}

template <typename T>
struct DefineElementGetSet
{
  void operator()(PyClassElement &_cls)
  {
    const std::string getFuncName = "get_" + computeSuffix<T>();
    const std::string setFuncName = "set_" + computeSuffix<T>();
    _cls.def(
            getFuncName.c_str(),
            [](const sdf::Element &_self, const std::string &_key)
            {
              sdf::Errors errors;
              T val = _self.Get<T>(errors, _key);
              ThrowIfErrors(errors);
              return val;
            },
            "Get the value of a key. This function assumes the _key exists.")
        .def(
            getFuncName.c_str(),
            [](const sdf::Element &_self, const std::string &_key,
               const T &_defaultValue)
            {
              sdf::Errors errors;
              std::pair<T, bool> val =
                  _self.Get<T>(errors, _key, _defaultValue);
              ThrowIfErrors(errors);
              return val;
            },
            "Get the value of a key. This function assumes the _key exists.")
        .def(
            setFuncName.c_str(),
            [](sdf::Element &_self, const T &_value)
            {
              sdf::Errors errors;
              bool result = _self.Set<T>(errors, _value);
              ThrowIfErrors(errors);
              return result;
            },
            "Get the value of a key. This function assumes the _key exists.");
  }
};

template<typename ...Ts>
struct DefineElementGetSet<std::variant<Ts...>>
{
  void operator()(PyClassElement &_cls)
  {
    (DefineElementGetSet<Ts>()(_cls), ...);
  }
};

/////////////////////////////////////////////////
// TODO(azeey) Interspersed between the `.def` calls is a list of
// functions that need to be implemented. I'm not 100% sure if all of them need
// to be implemented since some of them are mainly used internally by the
// parser. I've already excluded the following functions on this criteria:
// - SetReferenceSDF
// - ReferenceSDF
// - PrintDescription (Until https://github.com/gazebosim/sdformat/issues/1302
// is resolved)
// - PrintValues (Because ToString is available)
// - PrintDocLeftPane (Helper function for generating documentation)
// - PrintDocRightPane (Helper function for generating documentation)
// - GetElementDescriptionCount (Until
// https://github.com/gazebosim/sdformat/issues/1302 is resolved)
// - GetElementDescription (Until
// https://github.com/gazebosim/sdformat/issues/1302 is resolved)
// - HasElementDescription (Until
// https://github.com/gazebosim/sdformat/issues/1302 is resolved)
// - HasUniqueChildNames (Used for error checking by the parser)
// - CountNamedElements (Used for error checking by the parser)
// - GetElement (FindElement and AddElement should be used instead)
// - GetDescription (Until https://github.com/gazebosim/sdformat/issues/1302 is
// resolved)
// - SetDescription (Until https://github.com/gazebosim/sdformat/issues/1302 is
// resolved)
// - AddElementDescription (Until
// https://github.com/gazebosim/sdformat/issues/1302 is resolved)
// - GetElementImpl (Use FindElement instead)
// - NameUniquenessExceptions (Used for error checking by the parser)
void defineElement(py::object module)
{
  auto elemClass =
      PyClassElement(module, "Element")
          .def(py::init<>())
          .def(
              "clone",
              [](const sdf::Element &_self)
              {
                sdf::Errors errors;
                sdf::ElementPtr elem = _self.Clone(errors);
                ThrowIfErrors(errors);
                return elem;
              },
              "Create a copy of this Element.")
          .def("get_parent", &sdf::Element::GetParent,
               "Get a pointer to this Element's parent.")
          .def("set_parent", &sdf::Element::SetParent,
               "Set the parent of this Element.")
          .def("set_name", &sdf::Element::SetName,
               "Set the name of the Element")
          .def("get_name", &sdf::Element::GetName, "Get the Element's name.")
          .def("set_required", &sdf::Element::SetRequired,
               "Set the requirement type.")
          .def("get_required", &sdf::Element::GetRequired,
               "Get the requirement string.")
          // print_description
          .def("set_explicitly_set_in_file",
               &sdf::Element::SetExplicitlySetInFile,
               "Set if the element and children where set or default in the "
               "original file")
          .def("get_explicitly_set_in_file",
               &sdf::Element::GetExplicitlySetInFile,
               "Return if the element was been explicitly set in the file")
          // to_string
          .def(
              "add_attribute",
              [](sdf::Element &_self, const std::string &_key,
                 const std::string &_type, const std::string &_defaultvalue,
                 bool _required, const std::string &_description = "")
              {
                sdf::Errors errors;
                _self.AddAttribute(_key, _type, _defaultvalue, _required,
                                   errors, _description);
                ThrowIfErrors(errors);
              },
              "Add an attribute value.")
          .def(
              "add_value",
              [](sdf::Element &_self, const std::string &_type,
                 const std::string &_defaultValue, bool _required,
                 const std::string &_description = "")
              {
                sdf::Errors errors;
                _self.AddValue(_type, _defaultValue, _required, errors,
                               _description);
                ThrowIfErrors(errors);
              },
              "Add a value to this Element")
          .def(
              "add_value",
              [](sdf::Element &_self, const std::string &_type,
                 const std::string &_defaultValue, bool _required,
                 const std::string &_minValue, const std::string &_maxValue,
                 const std::string &_description = "")
              {
                sdf::Errors errors;
                _self.AddValue(_type, _defaultValue, _required, _minValue,
                               _maxValue, errors, _description);
                ThrowIfErrors(errors);
              },
              "Add a value to this Element")
          // get_attribute (string)
          .def("get_attribute_count", &sdf::Element::GetAttributeCount,
               "Get the number of attributes.")
          // get_attributes
          // get_attribute (index)
          // get_element_description_count
          // get_element_description (index)
          // get_element_description (string)
          // has_element_description
          .def("has_attribute", &sdf::Element::HasAttribute,
               "Return true if an attribute exists.")
          .def("get_attribute_set", &sdf::Element::GetAttributeSet,
               "Return true if the attribute was set (i.e. not default value)")
          // remove_attribute
          // remove_all_attributes
          // get_value
          .def(
              "get_any",
              [](sdf::Element &_self, const std::string &_key = "")
              {
                sdf::Errors errors;
                auto output = _self.GetAny(errors,_key);
                ThrowIfErrors(errors);
                return output;
              },
              "Add a value to this Element")
          .def("has_element", &sdf::Element::HasElement,
               "Return true if the named element exists.")
          .def("get_first_element", &sdf::Element::GetFirstElement,
               "Get the first child element")
          .def("get_next_element", &sdf::Element::GetNextElement,
               "Get the first child Get the next sibling of this element.")
          // get_element_type_names
          .def("find_element",
               py::overload_cast<const std::string &>(
                   &sdf::Element::FindElement),
               "Return a pointer to the child element with the provided name.")
          .def(
              "add_element",
              [](sdf::Element &_self, const std::string &_name)
              {
                sdf::Errors errors;
                auto output = _self.AddElement(_name, errors);
                ThrowIfErrors(errors);
                return output;
              },
              "Add a value to this Element")
          .def("insert_element",
               py::overload_cast<sdf::ElementPtr, bool>(
                   &sdf::Element::InsertElement),
               "Add an element object, and optionally set the given element's "
               "parent to this object",
               "elem"_a, "set_parent_to_self"_a = false)
          // remove_from_parent
          // remove_child
          // clear_elements
          // clear
          // update
          // reset
          .def("set_include_element", &sdf::Element::SetIncludeElement,
               "Set the `<include>` element that was used to load this element")
          .def(
              "get_include_element", &sdf::Element::GetIncludeElement,
              "Get the `<include>` element that was used to load this element.")
          .def("set_file_path", &sdf::Element::SetFilePath,
               "Set the path to the SDF document where this element came from.")
          .def("file_path", &sdf::Element::FilePath,
               "Get the path to the SDF document where this element came from")
          .def("set_line_number", &sdf::Element::SetLineNumber,
               "Set the line number of this element within the SDF document.")
          .def("line_number", &sdf::Element::LineNumber,
               "Get the line number of this element within the SDF document.")
          .def("set_xml_path", &sdf::Element::SetXmlPath,
               "Set the XML path of this element.")
          .def("xml_path", &sdf::Element::XmlPath,
               "Get the XML path of this element.")
          .def("set_original_version", &sdf::Element::SetOriginalVersion,
               "Set the spec version that this was originally parsed from.")
          .def("original_version", &sdf::Element::OriginalVersion,
               "Get the spec version that this was originally parsed from.");
  DefineElementGetSet<ParamPrivate::ParamVariant>()(elemClass);
}
}  // namespace python
}  // namespace SDF_VERSION_NAMESPACE
}  // namespace sdf
