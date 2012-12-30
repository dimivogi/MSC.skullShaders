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

sub r2 , c5 , r0	// calculate the light 1 vector
dp3	r2.w , r2 , r2	// calculate the light 1 vector normal
rsq r2.w, r2.w
mul r2.xyz , r2.xyz , r2.w

sub r3 , c6 , r0	// calculate the light 2 vector
dp3	r3.w , r3 , r3	// calculate the light 2 vector normal
rsq r3.w, r3.w
mul r3.xyz , r3.xyz , r3.w

sub r4 , c7 , r0	// calculate the light 3 vector
dp3	r4.w , r4 , r4	// calculate the light 3 vector normal
rsq r4.w, r4.w
mul r4.xyz , r4.xyz , r4.w


dp3 r5.w , r1 , r2	// calculate L1*V 
mul r5 , r5.w , c8	// calculate diffuse1:  Ld1*(N.Ldir1)

dp3 r6.w , r1 , r3 // calculate L2*V
mul r6 , r6.w , c9 // calculate diffuse2:  Ld2*(N.Ldir2)

dp3 r7.w , r1 , r4 // calculate L3*V
mul r7 , r7.w , c10 // calculate diffuse 2:  Ld3*(N.Ldir3)

mov r8 , c11 
mul r8 , r8 , c15	// add the ambient lighting: ( ka*La )
mad	r8 , r5 , c16 , r8	// add diffuse lighting: (kd*Ld1) + Ca
mad r8 , r6 , c16 , r8	// add diffuse2 lighting: (Kd*Ld2) + Cd1 + Ca
mad r8 , r7 , c16 , r8	// add diffuse3 lighting: (kd*Ld3) + Cd2 + Cd1 + Ca



////Do Vertices Stuff////
mov	oPos , r0
/////////////////////////

////Do Colour Stuff////
mov oD0 , r8			// send ambient , diffuse and specular lighting to the diffuse colour output reg
mov oT0 , v8			// send the default texture coords to the texture output reg
//////////////////////
