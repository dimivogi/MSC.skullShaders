vs_1_1						// version instruction

// declare register data
dcl_position v0             // v0 is position
dcl_normal v4               // v4 is normal
dcl_texcoord0 v8            // v8 is texture coordinates



m4x4 r0, v0, c0			// transform local space to world space

m3x3 r1 , v4 , c0	// transform the normal
dp3	r1.w , r1 , r1
rsq	r1.w , r1.w
mul r1.xyz , r1.xyz , r1.w	// normalize the normal

sub r2 , c5 , r0	// calculate the light vector
dp3	r2.w , r2 , r2	// calculate the light vector normal
rsq r2.w, r2.w
mul r2.xyz , r2.xyz , r2.w


dp3 r3.w , r1 , r2	// calculate L*V 
mul r3 , r3.w , c8	// calculate diffuse:  Ld*(N.Ldir) 

sub r4 , c4 , r0	// calculate Camera - Vertex
dp3 r4.w , r4 , r4
rsq r4.w , r4.w
mul r4.xyz , r4.xyz , r4.w	// normalize Camera-Vertex
add r4 , r4.xyz , r2.xyz	// add the light vector
dp3 r4.w , r4 , r4
rsq	r4.w , r4.w
mul r4.xyz , r4.xyz , r4.w	// calculate H vector: H = norm(norm(Camera - Vector) + Ldir)

dp3	r5.w , r1 , r4
log	r5.w , r5.w
mul r5.w , r5.w , c13.x
exp r5.w , r5.w
mul r5 , r5.w , c12;	// calculate specular lighting: Specular =  Ls * (N.H)^P


sge r6.w , v0.x , c14.x	// if the vertex is "above" the line specified we get 1.0 and therefore use it as the diffuse lighting colour
slt r7.w , v0.x , c14.x	// else we get 1.0 on the other diffuse colour
mul r6 , c16 , r6.w
mul r7 , c17 , r7.w
mul r6 , r6 , r3
mul r7 , r7 , r3
add r6.xyz , r6.xyz , r7.xyz

add	r7 , r6 , c11	// add ambient and diffuse lighting
add r7 , r7 , r5	// add specular lighting



////Do Vertices Stuff////
mov	oPos , r0
/////////////////////////

////Do Colour Stuff////
mov oD0 , r7			// send ambient , diffuse and specular lighting to the diffuse colour output reg
mov oT0 , v8			// send the default texture coords to the texture output reg
//////////////////////