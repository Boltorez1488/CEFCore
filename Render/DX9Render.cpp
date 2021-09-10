#include "DX9Render.h"

DX9Render::DX9Render(LPDIRECT3DDEVICE9 pDevice) {
	this->pDevice = pDevice;
	D3DDEVICE_CREATION_PARAMETERS d;
	pDevice->GetCreationParameters(&d);
	hWnd = d.hFocusWindow;
}

DX9Render::~DX9Render() {
}

HWND DX9Render::get_hwnd() const {
	return hWnd;
}

LPDIRECT3DDEVICE9 DX9Render::get_device() const {
	return pDevice;
}

RECT DX9Render::get_rect() const {
	RECT rc;
	GetClientRect(hWnd, &rc);
	return rc;
}

void DX9Render::draw_texture(LPDIRECT3DTEXTURE9 texture, RECT* rDest, D3DCOLOR vertexColour, float rotate) const {
	const DWORD D3DFVF_TLVERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

	struct TLVERTEX {
		float x, y, z, rhw;
		D3DCOLOR color;
		float u;
		float v;
	};

	IDirect3DVertexBuffer9* vertex_buffer;

	// Set vertex shader.
	pDevice->SetVertexShader(nullptr);
	pDevice->SetFVF(D3DFVF_TLVERTEX);

	// Create vertex buffer.
	pDevice->CreateVertexBuffer(sizeof(TLVERTEX) * 4, NULL,
		D3DFVF_TLVERTEX, D3DPOOL_MANAGED, &vertex_buffer, nullptr);
	pDevice->SetStreamSource(0, vertex_buffer, 0, sizeof(TLVERTEX));

	TLVERTEX* vertices;

	//Lock the vertex buffer
	vertex_buffer->Lock(0, 0, reinterpret_cast<void**>(&vertices), NULL);

	//Setup vertices
	//A -0.5f modifier is applied to vertex coordinates to match texture
	//and screen coords. Some drivers may compensate for this
	//automatically, but on others texture alignment errors are introduced
	//More information on this can be found in the Direct3D 9 documentation
	vertices[0].color = vertexColour;
	vertices[0].x = static_cast<float>(rDest->left) - 0.5f;
	vertices[0].y = static_cast<float>(rDest->top) - 0.5f;
	vertices[0].z = 0.0f;
	vertices[0].rhw = 1.0f;
	vertices[0].u = 0.0f;
	vertices[0].v = 0.0f;

	vertices[1].color = vertexColour;
	vertices[1].x = static_cast<float>(rDest->right) - 0.5f;
	vertices[1].y = static_cast<float>(rDest->top) - 0.5f;
	vertices[1].z = 0.0f;
	vertices[1].rhw = 1.0f;
	vertices[1].u = 1.0f;
	vertices[1].v = 0.0f;

	vertices[2].color = vertexColour;
	vertices[2].x = static_cast<float>(rDest->right) - 0.5f;
	vertices[2].y = static_cast<float>(rDest->bottom) - 0.5f;
	vertices[2].z = 0.0f;
	vertices[2].rhw = 1.0f;
	vertices[2].u = 1.0f;
	vertices[2].v = 1.0f;

	vertices[3].color = vertexColour;
	vertices[3].x = static_cast<float>(rDest->left) - 0.5f;
	vertices[3].y = static_cast<float>(rDest->bottom) - 0.5f;
	vertices[3].z = 0.0f;
	vertices[3].rhw = 1.0f;
	vertices[3].u = 0.0f;
	vertices[3].v = 1.0f;

	//Unlock the vertex buffer
	vertex_buffer->Unlock();

	//Set texture
	pDevice->SetTexture(0, texture);

	//Draw image
	pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
}

void DX9Render::begin() {
	pDevice->GetFVF(&fvf);

	pDevice->GetVertexShader(&vshader);
	pDevice->GetPixelShader(&pshader);

	pDevice->GetRenderState(D3DRS_LIGHTING, &d3DRS_LIGHTING);
	pDevice->GetRenderState(D3DRS_FOGENABLE, &d3DRS_FOGENABLE);
	pDevice->GetRenderState(D3DRS_ZENABLE, &d3DRS_ZENABLE);
	pDevice->GetRenderState(D3DRS_ALPHATESTENABLE, &d3DRS_ALPHATESTENABLE);
	pDevice->GetRenderState(D3DRS_CULLMODE, &d3DRS_CULLMODE);
	pDevice->GetRenderState(D3DRS_FILLMODE, &d3DRS_FILLMODE);
	pDevice->GetRenderState(D3DRS_SCISSORTESTENABLE, &d3DRS_SCISSORTESTENABLE);
	pDevice->GetRenderState(D3DRS_ZWRITEENABLE, &d3DRS_ZWRITEENABLE);
	pDevice->GetRenderState(D3DRS_MULTISAMPLEANTIALIAS, &d3DRS_MULTISAMPLEANTIALIAS);

	pDevice->GetSamplerState(0, D3DSAMP_ADDRESSU, &d3DSAMP_ADDRESSU);
	pDevice->GetSamplerState(0, D3DSAMP_ADDRESSV, &d3DSAMP_ADDRESSV);

	pDevice->GetTextureStageState(0, D3DTSS_COLORARG1, &d3DTSS_COLORARG1);
	pDevice->GetTextureStageState(0, D3DTSS_COLORARG2, &d3DTSS_COLORARG2);
	pDevice->GetTextureStageState(0, D3DTSS_COLOROP, &d3DTSS_COLOROP0);

	pDevice->GetTextureStageState(0, D3DTSS_ALPHAARG1, &d3DTSS_ALPHAARG1);
	pDevice->GetTextureStageState(0, D3DTSS_ALPHAARG2, &d3DTSS_ALPHAARG2);
	pDevice->GetTextureStageState(0, D3DTSS_ALPHAOP, &d3DTSS_ALPHAOP);

	pDevice->GetSamplerState(0, D3DSAMP_MINFILTER, &d3DSAMP_MINFILTER);
	pDevice->GetSamplerState(0, D3DSAMP_MAGFILTER, &d3DSAMP_MAGFILTER);

	pDevice->GetTextureStageState(1, D3DTSS_COLOROP, &d3DTSS_COLOROP1);

	pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &d3DRS_ALPHABLENDENABLE);
	pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &d3DRS_SRGBWRITEENABLE);

	pDevice->GetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, &d3DRS_SEPARATEALPHABLENDENABLE);
	pDevice->GetRenderState(D3DRS_SRCBLEND, &d3DRS_SRCBLEND);
	pDevice->GetRenderState(D3DRS_DESTBLEND, &d3DRS_DESTBLEND);
	pDevice->GetRenderState(D3DRS_SRCBLENDALPHA, &d3DRS_SRCBLENDALPHA);
	pDevice->GetRenderState(D3DRS_DESTBLENDALPHA, &d3DRS_DESTBLENDALPHA);

	//##################################################################################

	pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);

	pDevice->SetVertexShader(nullptr);
	pDevice->SetPixelShader(nullptr);

	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);

	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);

	pDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
}

void DX9Render::end() const {
	pDevice->SetRenderState(D3DRS_LIGHTING, d3DRS_LIGHTING);
	pDevice->SetRenderState(D3DRS_FOGENABLE, d3DRS_FOGENABLE);
	pDevice->SetRenderState(D3DRS_ZENABLE, d3DRS_ZENABLE);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, d3DRS_ALPHATESTENABLE);
	pDevice->SetRenderState(D3DRS_CULLMODE, d3DRS_CULLMODE);
	pDevice->SetRenderState(D3DRS_FILLMODE, d3DRS_FILLMODE);
	pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, d3DRS_SCISSORTESTENABLE);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, d3DRS_ZWRITEENABLE);
	pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, d3DRS_MULTISAMPLEANTIALIAS);

	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, d3DSAMP_ADDRESSU);
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, d3DSAMP_ADDRESSV);

	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, d3DTSS_COLORARG1);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, d3DTSS_COLORARG2);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, d3DTSS_COLOROP0);

	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, d3DTSS_ALPHAARG1);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, d3DTSS_ALPHAARG2);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, d3DTSS_ALPHAOP);

	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, d3DSAMP_MINFILTER);
	pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, d3DSAMP_MAGFILTER);

	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, d3DTSS_COLOROP1);

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, d3DRS_ALPHABLENDENABLE);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, d3DRS_SRGBWRITEENABLE);

	pDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, d3DRS_SEPARATEALPHABLENDENABLE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, d3DRS_SRCBLEND);
	pDevice->SetRenderState(D3DRS_DESTBLEND, d3DRS_DESTBLEND);
	pDevice->SetRenderState(D3DRS_SRCBLENDALPHA, d3DRS_SRCBLENDALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLENDALPHA, d3DRS_DESTBLENDALPHA);

	pDevice->SetFVF(fvf);

	pDevice->SetVertexShader(vshader);
	pDevice->SetPixelShader(pshader);
}
