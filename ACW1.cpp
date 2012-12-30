//-----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////DIRECTX 9////////////////////////////////////////////////
///////////////////////////////////ACW 1 - BASED ON X FILES TUTORIAL 2////////////////////////////////
///////////////////////////////////LOADING AND DISPLAYING AN X FILE///////////////////////////////////
/////////////////////////////////////////USEING VERTEX SHADERS////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////UNIVERSITY OF HULL////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////-----------------
#include	"stdafx.h"
#include	<Windows.h>
#include	<mmsystem.h>
#include	<d3dx9.h>
#include	<iostream>
#include	<string>
using namespace std;



struct D3DVERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	FLOAT       tu, tv;
};

#define D3DFVF_D3DVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)



//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

LPDIRECT3D9				g_pD3D           = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9		g_pd3dDevice     = NULL; // Our rendering device

LPD3DXMESH				g_pMesh          = NULL; // Our mesh object in sysmem
D3DMATERIAL9*			g_pMeshMaterials = NULL; // Materials for our mesh
LPDIRECT3DTEXTURE9*		g_pMeshTextures  = NULL; // Textures for our mesh
DWORD					g_dwNumMaterials = 0L;   // Number of mesh materials
LPDIRECT3DTEXTURE9		g_pTexture   = NULL;
LPDIRECT3DTEXTURE9		g_pTexture2   = NULL;	// cartoon rendering texture
LPDIRECT3DTEXTURE9		marbleTexture = NULL;	// marble texture
LPDIRECT3DTEXTURE9		backgroundTexture = NULL;	// reflection background texture
LPDIRECT3DVERTEXSHADER9	g_pVertexShader = NULL;////VS TUTORIAL STUFF
LPDIRECT3DVERTEXSHADER9	g_pVertexShader2 = NULL;
LPDIRECT3DVERTEXSHADER9	g_pVertexShader3 = NULL;
LPDIRECT3DVERTEXSHADER9	g_pVertexShader4 = NULL;
LPDIRECT3DVERTEXSHADER9	g_pVertexShader5 = NULL;
LPDIRECT3DVERTEXSHADER9	g_pVertexShader6 = NULL;
LPDIRECT3DVERTEXBUFFER9	g_pMeshVB;
LPDIRECT3DINDEXBUFFER9	g_pMeshIB;
DWORD					g_dwNumVertices;
DWORD					g_dwNumFaces;

D3DXMATRIX				g_matProj;  
D3DXMATRIX				g_matView;
D3DXMATRIX				g_matWorld;

bool					firstSet = true;
D3DXVECTOR3				cameraPos;
D3DXVECTOR2				mousePos;
D3DXVECTOR3				lightPos;
int						currentShader = 1;
int						maxShaders = 6;

wstring					shaders[6] = {
							L"SimpleShaderA.vsh" ,	// diffuse and specular lighting shader
							L"SimpleShaderB.vsh" ,	// multiple diffuse light shader
							L"SimpleShaderC.vsh" ,	// cartoon rendering texture shader
							L"SimpleShaderD.vsh" ,	// two tone painting shader ( with diffuse and specular lighting )
							L"SimpleShaderE.vsh" ,	// marble texture shader (with diffuse and specular lighting )
							L"SimpleShaderF.vsh"	// reflection shader
						};



//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
	// Create the D3D object.
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	// Set up the structure used to create the D3DDevice. Since we are now
	// using more complex geometry, we will create a device with a zbuffer.
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// Create the D3DDevice
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}

	// Turn on the zbuffer
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	//Turn off culling, so we see the front and back of the triangle
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	g_pd3dDevice->LightEnable( 0, TRUE );
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );


	////VS TUTORIAL STUFF

	LPD3DXBUFFER pCode;                  // Buffer with the assembled shader code				////VS TUTORIAL STUFF
	// This is our error buffer
	LPD3DXBUFFER pErrorMsgs = 0;             // Buffer with error messages							////VS TUTORIAL STUFF


	for ( int i = 0;  i < maxShaders;  i++ ) // load all the shader files
	{
		if(FAILED(D3DXAssembleShaderFromFile( shaders[i].c_str(),  NULL, NULL, 0,					////VS TUTORIAL STUFF
			&pCode, &pErrorMsgs )))	{   
				// Error(s) have occured.   
				// Allocate a character buffer the size of the ID3DXBuffer  
				if(pErrorMsgs != 0) {
					char* data = new char[pErrorMsgs->GetBufferSize()];   
					// Copy the buffer data over   
					memcpy( data, pErrorMsgs->GetBufferPointer(), pErrorMsgs->GetBufferSize() );   
					// Open a file and output the error string(s)   
					FILE* file;
					fopen_s(&file,"errors.txt", "w" );   
					fprintf( file, "%s", data );   
					fclose( file );   
					// Release allocated objects   
					delete[] data;   
					pErrorMsgs->Release();   
					MessageBox(hWnd,L"see errors.txt",L"vertex assemble problem",NULL);
				}
				return E_FAIL;
		}

		switch ( i )	// create the vertex shader and assosiate it with the proper variable
		{
			case 0 :	g_pd3dDevice->CreateVertexShader((DWORD*)pCode->GetBufferPointer(),	&g_pVertexShader);
			case 1 :	g_pd3dDevice->CreateVertexShader((DWORD*)pCode->GetBufferPointer(),	&g_pVertexShader2);
			case 2 :	g_pd3dDevice->CreateVertexShader((DWORD*)pCode->GetBufferPointer(),	&g_pVertexShader3);
			case 3 :	g_pd3dDevice->CreateVertexShader((DWORD*)pCode->GetBufferPointer(),	&g_pVertexShader4);
			case 4 :	g_pd3dDevice->CreateVertexShader((DWORD*)pCode->GetBufferPointer(),	&g_pVertexShader5);
			case 5 :	g_pd3dDevice->CreateVertexShader((DWORD*)pCode->GetBufferPointer(),	&g_pVertexShader6);
		}
		pCode->Release();
	}



	cameraPos.x = 0.0f;
	cameraPos.y = 15.0f;
	cameraPos.z = 20.0f;

	lightPos.x = -10.0f;
	lightPos.y = 0.0f;
	lightPos.z = 3.0f;



	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Load the mesh and build the material and texture arrays
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
	LPD3DXBUFFER pD3DXMtrlBuffer;
	LPDIRECT3DVERTEXBUFFER9 pMeshSourceVB;
	LPDIRECT3DINDEXBUFFER9  pMeshSourceIB;
	D3DVERTEX*              pSrc;
	D3DVERTEX*              pDst;


	// load the textures we are going to be using
	if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, L"cartoonpallet-white-to-black.bmp", &g_pTexture ) ) )
		MessageBox(NULL, L"Texture Load Problem", NULL, NULL);

	if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, L"cartoonpallet-black-to-white.bmp", &g_pTexture2 ) ) )
		MessageBox(NULL, L"Texture Load Problem", NULL, NULL);

	if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, L"marble.bmp",	&marbleTexture ) ) )
		MessageBox(NULL, L"Texture Load Problem", NULL, NULL);

	if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, L"background.jpg",	&backgroundTexture ) ) )
		MessageBox(NULL, L"Texture Load Problem", NULL, NULL);



	// Load the mesh from the specified file
	if( FAILED( D3DXLoadMeshFromX( L"skull.x", D3DXMESH_SYSTEMMEM, 
		g_pd3dDevice, NULL, 
		&pD3DXMtrlBuffer, NULL, &g_dwNumMaterials, 
		&g_pMesh ) ) )

		g_pd3dDevice->SetFVF(D3DFVF_D3DVERTEX );
	g_dwNumVertices  = g_pMesh->GetNumVertices();
	g_dwNumFaces     = g_pMesh->GetNumFaces();

	//Clone the mesh to set the FVF
	LPD3DXMESH pTempSysMemMesh = NULL;

	if( FAILED( g_pMesh->CloneMeshFVF( D3DXMESH_SYSTEMMEM, D3DFVF_D3DVERTEX,
		g_pd3dDevice, &pTempSysMemMesh ) ) )
		MessageBox(NULL,L"Mesh clone problem",NULL,NULL);
	g_pMesh->Release();


	g_pMesh = pTempSysMemMesh;

	//Compute normals in case the meshes have them
	if( g_pMesh )
		D3DXComputeNormals( g_pMesh, NULL );


	//Meshes cloned 

	if( FAILED(g_pd3dDevice->CreateVertexBuffer( g_dwNumVertices * sizeof(D3DVERTEX),
		D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED,
		&g_pMeshVB, NULL )))
		MessageBox(NULL,L"Vertex buffer create problem",NULL,NULL);

	if( FAILED(g_pd3dDevice->CreateIndexBuffer( g_dwNumFaces * 3 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16, D3DPOOL_MANAGED,
		&g_pMeshIB, NULL )))
		MessageBox(NULL,L"Index buffer create problem",NULL,NULL);
	g_pMesh->GetVertexBuffer(&pMeshSourceVB);
	g_pMeshVB->Lock( 0, 0, (void**)&pDst, 0 );
	pMeshSourceVB->Lock( 0, 0, (void**)&pSrc, 0 );
	memcpy( pDst, pSrc, g_dwNumVertices * sizeof(D3DVERTEX) );
	g_pMeshVB->Unlock();
	pMeshSourceVB->Unlock();
	pMeshSourceVB->Release();

	g_pMesh->GetIndexBuffer(&pMeshSourceIB);
	g_pMeshIB->Lock( 0, 0, (void**)&pDst, 0 );
	pMeshSourceIB->Lock( 0, 0, (void**)&pSrc, 0 );
	memcpy( pDst, pSrc, g_dwNumFaces * 3 * sizeof(WORD));
	g_pMeshIB->Unlock();
	pMeshSourceIB->Unlock();
	pMeshSourceIB->Release();



	//// Done with the material buffer
	pD3DXMtrlBuffer->Release();

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	// release all textures used
	if ( g_pTexture != NULL )
		g_pTexture->Release();

	if ( g_pTexture2 != NULL )
		g_pTexture2->Release();

	if ( marbleTexture != NULL )
		marbleTexture->Release();

	if ( backgroundTexture != NULL )
		backgroundTexture->Release();

	if( g_pMeshMaterials != NULL ) 
		delete[] g_pMeshMaterials;

	if( g_pMeshTextures )
	{
		for( DWORD i = 0; i < g_dwNumMaterials; i++ )
		{
			if( g_pMeshTextures[i] )
				g_pMeshTextures[i]->Release();
		}
		delete[] g_pMeshTextures;
	}
	if( g_pMesh != NULL )
		g_pMesh->Release();

	if( g_pd3dDevice != NULL )
		g_pd3dDevice->Release();

	if( g_pD3D != NULL )
		g_pD3D->Release();
}



//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID SetupMatrices()
{
	DWORD time = timeGetTime();

	// For our world matrix, we will just leave it as the identity
	D3DXMatrixIdentity( &g_matWorld );
	D3DXMatrixRotationYawPitchRoll( &g_matWorld,time/1000.0f,time/8000.0f,time/8000.0f );

	g_pd3dDevice->SetTransform( D3DTS_WORLD, &g_matWorld );

	// Set up our view matrix. A view matrix can be defined given an eye point,
	// a point to lookat, and a direction for which way is up. Here, we set the
	// eye five units back along the z-axis and up three units, look at the
	// origin, and define "up" to be in the y-direction.
	D3DXMatrixLookAtLH( &g_matView, &cameraPos,
		&D3DXVECTOR3( 0.0f, 0.0f, 0.0f ),
		&D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &g_matView );

	// For the projection matrix, we set up a perspective transform (which
	// transforms geometry from 3D view space to 2D viewport space, with
	// a perspective divide making objects smaller in the distance). To build
	// a perpsective transform, we need the field of view (1/4 pi is common),
	// the aspect ratio, and the near and far clipping planes (which define at
	// what distances geometry should be no longer be rendered).
	D3DXMatrixPerspectiveFovLH( &g_matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &g_matProj );
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render(HWND hWnd)
{
	// Clear the backbuffer and the zbuffer
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
		D3DCOLOR_XRGB(40,40,40), 1.0f, 0 );


	g_pd3dDevice->BeginScene();



	g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	SetupMatrices();

	// Set up the vertex shader constants

	D3DXMATRIX mat;
	D3DXMATRIX mat1;
	D3DXMatrixMultiply( &mat, &g_matWorld, &g_matView );
	D3DXMatrixTranspose( &mat1, &mat );

	D3DXMatrixMultiply( &mat, &mat, &g_matProj );
	D3DXMatrixTranspose( &mat, &mat );

	// send the world matrix to the shader into constant register 0 (actually 0 to 3 due to the size of a matrix)
	g_pd3dDevice->SetVertexShaderConstantF(0, (float*)&mat,  4);


	// send the camera position to constant register 4
	float	fCamera[3] = { cameraPos.x , cameraPos.y , cameraPos.z };
	g_pd3dDevice->SetVertexShaderConstantF(4, (float*)fCamera,1);

	// send the light position into constant register 5
	float	fLightPosition[3] = { lightPos.x , lightPos.y , lightPos.z };
	g_pd3dDevice->SetVertexShaderConstantF(5, (float*)fLightPosition,1);

	// send the light 2  position into constant register 6
	float	fLightPosition2[3] = { -5.0f , 0.0f , 2.0f };
	g_pd3dDevice->SetVertexShaderConstantF(6, (float*)fLightPosition2,1);

	// send the light 3 position into constant register 7
	float	fLightPosition3[3] = { 0.0f , 5.0f , -2.0f };
	g_pd3dDevice->SetVertexShaderConstantF(7, (float*)fLightPosition3,1);

	// send the diffuse color into constant register 8
	float	diffuseColor[4] = { 0.90f, 0.90f, 0.90f, 1.0f };
	g_pd3dDevice->SetVertexShaderConstantF(8, (float*)diffuseColor, 1);

	// send the diffuse color 2 into constant register 9
	float	diffuseColor2[4] = { 0.95f, 0.20f, 0.10f, 1.0f };
	g_pd3dDevice->SetVertexShaderConstantF(9, (float*)diffuseColor2, 1);

	// send the diffuse color 3  into constant register 10
	float	diffuseColor3[4] = { 0.10f, 0.20f, 0.95f, 1.0f };
	g_pd3dDevice->SetVertexShaderConstantF(10, (float*)diffuseColor3, 1);

	// send the ambient colour into constant register 11
	float	fAmbient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
	g_pd3dDevice->SetVertexShaderConstantF(11, (float*)fAmbient,1);

	// send the specular lighting colour into constant register 12
	float	fSpecularLight[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	g_pd3dDevice->SetVertexShaderConstantF(12, (float*)fSpecularLight,1);

	// send the reflection constant to constant register 13
	float	fReflection = 10.0f;
	g_pd3dDevice->SetVertexShaderConstantF(13,&fReflection,1);

	// send the separation line to constant register 14
	float	separationLine = 0.0f;
	g_pd3dDevice->SetVertexShaderConstantF(14,&separationLine,1);

	// send ambient material to the constant register 15
	float	ambientMaterial[4] = { 0.25f , 0.25f , 0.25f , 1.0f};
	g_pd3dDevice->SetVertexShaderConstantF(15,(float*)ambientMaterial,1);

	// send diffuse material one to constant register 16
	float	diffuseMaterial1[4] = { 0.75f , 0.60f , 0.40f , 1.0f};
	g_pd3dDevice->SetVertexShaderConstantF(16,(float*)diffuseMaterial1,1);

	// send diffuse material two to constant register 17
	float	diffuseMaterial2[4] = { 0.95f , 0.20f , 0.1f , 1.0f };
	g_pd3dDevice->SetVertexShaderConstantF(17,(float*)diffuseMaterial2,1);

	// send specular material to the constant register 18
	float	specularMaterial[4] = { 1.0f , 1.0f , 1.0f , 1.0f};
	g_pd3dDevice->SetVertexShaderConstantF(18,(float*)specularMaterial,1);



	//   Render the vertex buffer contents 
	g_pd3dDevice->SetFVF(D3DFVF_D3DVERTEX );
	g_pd3dDevice->SetStreamSource( 0, g_pMeshVB, 0, sizeof(D3DVERTEX) );
	g_pd3dDevice->SetIndices( g_pMeshIB);
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	g_pd3dDevice->SetTexture( 0, g_pTexture ); 

	switch ( currentShader )  // activate the appropiate vertex shader
	{
		case 1:	g_pd3dDevice->SetVertexShader( g_pVertexShader); break;
		case 2:	g_pd3dDevice->SetVertexShader( g_pVertexShader2); break;
		case 3:	g_pd3dDevice->SetVertexShader( g_pVertexShader3); g_pd3dDevice->SetTexture( 0, g_pTexture2 );break;
		case 4:	g_pd3dDevice->SetVertexShader( g_pVertexShader4); break;
		case 5:	g_pd3dDevice->SetVertexShader( g_pVertexShader5); g_pd3dDevice->SetTexture( 0, marbleTexture ); break;
		case 6:	g_pd3dDevice->SetVertexShader( g_pVertexShader6); g_pd3dDevice->SetTexture( 0, backgroundTexture );break;
	};


	g_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,0,
		0, g_dwNumVertices,
		0, g_dwNumFaces );

	// End the scene.
	g_pd3dDevice->EndScene();

	// Present the backbuffer contents to the display
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

void	KeyPress( WPARAM key , LPARAM flag )
{
	if ( VK_ESCAPE == key )
	{
		Cleanup();
		PostQuitMessage( 0 );
	}
	else if ( VK_SPACE == key )
	{

		currentShader++;
		if ( currentShader > maxShaders )
			currentShader = 1;
	}
	else if ( VK_HOME == key )
	{
		lightPos.x = -10.0f;
		lightPos.y = 0.0f;
		firstSet = true;
	}



	return;
};

void	MouseKeyPress( WPARAM button , LPARAM pos )
{
	// if the left button is pressed
	if ( MK_LBUTTON == button )
	{
		if ( !firstSet )	// if the mouse keeps getting dragged
		{
			float	x , y;
			float	diffX , diffY;


			// get the mouse position
			x = LOWORD(pos);
			y = HIWORD(pos);
			// calculate the distance from the last function call
			diffX = mousePos.x - x;
			diffY = mousePos.y - y;

			// update the light position
			lightPos.x += diffX*-0.1f;
			lightPos.y += diffY*0.1f;
			// save the current mouse position
			mousePos.x = x;
			mousePos.y = y;
		}
		else
		{
			// if it is the first click we save the mouse position
			mousePos.x = LOWORD(pos);
			mousePos.y = HIWORD(pos);
			firstSet = false;
		}
	}
}


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	// on event call the necessary functions
	switch( msg )
	{
		case WM_DESTROY :
								Cleanup();
								PostQuitMessage( 0 );
								return 0;

		case WM_KEYDOWN :
								KeyPress(wParam,lParam);
								break;

		case WM_MOUSEMOVE :
								MouseKeyPress(wParam,lParam); break;
		case WM_LBUTTONDOWN :
		case WM_RBUTTONDOWN :
								SetCapture(hWnd);	// force the application to keep track of the mouse while the mouse button is pressed
								MouseKeyPress(wParam,lParam);
								break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
								ReleaseCapture();	// release the mouse capture flag when the mouse button is released
								break;
	}



	return DefWindowProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT ){
	// Register the window class
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, 
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		L"ACW1", NULL };
	RegisterClassEx( &wc );

	// Create the application's window
	HWND hWnd = CreateWindow( L"ACW1", L"ACW 1", 
		WS_OVERLAPPEDWINDOW, 100, 100, 600, 600,
		GetDesktopWindow(), NULL, wc.hInstance, NULL );

	// Initialize Direct3D
	if( SUCCEEDED( InitD3D( hWnd ) ) )
	{ 
		// Create the scene geometry
		if( SUCCEEDED( InitGeometry() ) )
		{
			// Show the window
			ShowWindow( hWnd, SW_SHOWDEFAULT );
			UpdateWindow( hWnd );

			// Enter the message loop
			MSG msg; 
			ZeroMemory( &msg, sizeof(msg) );
			while( msg.message!=WM_QUIT )
			{
				if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
				else
					Render(hWnd);
			}
		}
	}

	UnregisterClass( L"ACW1", wc.hInstance );
	return 0;
}


