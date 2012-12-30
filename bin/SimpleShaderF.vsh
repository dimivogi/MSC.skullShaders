vs_1_1						// version instruction

// declare register data
dcl_position v0             // v0 is position
dcl_normal v4               // v4 is normal
dcl_texcoord0 v8            // v8 is texture coordinates

def c25, 2.0 , 1.0 , 0.5 , 0.0



m4x4 r0, v0, c0			// transform local space to world space

m3x3 r1 , v4 , c0	// transform the normal
dp3	r1.w , r1 , r1	// dot product between the normal and itself
rsq	r1.w , r1.w
mul r1.xyz , r1.xyz , r1.w	// normalize the normal

mul r2.x , -r1.x , c25.z	// s = Nx*0.5 + 0.5
add r2.x , r2.x , c25.z

mul r2.y , -r1.y , c25.z	// t = Ny*0.5 0.5
add r2.y , r2.y , c25.z



////Do Vertices Stuff////
mov	oPos , r0
/////////////////////////

////Do Colour Stuff////
//mov oD0 , r2			// send ambient , diffuse and specular lighting to the diffuse colour output reg
mov oT0 , r2.xy			// send the default texture coords to the texture output reg
//////////////////////