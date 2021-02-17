#pragma once

#include <string>
#include <vector>

#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>

#include <GL/glew.h>

class shader_t {
public:
   shader_t(const std::string& vertex_code_fname, const std::string& fragment_code_fname);
   ~shader_t();

   void use();
   template<typename T> void set_uniform(const std::string& name, T val);
   template<typename T> void set_uniform(const std::string& name, T val1, T val2);
   template<typename T> void set_uniform(const std::string& name, T val1, T val2, T val3);
   
    void set_vec2(const std::string& name, const glm::vec2& value) const;
    void set_vec2(const std::string& name, float x, float y) const;
    void set_vec3(const std::string& name, const glm::vec3& value) const;
    void set_vec3(const std::string& name, float x, float y, float z) const;
    void set_vec4(const std::string& name, const glm::vec4& value) const;
    void set_vec4(const std::string& name, float x, float y, float z, float w) const;
    void set_mat2(const std::string& name, const glm::mat2& mat) const;
    void set_mat3(const std::string& name, const glm::mat3& mat) const;
    void set_mat4(const std::string& name, const glm::mat4& mat) const;

    GLuint program_id_;
private:
   void check_compile_error();
   void check_linking_error();
   void compile(const std::string& vertex_code, const std::string& fragment_code);
   void link();

   GLuint vertex_id_, fragment_id_;
};
