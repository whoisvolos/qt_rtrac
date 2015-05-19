#version 150 core

in highp vec3 vert;
in highp vec3 vertNormal;
out highp vec4 fragColor;
uniform highp vec3 lightPos;

void main() {
    // Normalize normal :)
    highp vec3 normal = normalize(vertNormal);
    // Light dir
    highp vec3 s = normalize(lightPos - vert);
    // View dir (camera @ (0,0,0))
    highp vec3 v = normalize(-vert);
    // Reflection using normal
    highp vec3 r = reflect(-s, normal);
    // Diffuse intensity
    highp vec3 di = vec3(max(dot(s, normal), 0.0));
    // Specilar intensity
    highp vec3 si = vec3(0.0);
    if (dot(s, normal) > 0.0) {
        si = vec3(pow(max(dot(r, v), 0.0), 256.0));
    }
    // Final intensity
    fragColor = vec4(clamp(0.9 * di + 1.0 * si, 0.0, 1.0), 1.0);
}