////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_TextureShader = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	struct filename {
		char* objName;
		WCHAR* textureName;
	};
	filename* filenames;

	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	
	// Create the model object.
	m_ModelCount = 4;
	m_Model = new ModelClass[m_ModelCount];
	if(!m_Model)
	{
		return false;
	}
	filenames = new filename[m_ModelCount];
	if (!filenames)
	{
		return false;
	}

	filenames[0].objName = (char*)"../DirectX11_HW2/data/Dolphin.obj"; //돌고래
	filenames[0].textureName = (WCHAR*)L"../DirectX11_HW2/data/Dolphin.dds";

	filenames[1].objName = (char*)"../DirectX11_HW2/data/hammer.obj";
	filenames[1].textureName = (WCHAR*)L"../DirectX11_HW2/data/hammer.dds";

	filenames[2].objName = (char*)"../DirectX11_HW2/data/amongus.obj"; //어몽어스
	filenames[2].textureName = (WCHAR*)L"../DirectX11_HW2/data/amongus.dds";

	filenames[3].objName = (char*)"../DirectX11_HW2/data/Cube.obj";
	filenames[3].textureName = (WCHAR*)L"../DirectX11_HW2/data/tile.dds";

	for (int i = 0; i < m_ModelCount; i++)
	{
		if (!&m_Model[i])
		{
			return false;
		}
		// Initialize the model object.
		result = m_Model[i].Initialize(m_D3D->GetDevice(), filenames[i].objName, filenames[i].textureName);
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
			return false;
		}
	}
	// Initialize the model object.
	//result = m_Model->Initialize(m_D3D->GetDevice(), "../DirectX11_HW2/data/Diamond.obj", L"../DirectX11_HW2/data/seafloor.dds");
//	result = m_Model->Initialize(m_D3D->GetDevice(), "../Engine/data/chair.txt", L"../Engine/data/chair_d.dds");
	/*if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}*/

	// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;
	if(!m_TextureShader)
	{
		return false;
	}

	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the texture shader object.
	if(m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the model object.
	if(m_Model)
	{
		for (int i = 0; i < m_ModelCount; i++)
			m_Model->Shutdown();

		delete[] m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


bool GraphicsClass::Frame()
{
	bool result;

	static float rotation = 0.0f;


	// Update the rotation variable each frame.
	rotation += (float)D3DX_PI * 0.01f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Render the graphics scene.
	result = Render(rotation);
	if(!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::Render(float rotation)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	D3DXMATRIX transform;
	D3DXMATRIX scale;
	D3DXMATRIX* m_worldMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Rotate the world matrix by the rotation value so that the triangle will spin.
	m_worldMatrix = new D3DXMATRIX[m_ModelCount];
	if (!m_worldMatrix)
	{
		return false;
	}
	for (int i = 0; i < m_ModelCount; i++)
	{
		m_worldMatrix[i] = worldMatrix;
	}

	for (int i = 0; i < 3; i++)
	{
		D3DXMatrixRotationY(&m_worldMatrix[i], rotation);
	}

	D3DXMatrixTranslation(&transform, 350, 2, 0);
	D3DXMatrixScaling(&scale, 0.01, 0.01, 0.01);
	m_worldMatrix[0] *= transform * scale;

	D3DXMatrixTranslation(&transform, -30, -20, 0);
	D3DXMatrixScaling(&scale, 0.1, 0.1, 0.1);
	m_worldMatrix[1] *= transform * scale;

	D3DXMatrixTranslation(&transform, 0, -100, 0);
	D3DXMatrixScaling(&scale, 0.01, 0.01, 0.01);
	m_worldMatrix[2] *= transform * scale;

	D3DXMatrixTranslation(&transform, 0, -20, 0.5);
	D3DXMatrixScaling(&scale, 150, 1, 150);
	m_worldMatrix[3] *= transform * scale;

	for (int i = 0; i < m_ModelCount; i++)
	{
		worldMatrix = m_worldMatrix[i];
		m_Model[i].Render(m_D3D->GetDeviceContext());
		// Render the model using the texture shader.
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model[i].GetIndexCount(),
			worldMatrix, viewMatrix, projectionMatrix, m_Model[i].GetTexture());
		if (!result)
		{
			return false;
		}
	}

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}