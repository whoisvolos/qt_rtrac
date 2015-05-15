varying highp vec3 vert;
varying highp vec3 vertNormal;
uniform highp vec3 lightPos;
uniform highp vec3 cameraPos;

void main() {
    // Normalize normal :)
    vec3 normal = normalize(vertNormal);
    // Light dir
    vec3 s = normalize(lightPos - vert);
    // View dir (camera @ (0,0,0))
    vec3 v = normalize(-vert);
    // Reflection using normal
    vec3 r = reflect(-s, normal);
    // Diffuse intensity
    vec3 di = vec3(max(dot(s, normal), 0.0));
    // Specilar intensity
    vec3 si = vec3(0.0);
    if (dot(s, normal) > 0.0) {
        si = vec3(pow(max(dot(r, v), 0.0), 64.0));
    }
    // Final intensity
    gl_FragColor = vec4(clamp(0.9 * di + 1.0 * si, 0.0, 1.0), 1.0);
}