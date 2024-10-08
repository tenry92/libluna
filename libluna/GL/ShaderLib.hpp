#pragma once

#include <map>
#include <string>
#include <vector>

#include <libluna/GL/Shader.hpp>
#include <libluna/InputStream.hpp>
#include <libluna/Logger.hpp>
#include <libluna/String.hpp>

#if !defined(NDEBUG) && !defined(__SWITCH__)
#define INSERT_GLSL_LINE_STATEMENTS
#endif

namespace Luna::GL {
  class ShaderLib {
    public:
    void registerShader(
        const std::string &filename, std::unique_ptr<Luna::InputStream> source
    ) {
      std::vector<char> contents(source->getSize());
      source->read(contents.data(), source->getSize());
      mRegisteredShaders.emplace(
          filename, Luna::String(contents.data(), source->getSize())
      );
    }

    GL::Shader
    compileShader(const std::string &vertex, const std::string &fragment) {
      auto vertexLines = getShaderLines(vertex);
      auto fragmentLines = getShaderLines(fragment);

      vertexLines.emplace_front("#version 330 core");
      fragmentLines.emplace_front("#version 330 core");

      auto vertexSource =
          Luna::String::join("\n", vertexLines.begin(), vertexLines.end());
      auto fragmentSource =
          Luna::String::join("\n", fragmentLines.begin(), fragmentLines.end());

      return GL::Shader(vertexSource.c_str(), fragmentSource.c_str());
    }

    std::list<Luna::String> getShaderLines(const std::string &filename) {
      if (!mRegisteredShaders.count(filename)) {
        logError("unknown shader file {}", filename);
        return {};
      }

      auto sourceLines = mRegisteredShaders.at(filename).split('\n');

      std::list<Luna::String> output;
#ifdef INSERT_GLSL_LINE_STATEMENTS
      output.emplace_back(Luna::String("#line 1 \"{}\"").format(filename));
      int lineNumber = 1;
#endif

      for (auto &&line : sourceLines) {
#ifdef INSERT_GLSL_LINE_STATEMENTS
        ++lineNumber;
#endif

        if (line.startsWith("#include ")) {
          auto nameOffsetStart = line.indexOf('"').value() + 1;
          auto nameOffsetEnd = line.indexOf('"', nameOffsetStart).value();
          auto fileToInclude = line.subString(nameOffsetStart, nameOffsetEnd);
          auto includedLines = getShaderLines(fileToInclude.s_str());

          for (auto &&includedLine : includedLines) {
            output.emplace_back(includedLine);
          }

#ifdef INSERT_GLSL_LINE_STATEMENTS
          output.emplace_back(
              Luna::String("#line {} \"{}\"").format(lineNumber, filename)
          );
#endif
        } else {
          if (!line.startsWith("#version ")) {
            output.emplace_back(line);
          }
        }
      }

      return output;
    }

    private:
    std::map<std::string, Luna::String> mRegisteredShaders;
  };
} // namespace Luna::GL
