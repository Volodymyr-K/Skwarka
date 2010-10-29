#include "../PbrtSceneImporter.h"
#include "PbrtUtils.h"
#include "PbrtStringRoutines.h"
#include <cstdio>
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

PbrtSceneImporter::PbrtSceneImporter(std::string i_filename, intrusive_ptr<Log> ip_log):
m_filename(i_filename), mp_log(ip_log)
  {
  m_currentApiState = STATE_UNINITIALIZED;

  m_known_commands.push_back("Accelerator");
  m_known_commands.push_back("ActiveTransform");
  m_known_commands.push_back("AreaLightSource");
  m_known_commands.push_back("AttributeBegin");
  m_known_commands.push_back("AttributeEnd");
  m_known_commands.push_back("Camera");
  m_known_commands.push_back("ConcatTransform");
  m_known_commands.push_back("CoordinateSystem");
  m_known_commands.push_back("CoordSysTransform");
  m_known_commands.push_back("Film");
  m_known_commands.push_back("Identity");
  m_known_commands.push_back("Include");
  m_known_commands.push_back("LightSource");
  m_known_commands.push_back("LookAt");
  m_known_commands.push_back("MakeNamedMaterial");
  m_known_commands.push_back("Material");
  m_known_commands.push_back("NamedMaterial");
  m_known_commands.push_back("ObjectBegin");
  m_known_commands.push_back("ObjectEnd");
  m_known_commands.push_back("ObjectInstance");
  m_known_commands.push_back("PixelFilter");
  m_known_commands.push_back("Renderer");
  m_known_commands.push_back("ReverseOrientation");
  m_known_commands.push_back("Rotate");
  m_known_commands.push_back("Sampler");
  m_known_commands.push_back("Scale");
  m_known_commands.push_back("Shape");
  m_known_commands.push_back("SurfaceIntegrator");
  m_known_commands.push_back("Texture");
  m_known_commands.push_back("TransformBegin");
  m_known_commands.push_back("TransformEnd");
  m_known_commands.push_back("TransformTimes");
  m_known_commands.push_back("Transform");
  m_known_commands.push_back("Translate");
  m_known_commands.push_back("Volume");
  m_known_commands.push_back("VolumeIntegrator");
  m_known_commands.push_back("WorldBegin");
  m_known_commands.push_back("WorldEnd");

  for(size_t i=0;i<m_known_commands.size();++i)
    boost::to_lower(m_known_commands[i]);
  std::sort(m_known_commands.begin(), m_known_commands.end());

  _ReadScene();
  }

bool PbrtSceneImporter::_VerifyInitialized(const std::string &i_method) const
  {
  if (m_currentApiState == STATE_UNINITIALIZED)
    {
    PbrtImport::Utils::LogError(mp_log, std::string("_pbrtInit() must be before calling ") + i_method + std::string(". Ignoring."));
    return false;
    }
  return true;
  }

bool PbrtSceneImporter::_VerifyOptions(const std::string &i_method) const
  {
  if (_VerifyInitialized(i_method)==false) return false;
  if (m_currentApiState == STATE_WORLD_BLOCK)
    {
    PbrtImport::Utils::LogError(mp_log, std::string("Options cannot be set inside world block; ") + i_method + std::string(" not allowed. Ignoring."));
    return false;
    }
  return true;
  }

bool PbrtSceneImporter::_VerifyWorld(const std::string &i_method) const
  {
  if (_VerifyInitialized(i_method)==false) return false;
  if (m_currentApiState == STATE_OPTIONS_BLOCK)
    {
    PbrtImport::Utils::LogError(mp_log, std::string("Scene description must be inside world block; ") + i_method + std::string(" not allowed. Ignoring."));
    return false;
    }
  return true;
  }

intrusive_ptr<const Scene> PbrtSceneImporter::GetScene() const
  {
  return mp_scene;
  }

std::vector<intrusive_ptr<const Camera> > PbrtSceneImporter::GetCameras() const
  {
  if (mp_camera)
    return std::vector<intrusive_ptr<const Camera> >(1, mp_camera);
  else
    return std::vector<intrusive_ptr<const Camera> >();
  }

void PbrtSceneImporter::_ReadScene()
  {
  mp_scene.reset(NULL);

  _pbrtInit();
  _ParseFile();

  mp_scene = mp_renderOptions->MakeScene();
  mp_camera = mp_renderOptions->MakeCamera();

  _pbrtCleanup();
  }

void PbrtSceneImporter::_ParseFile()
  {
  m_buffer = "";
  if (_ReadLines(m_filename)==false) return;

  // Make sure that the last read command is processed.
  if (_ProcessCommand()==false) return;
  }

/**
* This method reads the specified file and processes all found commands.
* The method also recursively reads all the included files.
* Returns true on success and failure in case of error.
*/
bool PbrtSceneImporter::_ReadLines(const std::string &i_filename)
  {
  // Convert the filename to the canonical form and check if we already visited this file on the higher call stack levels.
  // If yes, it means we found a cycle so the method returns.
  std::string filename = PbrtImport::StringRoutines::CanonizeFilePath(i_filename);
  if (std::find(m_pushed_filenames.begin(), m_pushed_filenames.end(), filename) != m_pushed_filenames.end())
    {
    PbrtImport::Utils::LogError(mp_log, "There is a cycle of the included files.");
    return false;
    }

  FILE *fp = fopen(filename.c_str(), "r"); 
  if (fp==NULL)
    {
    PbrtImport::Utils::LogError(mp_log, std::string("The file \"") + i_filename + std::string("\" can not be opened."));
    return false;
    }

  // Push the current file to the stack for the later cycle detection/
  m_pushed_filenames.push_back(filename);

  std::string s;
  char c = fgetc(fp);
  do
    {
    if (c=='\n' || c=='\r')
      {
      // Remove comments.
      size_t hash_pos = s.find_first_of('#');
      if (hash_pos != std::string::npos) s.erase(hash_pos);      

      // Adds line to the buffer and executes the command if necessary.
      if (_AddLine(s)==false) {fclose(fp);m_pushed_filenames.pop_back();return false;}
      s.clear();
      c = fgetc(fp);
      continue;
      }
    s += c;

    c = fgetc(fp);
    } while (feof(fp) == 0); 

  // Don't forget about the last read line.
  if (_AddLine(s)==false) {fclose(fp);m_pushed_filenames.pop_back();return false;}

  fclose(fp);
  m_pushed_filenames.pop_back();
  return true;
  }

/**
* Adds the specified line to the current buffer and processes the commands if an entire command has been read.
* The method also calls _ReadLines() method for "Include" commands to process the included files.
*/
bool PbrtSceneImporter::_AddLine(const std::string &i_line)
  {
  std::string line = boost::trim_copy(i_line);
  if (line.empty()) return true;

  // If this is the "Include" command we call the _ReadLines() method recursively to process the included file.
  std::string first_word = PbrtImport::StringRoutines::GetFirstWord(line);
  if (boost::iequals(first_word, "Include"))
    {
    std::string new_filename = boost::trim_copy( line.substr(first_word.size()) );
    new_filename = PbrtImport::StringRoutines::TrimQuotes(new_filename);

    // Make sure the path is absolute.
    if (PbrtImport::Utils::IsAbsolutePath(new_filename)==false)
      new_filename = PbrtImport::StringRoutines::GetDirectoryPath(m_pushed_filenames.back()) + std::string("/") + new_filename;

    if (_ReadLines(new_filename)==false) return false;
    }
  else
    {
    // Check if the new line contains new command.
    // If this is the case we are ready to process the previous command.
    bool new_command = std::binary_search(m_known_commands.begin(), m_known_commands.end(), boost::to_lower_copy(first_word));
    if (new_command)
      {
      if (_ProcessCommand()==false) return false;
      m_buffer = line;
      }
    else
      // Otherwise just add the current line to the buffer.
      m_buffer += std::string(m_buffer.empty() ? "" : " ") + line;
    }

  return true;
  }

/**
* Processes single command from the buffer.
*/
bool PbrtSceneImporter::_ProcessCommand()
  {
  if (m_buffer.empty()) return true;

  // Split the buffer into parts so that each part is either a single word or a text block enclosed by quotes(") or square brackets([]).
  std::vector<std::string> parts;
  if (PbrtImport::StringRoutines::Split(m_buffer, parts, mp_log)==false) return false;
  if (parts.empty()) {ASSERT(0 && "No command name. How did that happen?..");return false;}

  // The name of the command is the first "part".
  std::string command_name = parts[0];

  try 
    {
    if (boost::iequals(command_name, "Accelerator") || boost::iequals(command_name, "Renderer") || boost::iequals(command_name, "Sampler") ||
      boost::iequals(command_name, "SurfaceIntegrator") || boost::iequals(command_name, "VolumeIntegrator"))
      {
      // These commands are ignored.
      }
    else if (boost::iequals(command_name, "Identity"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 0, command_name)==false) return false;
      _pbrtIdentity();
      }
    else if (boost::iequals(command_name, "Translate"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 3, command_name)==false) return false;
      float x = boost::lexical_cast<float>(expanded_parts[0]);
      float y = boost::lexical_cast<float>(expanded_parts[1]);
      float z = boost::lexical_cast<float>(expanded_parts[2]);
      _pbrtTranslate(x,y,z);
      }
    else if (boost::iequals(command_name, "Scale"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 3, command_name)==false) return false;
      float x = boost::lexical_cast<float>(expanded_parts[0]);
      float y = boost::lexical_cast<float>(expanded_parts[1]);
      float z = boost::lexical_cast<float>(expanded_parts[2]);
      _pbrtScale(x,y,z);
      }
    else if (boost::iequals(command_name, "Rotate"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 4, command_name)==false) return false;
      float angle = boost::lexical_cast<float>(expanded_parts[0]);
      float x = boost::lexical_cast<float>(expanded_parts[1]);
      float y = boost::lexical_cast<float>(expanded_parts[2]);
      float z = boost::lexical_cast<float>(expanded_parts[3]);
      _pbrtRotate(angle,x,y,z);
      }
    else if (boost::iequals(command_name, "LookAt"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 9, command_name)==false) return false;
      float ex = boost::lexical_cast<float>(expanded_parts[0]);
      float ey = boost::lexical_cast<float>(expanded_parts[1]);
      float ez = boost::lexical_cast<float>(expanded_parts[2]);
      float lx = boost::lexical_cast<float>(expanded_parts[3]);
      float ly = boost::lexical_cast<float>(expanded_parts[4]);
      float lz = boost::lexical_cast<float>(expanded_parts[5]);
      float ux = boost::lexical_cast<float>(expanded_parts[6]);
      float uy = boost::lexical_cast<float>(expanded_parts[7]);
      float uz = boost::lexical_cast<float>(expanded_parts[8]);
      _pbrtLookAt(ex,ey,ez,lx,ly,lz,ux,uy,uz);
      }
    else if (boost::iequals(command_name, "CoordinateSystem"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 1, command_name)==false) return false;
      _pbrtCoordinateSystem(PbrtImport::StringRoutines::TrimQuotes(expanded_parts[0]));
      }
    else if (boost::iequals(command_name, "CoordSysTransform"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 1, command_name)==false) return false;
      _pbrtCoordSysTransform(PbrtImport::StringRoutines::TrimQuotes(expanded_parts[0]));
      }
    else if (boost::iequals(command_name, "Transform"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 16, command_name)==false) return false;
      float m[16];
      for(size_t i=0;i<16;++i) m[i] = boost::lexical_cast<float>(expanded_parts[i]);
      _pbrtTransform(m);
      }
    else if (boost::iequals(command_name, "ConcatTransform"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 16, command_name)==false) return false;
      float m[16];
      for(size_t i=0;i<16;++i) m[i] = boost::lexical_cast<float>(expanded_parts[i]);
      _pbrtConcatTransform(m);
      }
    else if (boost::iequals(command_name, "AttributeBegin"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 0, command_name)==false) return false;
      _pbrtAttributeBegin();
      }
    else if (boost::iequals(command_name, "AttributeEnd"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 0, command_name)==false) return false;
      _pbrtAttributeEnd();
      }
    else if (boost::iequals(command_name, "Include"))
      {
      ASSERT(0 && "\"Include\" command should have been processed already. How did we get here?..");
      PbrtImport::Utils::LogError(mp_log, "\"Include\" command should have been processed already. How did we get here?..");
      return false;
      }
    else if (boost::iequals(command_name, "WorldBegin"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 0, command_name)==false) return false;
      _pbrtWorldBegin();
      }
    else if (boost::iequals(command_name, "WorldEnd"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 0, command_name)==false) return false;
      _pbrtWorldEnd();
      }
    else if (boost::iequals(command_name, "ObjectBegin"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 1, command_name)==false) return false;
      _pbrtObjectBegin(PbrtImport::StringRoutines::TrimQuotes(expanded_parts[0]));
      }
    else if (boost::iequals(command_name, "ObjectEnd"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 0, command_name)==false) return false;
      _pbrtObjectEnd();
      }
    else if (boost::iequals(command_name, "ObjectInstance"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 1, command_name)==false) return false;
      _pbrtObjectInstance(PbrtImport::StringRoutines::TrimQuotes(expanded_parts[0]));
      }
    else if (boost::iequals(command_name, "TransformTimes"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 2, command_name)==false) return false;
      PbrtImport::Utils::LogWarning(mp_log, "\"TransformTimes\" command is not supported.");
      }
    else if (boost::iequals(command_name, "ActiveTransform"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 1, command_name)==false) return false;
      PbrtImport::Utils::LogWarning(mp_log, "\"ActiveTransform\" command is not supported.");
      }
    else if (boost::iequals(command_name, "NamedMaterial"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 1, command_name)==false) return false;
      _pbrtNamedMaterial(PbrtImport::StringRoutines::TrimQuotes(expanded_parts[0]));
      }
    else if (boost::iequals(command_name, "ReverseOrientation"))
      {
      std::vector<std::string> expanded_parts;
      if (PbrtImport::StringRoutines::ExpandBraces(parts, 1, expanded_parts, mp_log)==false) return false;
      if (_CheckParameters(expanded_parts, 0, command_name)==false) return false;
      _pbrtReverseOrientation();
      }
    else if (boost::iequals(command_name, "Camera"))
      {
      PbrtImport::ParamSet params(PbrtImport::StringRoutines::GetDirectoryPath(m_pushed_filenames.front()));
      if (_ParseParamSet(parts, params)==false) return false;
      _pbrtCamera(PbrtImport::StringRoutines::TrimQuotes(parts[1]), params);
      }
    else if (boost::iequals(command_name, "AreaLightSource"))
      {
      PbrtImport::ParamSet params(PbrtImport::StringRoutines::GetDirectoryPath(m_pushed_filenames.front()));
      if (_ParseParamSet(parts, params)==false) return false;
      _pbrtAreaLightSource(PbrtImport::StringRoutines::TrimQuotes(parts[1]), params);
      }
    else if (boost::iequals(command_name, "Film"))
      {
      PbrtImport::ParamSet params(PbrtImport::StringRoutines::GetDirectoryPath(m_pushed_filenames.front()));
      if (_ParseParamSet(parts, params)==false) return false;
      _pbrtFilm(PbrtImport::StringRoutines::TrimQuotes(parts[1]), params);
      }
    else if (boost::iequals(command_name, "LightSource"))
      {
      PbrtImport::ParamSet params(PbrtImport::StringRoutines::GetDirectoryPath(m_pushed_filenames.front()));
      if (_ParseParamSet(parts, params)==false) return false;
      _pbrtLightSource(PbrtImport::StringRoutines::TrimQuotes(parts[1]), params);
      }
    else if (boost::iequals(command_name, "Material"))
      {
      PbrtImport::ParamSet params(PbrtImport::StringRoutines::GetDirectoryPath(m_pushed_filenames.front()));
      if (_ParseParamSet(parts, params)==false) return false;
      _pbrtMaterial(PbrtImport::StringRoutines::TrimQuotes(parts[1]), params);
      }
    else if (boost::iequals(command_name, "MakeNamedMaterial"))
      {
      PbrtImport::ParamSet params(PbrtImport::StringRoutines::GetDirectoryPath(m_pushed_filenames.front()));
      if (_ParseParamSet(parts, params)==false) return false;
      _pbrtMakeNamedMaterial(PbrtImport::StringRoutines::TrimQuotes(parts[1]), params);
      }
    else if (boost::iequals(command_name, "Texture"))
      {
      PbrtImport::ParamSet params(PbrtImport::StringRoutines::GetDirectoryPath(m_pushed_filenames.front()));
      if (_ParseParamSet(parts, params, 4)==false) return false;
      _pbrtTexture(PbrtImport::StringRoutines::TrimQuotes(parts[1]), PbrtImport::StringRoutines::TrimQuotes(parts[2]), PbrtImport::StringRoutines::TrimQuotes(parts[3]), params);
      }
    else if (boost::iequals(command_name, "PixelFilter"))
      {
      PbrtImport::ParamSet params(PbrtImport::StringRoutines::GetDirectoryPath(m_pushed_filenames.front()));
      if (_ParseParamSet(parts, params)==false) return false;
      _pbrtPixelFilter(PbrtImport::StringRoutines::TrimQuotes(parts[1]), params);
      }
    else if (boost::iequals(command_name, "Shape"))
      {
      PbrtImport::ParamSet params(PbrtImport::StringRoutines::GetDirectoryPath(m_pushed_filenames.front()));
      if (_ParseParamSet(parts, params)==false) return false;
      _pbrtShape(PbrtImport::StringRoutines::TrimQuotes(parts[1]), params);
      }
    else if (boost::iequals(command_name, "Volume"))
      {
      PbrtImport::ParamSet params(PbrtImport::StringRoutines::GetDirectoryPath(m_pushed_filenames.front()));
      if (_ParseParamSet(parts, params)==false) return false;
      _pbrtVolume(PbrtImport::StringRoutines::TrimQuotes(parts[1]), params);
      }
    }
  catch(boost::bad_lexical_cast &)
    {
    PbrtImport::Utils::LogError(mp_log, "Can not parse parameters for the command " + command_name);
    return false;
    }

  m_buffer.clear();
  return true;
  }

bool PbrtSceneImporter::_CheckParameters(const std::vector<std::string> &i_parts, size_t i_num, const std::string &i_command_name) const
  {
  if (i_parts.size() < i_num)
    {
    PbrtImport::Utils::LogError(mp_log, "Not enough parameters for command \"" + i_command_name + std::string("\"."));
    return false;
    }

  if (i_parts.size() > i_num)
    PbrtImport::Utils::LogWarning(mp_log, "Excess parameters provided for command \"" + i_command_name + std::string("\"."));

  return true;
  }

bool PbrtSceneImporter::_ParseParamSet(const std::vector<std::string> &i_parts, PbrtImport::ParamSet &o_params, size_t i_first) const
  {
  o_params.Clear();

  // Each parameter consists of "type name" pair and following value or a set of values enclosed by square brackets([]).
  for(size_t i=i_first;i<i_parts.size();i+=2)
    {
    // First read the "type name" pair.
    std::vector<std::string> type_and_name;
    if (PbrtImport::StringRoutines::Split(PbrtImport::StringRoutines::TrimQuotes(i_parts[i]), type_and_name, mp_log)==false) return false;
    if (type_and_name.size()!=2) {PbrtImport::Utils::LogError(mp_log, "Invalid parameters structure.");return false;}
    std::string type = type_and_name[0], name = type_and_name[1];

    // Check that there is a value following the "type name" pair.
    if (i+1>=i_parts.size() || i_parts[i+1].empty())  {PbrtImport::Utils::LogError(mp_log, "Invalid parameters structure.");return false;}

    // Now read the values.
    std::vector<std::string> values;
    if (i_parts[i+1][0]=='[' && i_parts[i+1][i_parts[i+1].size()-1]==']')
      {
      // If there are enclosing square brackets we remove them and split the string into a set of values.
      if (PbrtImport::StringRoutines::Split(i_parts[i+1].substr(1,i_parts[i+1].size()-2), values, mp_log)==false) return false;
      }
    else
      {
      if (PbrtImport::StringRoutines::Split(i_parts[i+1], values, mp_log)==false) return false;
      }

    if (type=="float")
      {
      std::vector<float> converted_values;
      if (PbrtImport::StringRoutines::ParseFloatValues(values, converted_values, mp_log)==false) return false;
      o_params.AddFloat(name, &converted_values[0], converted_values.size());
      }
    else if (type=="integer")
      {
      std::vector<int> converted_values;
      if (PbrtImport::StringRoutines::ParseIntegerValues(values, converted_values, mp_log)==false) return false;
      o_params.AddInt(name, &converted_values[0], converted_values.size());
      }
    else if (type=="bool")
      {
      std::vector<char> converted_values;
      if (PbrtImport::StringRoutines::ParseBoolValues(values, converted_values, mp_log)==false) return false;
      o_params.AddBool(name, reinterpret_cast<bool*>(&converted_values[0]), converted_values.size());
      }
    else if (type=="string")
      {
      std::vector<std::string> converted_values;
      if (PbrtImport::StringRoutines::ParseStringValues(values, converted_values, mp_log)==false) return false;
      o_params.AddString(name, &converted_values[0], converted_values.size());
      }
    else if (type=="point")
      {
      std::vector<Point3D_d> converted_values;
      if (PbrtImport::StringRoutines::ParsePoint3DValues(values, converted_values, mp_log)==false) return false;
      o_params.AddPoint(name, &converted_values[0], converted_values.size());
      }
    else if (type=="vector")
      {
      std::vector<Vector3D_d> converted_values;
      if (PbrtImport::StringRoutines::ParseVector3DValues(values, converted_values, mp_log)==false) return false;
      o_params.AddVector(name, &converted_values[0], converted_values.size());
      }
    else if (type=="normal")
      {
      std::vector<Vector3D_d> converted_values;
      if (PbrtImport::StringRoutines::ParseVector3DValues(values, converted_values, mp_log)==false) return false;
      o_params.AddNormal(name, &converted_values[0], converted_values.size());
      }
    else if (type=="texture")
      {
      if (values.size()==1)
        {
        std::vector<std::string> texture_names;
        if (PbrtImport::StringRoutines::ParseStringValues(values, texture_names, mp_log)==false) return false;
        o_params.AddTexture(name, texture_names[0]);
        }
      else
        {
        PbrtImport::Utils::LogError(mp_log, "Only one string allowed for \"texture\" parameter.");
        return false;
        }
      }
    else if (type=="spectrum")
      {
      if (values[0][0]=='\"')
        {
        // Read spectrum from the file.
        std::vector<std::string> filenames;
        if (PbrtImport::StringRoutines::ParseStringValues(values, filenames, mp_log)==false) return false;
        o_params.AddSampledSpectrumFiles(name, &filenames[0], filenames.size(), mp_log);
        o_params.AddSampledSpectrumCoefFiles(name, &filenames[0], filenames.size(), mp_log);
        }
      else
        {
        // Read spectrum directly from the values.
        std::vector<float> converted_values;
        if (PbrtImport::StringRoutines::ParseFloatValues(values, converted_values, mp_log)==false) return false;
        if ((converted_values.size()%2)!=0) PbrtImport::Utils::LogWarning(mp_log, "Excess values given with spectrum parameter. Ignoring the excess values.");
        o_params.AddSampledSpectrum(name, &converted_values[0], converted_values.size());
        o_params.AddSampledSpectrumCoef(name, &converted_values[0], converted_values.size());
        }
      }
    else if (type=="color" || type=="rgb")
      {
      std::vector<float> converted_values;
      if (PbrtImport::StringRoutines::ParseFloatValues(values, converted_values, mp_log)==false) return false;
      if ((converted_values.size()%3)!=0) PbrtImport::Utils::LogWarning(mp_log, "Excess values given with RGB parameter. Ignoring the excess values.");
      o_params.AddRGBSpectrum(name, &converted_values[0], converted_values.size());
      o_params.AddRGBSpectrumCoef(name, &converted_values[0], converted_values.size());
      }
    else if (type=="xyz")
      {
      std::vector<float> converted_values;
      if (PbrtImport::StringRoutines::ParseFloatValues(values, converted_values, mp_log)==false) return false;
      if ((converted_values.size()%3)!=0) PbrtImport::Utils::LogWarning(mp_log, "Excess values given with XYZ parameter. Ignoring the excess values.");
      o_params.AddXYZSpectrum(name, &converted_values[0], converted_values.size());
      o_params.AddXYZSpectrumCoef(name, &converted_values[0], converted_values.size());
      }
    else if (type=="blackbody")
      {
      PbrtImport::Utils::LogError(mp_log, "Blackbody spectrum is not supported yet.");
      return false;
      }
    else
      {
      PbrtImport::Utils::LogError(mp_log, std::string("Unknown type \"") + type + std::string("\"."));
      return false;
      }
    }

  return true;
  }