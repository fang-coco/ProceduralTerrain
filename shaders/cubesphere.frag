#version 400 core

layout (location = 0) out vec4 fragColor;

in FragData {
  vec3 color;
  vec3 triPosition;
  vec2 texCoords;
  vec4 patchDistance;
  vec3 eyePosition;
  vec3 normal;
//  vec3 worldPosition;
  flat int idxIntoSatelliteTextureInfo;
} fragIn;

uniform sampler2DArray satelliteImagery;
uniform bool useWireFrame;
uniform bool useImagery;

struct TextureInfo
{
  vec4 layerScaleOffsets[4];
  vec4 extentMinMax[4];
};
// sizeof(TextureInfo) is 128 bytes with std140

// UBO minimum max size should be 16384 bytes, we've assumed 65536
layout(std140) uniform ImageryMappingBlock
{
    TextureInfo info[512]; // At most we can have 65536 / 128 == 512 (16384 / 32 == 128 if we were to strictly follow the GL specs) different textureInfo
} satelliteUniformBuffer;

float amplify(float d, float scale, float offset)
{
  d = scale * d + offset;
  d = clamp(d, 0, 1);
  d = 1 - exp2(-2*d*d);
  return d;
}


void main()
{
  if (useImagery) {
      fragColor = vec4(0.0);
      TextureInfo satelliteInfo = satelliteUniformBuffer.info[fragIn.idxIntoSatelliteTextureInfo];

      for (int i = 0; i < 4; ++i) {
           // Satellite
           float layer = satelliteInfo.layerScaleOffsets[i].x;
           if (layer >= 0.0) {
             float scale = satelliteInfo.layerScaleOffsets[i].y;
             vec2 offset = satelliteInfo.layerScaleOffsets[i].zw;
             vec3 texCoords = vec3(fragIn.texCoords * scale + offset, layer);
             vec4 color = texture(satelliteImagery, texCoords);

             // We need alpha to be 0 when the texture coordinates are beyond
             // or below the extent of the image
             // step -> 0.0 is returned if x[i] < edge[i], and 1.0 is returned otherwise
             vec2 stepMin = step(satelliteInfo.extentMinMax[i].xy, texCoords.xy);
             vec2 stepMax = step(vec2(0, 0), texCoords.xy - satelliteInfo.extentMinMax[i].zw);
             float alpha = stepMin.x * stepMin.y * stepMax.x * stepMax.y;

             fragColor += vec4(color.rgb, color.a * alpha);
           }
          // Add some lighting
//          fragColor = vec4(adsModel(fragIn.eyePosition, normalize(fragIn.normal), fragColor.rgb), 1.0);
      }
  }
  else {
      fragColor = vec4(fragIn.color, 1.0);
  }

  if (useWireFrame) {
    float d1 = min(min(fragIn.triPosition.x, fragIn.triPosition.y), fragIn.triPosition.z);
    float d2 = min(min(min(fragIn.patchDistance.x, fragIn.patchDistance.y), fragIn.patchDistance.z), fragIn.patchDistance.w);
    d1 = 1 - amplify(d1, 50, -0.5);
    d2 = amplify(d2, 50, -0.5);
    fragColor = vec4(d2 * fragColor.xyz + d1 * d2 * -fragColor.xzy, 1.0);
  }
}
