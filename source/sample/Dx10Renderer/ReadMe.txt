	pVS = gDx10Renderer->CreateVertexShader("D:\\develop\\GaEngine\\bin\\TestVertexShader.shd", vector<pair<string, string>>());
	pPS = gDx10Renderer->CreatePixelShader("D:\\develop\\GaEngine\\bin\\TestPixelShader.shd", vector<pair<string, string>>());

	
	D3D10_BUFFER_DESC	vertexConstBufferDesc;
	vertexConstBufferDesc.ByteWidth = sizeof(VertexConstBuffer);
	vertexConstBufferDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	vertexConstBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vertexConstBufferDesc.MiscFlags = 0;
	vertexConstBufferDesc.Usage = D3D10_USAGE_DYNAMIC;

	HRESULT hr;
	hr = RendererDx10::s_pd3dDevice->CreateBuffer(&vertexConstBufferDesc, NULL, &gpVertexConstBuffer);
	hr = RendererDx10::s_pd3dDevice->CreateBuffer(&vertexConstBufferDesc, NULL, &gpVertexConstBuffer1);
	hr = RendererDx10::s_pd3dDevice->CreateBuffer(&vertexConstBufferDesc, NULL, &gpVertexConstBuffer2);
	if (FAILED(hr))
	{
		return;
	}


	D3D10_BLEND_DESC BlendState;
	ZeroMemory( &BlendState, sizeof( D3D10_BLEND_DESC ) );
	BlendState.BlendEnable[0] = FALSE;
	BlendState.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
	hr = RendererDx10::s_pd3dDevice->CreateBlendState( &BlendState, &g_pBlendStateNoBlend );
	if( FAILED( hr ) )
		return;

	// Create a rasterizer state to disable culling
	D3D10_RASTERIZER_DESC RSDesc;
	RSDesc.FillMode = D3D10_FILL_SOLID;
	RSDesc.CullMode = D3D10_CULL_NONE;
	RSDesc.FrontCounterClockwise = TRUE;
	RSDesc.DepthBias = 0;
	RSDesc.DepthBiasClamp = 0;
	RSDesc.SlopeScaledDepthBias = 0;
	RSDesc.ScissorEnable = FALSE;
	RSDesc.MultisampleEnable = TRUE;
	RSDesc.AntialiasedLineEnable = FALSE;
	hr = RendererDx10::s_pd3dDevice->CreateRasterizerState( &RSDesc, &g_pRasterizerStateNoCull );
	if( FAILED( hr ) )
		return;

	// Create a depth stencil state to enable less-equal depth testing
	D3D10_DEPTH_STENCIL_DESC DSDesc;
	ZeroMemory( &DSDesc, sizeof( D3D10_DEPTH_STENCIL_DESC ) );
	DSDesc.DepthEnable = TRUE;
	DSDesc.DepthFunc = D3D10_COMPARISON_LESS_EQUAL;
	DSDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	hr = RendererDx10::s_pd3dDevice->CreateDepthStencilState( &DSDesc, &g_pLessEqualDepth );
	if( FAILED( hr ) )
		return;
		
		
		
/////////////////////////////////////////////////////////////////////////////////////////////



	// Update the Constant Buffer
	VertexConstBuffer* pConstData;
	gpVertexConstBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, ( void** )&pConstData );
	pConstData->matrix = Matrix4f::IDENTITY;
	//pConstData->matrix1 = matView;
	//pConstData->matrix2 = matProj;
	gpVertexConstBuffer->Unmap();
	RendererDx10::s_pd3dDevice->VSSetConstantBuffers(1, 1, &gpVertexConstBuffer);	


	gpVertexConstBuffer1->Map( D3D10_MAP_WRITE_DISCARD, NULL, ( void** )&pConstData );
	pConstData->matrix = matView;
	gpVertexConstBuffer1->Unmap();
	RendererDx10::s_pd3dDevice->VSSetConstantBuffers(2, 1, &gpVertexConstBuffer1);


	gpVertexConstBuffer2->Map( D3D10_MAP_WRITE_DISCARD, NULL, ( void** )&pConstData );
	pConstData->matrix = matProj;
	gpVertexConstBuffer2->Unmap();
	RendererDx10::s_pd3dDevice->VSSetConstantBuffers(3, 1, &gpVertexConstBuffer2);
	
	


	//RendererDx10::s_pd3dDevice->OMSetBlendState( g_pBlendStateNoBlend, 0, 0xffffffff );
	//RendererDx10::s_pd3dDevice->RSSetState( g_pRasterizerStateNoCull );
	//RendererDx10::s_pd3dDevice->OMSetDepthStencilState( g_pLessEqualDepth, 0 );
	//RendererDx10::s_pd3dDevice->VSSetShader(gpVertexShader);


	//HALSetVertexShader(pVS.get());
	//RendererDx10::s_pd3dDevice->GSSetShader( NULL );
	////RendererDx10::s_pd3dDevice->PSSetShader(gpPixelShader);
	//HALSetPixelShader(pPS.get());