#version 140
#extension GL_ARB_compatibility: enable

out vec4 fcolor;
    
void main() {
    if (gl_FrontFacing) {
        fcolor = vec4(1.0, 1.0, 1.0, 1.0);
    } else {
        fcolor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}