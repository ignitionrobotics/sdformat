/*
 * Copyright 2016 Open Source Robotics Foundation
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
 *
*/
#ifndef SDF_PARSER_PRIVATE_HH_
#define SDF_PARSER_PRIVATE_HH_

#include <tinyxml2.h>

#include <string>

#include "sdf/SDFImpl.hh"
#include "sdf/config.hh"
#include "sdf/system_util.hh"

/// \ingroup sdf_parser
/// \brief namespace for Simulation Description Format parser
namespace sdf
{
  // Inline bracket to help doxygen filtering.
  inline namespace SDF_VERSION_NAMESPACE {
  //

  /// \brief Get the best SDF version from models supported by this sdformat
  /// \param[out] _modelFileName file name of the best model file
  /// \param[out] _errors Vector of errors.
  /// \param[in] _modelXML XML element from config file pointing to the
  ///            model XML tag
  /// \return string with the best SDF version supported
  std::string getBestSupportedModelVersion(std::string &_modelFileName,
                                           sdf::Errors &_errors,
                                           tinyxml2::XMLElement *_modelXML);

  /// \brief Initialize the SDF interface using a TinyXML2 document.
  ///
  /// This actually forwards to initXml after converting the inputs
  /// \param[out] _errors Vector of errors.
  /// \param[out] _sdf SDF interface to be initialized
  /// \param[in] _xmlDoc TinyXML2 document containing the SDFormat description
  /// file that corresponds with the input SDFPtr
  /// \param[in] _config Custom parser configuration
  /// \return True on success, false on error.
  bool initDoc(sdf::Errors &_errors,
               SDFPtr _sdf,
               tinyxml2::XMLDocument *_xmlDoc,
               const ParserConfig &_config);

  /// \brief Initialize the SDF Element using a TinyXML2 document
  ///
  /// This actually forwards to initXml after converting the inputs
  /// \param[out] _errors Vector of errors.
  /// \param[out] _sdf SDF Element to be initialized
  /// \param[in] _xmlDoc TinyXML2 document containing the SDFormat description
  /// file that corresponds with the input ElementPtr
  /// \param[in] _config Custom parser configuration
  /// \return True on success, false on error.
  bool initDoc(sdf::Errors &_errors,
               ElementPtr _sdf,
               tinyxml2::XMLDocument *_xmlDoc,
               const ParserConfig &_config);

  /// \brief Initialize the SDF Element by parsing the SDFormat description in
  /// the input TinyXML2 element. This is where SDFormat spec/description files
  /// are parsed
  /// \remark For internal use only. Do not use this function.
  /// \param[out] _errors Vector of errors.
  /// \param[out] _sdf SDF ElementPtr to be initialized
  /// \param[in] _xml TinyXML2 element containing the SDFormat description
  /// file that corresponds with the input ElementPtr
  /// \param[in] _config Custom parser configuration
  /// \return True on success, false on error.
  bool initXml(sdf::Errors &_errors,
               ElementPtr _sdf,
               tinyxml2::XMLElement *_xml,
               const ParserConfig &_config);

  /// \brief Populate the SDF values from a TinyXML document
  bool readDoc(tinyxml2::XMLDocument *_xmlDoc, SDFPtr _sdf,
                      const std::string &_source, bool _convert,
                      const ParserConfig &_config, Errors &_errors);

  /// \brief Populate the SDF values from a TinyXML document
  bool readDoc(tinyxml2::XMLDocument *_xmlDoc, ElementPtr _sdf,
      const std::string &_source, bool _convert, const ParserConfig &_config,
      Errors &_errors);

  /// \brief Perform a series of checks to determine the validity of this XML
  /// document from the root level element in the context of an SDF Element.
  /// This is called before readXml.
  /// \remark For internal use only. Do not use this function.
  /// \param[in] _xmlRoot Pointer to the root level TinyXML element.
  /// \param[in] _source Source of the XML document.
  /// \param[in] _errors Captures errors found during the checks.
  /// \return True on success, false on error.
  bool checkXmlFromRoot(tinyxml2::XMLElement *_xmlRoot,
      const std::string &_source, Errors &_errors);

  /// \brief Populate an SDF Element from the XML input. The XML input here is
  /// an actual SDFormat file or string, not the description of the SDFormat
  /// spec.
  /// \remark For internal use only. Do not use this function.
  /// \param[in] _xml Pointer to the TinyXML element
  /// \param[in,out] _sdf SDF pointer to parse data into.
  /// \param[in] _config Custom parser configuration
  /// \param[in] _source Source of the XML document, empty if it came from a
  ///            string.
  /// \param[out] _errors Captures errors found during parsing.
  /// \return True on success, false on error.
  bool readXml(tinyxml2::XMLElement *_xml, ElementPtr _sdf,
      const ParserConfig &_config, const std::string &_source, Errors &_errors);

  /// \brief Copy child XML elements into the _sdf element.
  /// \param[in] _sdf Parent Element.
  /// \param[in] _xml Pointer to element from which child elements should be
  /// copied.
  /// \param[in] _onlyUnknown True to copy only elements that are NOT part of
  /// the SDF spec. Set this to false to copy everything.
  void copyChildren(ElementPtr _sdf, tinyxml2::XMLElement *_xml,
                    const bool _onlyUnknown);
  }
}
#endif
