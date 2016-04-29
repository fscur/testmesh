#version 450

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;

uniform sampler2D inputTexture;
float radius = 4.0;
float resolution = 1600.0;
vec2 dir = vec2(1.0, 0.0);
out vec4 fragColor;

//void main(void)
//{
//    //fragColor = texture2D(inputTexture, gl_FragCoord.xy) * 0.8;
//    //fragColor = texture2D(inputTexture, fragTexCoord);
//    //fragColor = vec4(0.0, 0.0, 1.0, 1.0);

//    vec4 sum = vec4(0.0);

//    //our original texcoord for this fragment
//    vec2 tc = fragTexCoord;

//    //the amount to blur, i.e. how far off center to sample from 
//    //1.0 -> blur by one pixel
//    //2.0 -> blur by two pixels, etc.
//    float blur = radius/resolution; 

//    //the direction of our blur
//    //(1.0, 0.0) -> x-axis blur
//    //(0.0, 1.0) -> y-axis blur
//    float hstep = dir.x;
//    float vstep = dir.y;

//    //apply blurring, using a 9-tap filter with predefined gaussian weights

//    sum += texture2D(inputTexture, vec2(tc.x - 4.0*blur*hstep, tc.y - 4.0*blur*vstep)) * 0.0162162162;
//    sum += texture2D(inputTexture, vec2(tc.x - 3.0*blur*hstep, tc.y - 3.0*blur*vstep)) * 0.0540540541;
//    sum += texture2D(inputTexture, vec2(tc.x - 2.0*blur*hstep, tc.y - 2.0*blur*vstep)) * 0.1216216216;
//    sum += texture2D(inputTexture, vec2(tc.x - 1.0*blur*hstep, tc.y - 1.0*blur*vstep)) * 0.1945945946;

//    sum += texture2D(inputTexture, vec2(tc.x, tc.y)) * 0.2270270270;

//    sum += texture2D(inputTexture, vec2(tc.x + 1.0*blur*hstep, tc.y + 1.0*blur*vstep)) * 0.1945945946;
//    sum += texture2D(inputTexture, vec2(tc.x + 2.0*blur*hstep, tc.y + 2.0*blur*vstep)) * 0.1216216216;
//    sum += texture2D(inputTexture, vec2(tc.x + 3.0*blur*hstep, tc.y + 3.0*blur*vstep)) * 0.0540540541;
//    sum += texture2D(inputTexture, vec2(tc.x + 4.0*blur*hstep, tc.y + 4.0*blur*vstep)) * 0.0162162162;

//    fragColor = vec4(sum.rgb, 1.0);
//}

 
float rt_w = 1600.0; // render target width
float rt_h = 900.0; // render target height
float vx_offset = 16.0;
 
float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );
 
void main() 
{ 
  vec3 tc = vec3(1.0, 0.0, 0.0);
  if (fragTexCoord.x<(vx_offset-0.01))
  {
    vec2 uv = fragTexCoord.xy;
    tc = texture2D(inputTexture, uv).rgb * weight[0];
    for (int i=1; i<3; i++) 
    {
      tc += texture2D(inputTexture, uv + vec2(offset[i])/rt_w, 0.0).rgb \
              * weight[i];
      tc += texture2D(inputTexture, uv - vec2(offset[i])/rt_w, 0.0).rgb \
              * weight[i];
    }
  }
  else if (fragTexCoord.x>=(vx_offset+0.01))
  {
    tc = texture2D(inputTexture, fragTexCoord.xy).rgb;
  }

  fragColor = vec4(tc, 1.0);
}