
GLfloat _quadVerts[] = {
    // X     Y     Z     U    V
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
     -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      1.0f,-1.0f, 0.0f, 1.0f, 0.0f,
     -1.0f,-1.0f, 0.0f, 0.0f, 0.0f
};

GLint _quadIndices[] = {
    0, 1, 2,
    2, 1, 3
};

const GLchar *vertex_shader =
    R"raw(
        #version 330
        layout(location = 0) in vec3 Pos;
        layout(location = 1) in vec2 UV;
        out vec2 Frag_UV;
        void main(){
            Frag_UV = UV;
            gl_Position = vec4(Pos, 1.0);
        }
    )raw";

const GLchar *frag_shader =
    R"raw(
        #version 330
        in vec2 Frag_UV;
        layout(location = 0) out vec4 Out_Color;
        void main()
        {
           Out_Color = vec4(Frag_UV, 0.0, 1.0);
        }
    )raw";

const GLchar *error_frag_shader =
    R"raw(
        #version 330
        in vec2 Frag_UV;
        layout(location = 0) out vec4 Out_Color;
        void main()
        {
           Out_Color = vec4(1.0, 0.0, 1.0, 1.0);
        }
    )raw";
