#version 410

//uniform vec3 u_lightPos;
//uniform vec3 u_lightColor;
//uniform vec3 u_spotDir;
//uniform float u_spotCutoff; // use cosine
//uniform float u_spotExponent;

//uniform vec3 u_ambient;
//uniform vec3 u_diffuse;
//uniform vec3 u_specular;
//uniform float u_shininess;

uniform sampler2D u_shadowMap;

in vec3 v_shadowPos;
in vec3 v_vertex;
in vec3 v_normal;

out vec4 f_color;

void main()
{
    float depth = texture(u_shadowMap, v_shadowPos.xy).x;
    float visibility = (depth < v_shadowPos.z) ? 0.0 : 1.0;
  
    vec3 u_lightPos = vec3(0.1, -0.1, 0.1);
    vec3 u_lightColor = vec3(0.6, 0.6, 0.5);
    vec3 u_spotDir = vec3(0.0, 0.0, -1.0);
    float u_spotCutoff = cos(30.0 * 3.14159 / 180.0);
    float u_spotExponent = 75.0;
    vec3 u_ambient = vec3(0.8, 0.8, 0.8);
    vec3 u_diffuse = vec3(0.8, 0.8, 0.8);
    vec3 u_specular = vec3(0.8, 0.8, 0.8);
    float u_shininess = 5.0;
  
    vec3 normal = normalize(v_normal);
    vec3 color = vec3(0.0, 0.0, 0.0);
    vec3 ambient = u_lightColor * u_ambient;
  
    // Get distance and direction from vertex to light
    vec3 lightVector = u_lightPos - v_vertex;
    float d = length(lightVector);
    float attenuation = 1.0 + (0.1 * d*d);
    lightVector = normalize(lightVector);
    
    // Compute diffuse component
    float NdotL = max(dot(normal, lightVector), 0.0);
    vec3 diffuse = u_lightColor * u_diffuse * NdotL;
    
    // Only compute diffuse and specular inside spotlight cone
    float spotEffect = dot(u_spotDir, -lightVector);
    if (NdotL > 0.0 && spotEffect > u_spotCutoff)
    {
        //spotEffect = pow(spotEffect, u_spotExponent);
        
        // Compute specular component
        vec3 halfVector = normalize(normalize(-v_vertex) + lightVector);
        float NdotHV = max(dot(normal, halfVector), 0.0);
        vec3 specular = u_lightColor * u_specular * pow(NdotHV, u_shininess);
        
        // Compute attenuated sum of component
        //color += pow(spotEffect, u_spotExponent) * (diffuse + specular) / attenuation;
        color += visibility * pow(spotEffect, u_spotExponent) * (specular + 0.5 * diffuse) / attenuation;
        //color += vec4(u_lightColor, 1.0) * u_ambient / attenuation;
    }
  
    color += (0.3 * diffuse + 0.15 * ambient) / attenuation;
  
    f_color = vec4(color, 1.0);
}
