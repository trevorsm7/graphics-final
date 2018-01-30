#version 410

uniform sampler2D u_ambientMap;
uniform sampler2D u_normalMap;
uniform sampler2D u_specularMap;
uniform sampler2D u_shadowMap;

uniform mat4 u_modelView;

in vec3 v_shadowPos;
in vec3 v_vertex;
in vec2 v_texCoord;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;

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
    float u_shininess = 5.0;
  
    vec3 u_ambient = texture(u_ambientMap, v_texCoord).rgb;
    vec3 u_diffuse = u_ambient;
    vec3 u_normal = 2.0 * texture(u_normalMap, v_texCoord).rgb - 1.0;
    vec3 u_specular = texture(u_specularMap, v_texCoord).rgb;
  
    //mat3 TBN = transpose(mat3(normalize(v_tangent), normalize(v_bitangent), normalize(v_normal)));
    mat3 TBN = mat3(normalize(v_tangent), normalize(v_bitangent), normalize(v_normal));
    vec3 normal = TBN * normalize(u_normal);
    //vec3 normal = normalize(v_normal);
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
